#include "databaseadapter.h"
#include "value.h"
#include "common.h"
#include "DBUtil.h"

#include <QFile>
#include <QPair>
#include <QStringList>

DatabaseAdapter::DatabaseAdapter(const QString &file)
    : db(file.toLocal8Bit().data())
{
    initDB(db);

    stmtAddDataFloat = db.prepare("INSERT INTO data_float (name, time, value) VALUES (?,?,?);");
    stmtAddDataString = db.prepare("INSERT INTO data_string (name, time, value) VALUES (?,?,?);");
    stmtSelectDataWithTimeFloat = db.prepare("SELECT time, value FROM data_float WHERE name=? and time>=? and time<=?;");
    stmtSelectDataWithTimeString = db.prepare("SELECT time, value FROM data_string WHERE name=? and time>=? and time<=?;");
    stmtSelectDataFloat = db.prepare("SELECT time, value FROM data_float WHERE name=?;");
    stmtSelectDataString = db.prepare("SELECT time, value FROM data_string WHERE name=?;");
}

void DatabaseAdapter::initDB(Sqlite::DB &db)
{
    db.execute("PRAGMA journal_mode=WAL;");
    db.execute("PRAGMA locking_mode=EXCLUSIVE;");
    db.execute("PRAGMA synchronous=OFF;");
    db.execute("PRAGMA foreign_keys = ON;");

    if(db.execute("CREATE TABLE IF NOT EXISTS data_string (name TEXT, time INT, value TEXT);") != Sqlite::DB::Done)
        throw std::exception();
    if(db.execute("CREATE INDEX IF NOT EXISTS time_name_idx on data_string(name,time);") != Sqlite::DB::Done)
        throw std::exception();

    if(db.execute("CREATE TABLE IF NOT EXISTS data_float (name TEXT, time INT, value FLOAT);") != Sqlite::DB::Done)
        throw std::exception();
    if(db.execute("CREATE INDEX IF NOT EXISTS time_name_idx on data_float(name,time);") != Sqlite::DB::Done)
        throw std::exception();

    if(db.execute("CREATE TABLE IF NOT EXISTS senders (name TEXT PRIMARY KEY, devicetype TEXT);") != Sqlite::DB::Done)
        throw std::exception();
    if(db.execute("CREATE INDEX IF NOT EXISTS name_idx on senders(name);") != Sqlite::DB::Done)
        throw std::exception();

    if(db.execute("CREATE TABLE IF NOT EXISTS series (name TEXT, type TEXT, properties INT, misc TEXT, min FLOAT, max FLOAT, offset INT, sender TEXT, FOREIGN KEY(sender) REFERENCES senders(name));") != Sqlite::DB::Done)
        throw std::exception();
    if(db.execute("CREATE INDEX IF NOT EXISTS sender_name_idx on series(sender, name);") != Sqlite::DB::Done)
        throw std::exception();
}

template<typename T>
static QMap<qint64, T> fillMap(Sqlite::PreparedStatement& stmt, qint64 offset)
{
    auto result = stmt.execute();

    QMap<qint64, T> map;
    std::for_each(result, stmt.done(),
                  [&map,offset](Sqlite::Row r)
    {
        qint64 timestamp;
        T value;
        r >> timestamp >> value;
        map.insert(timestamp +  offset, value);
    });

    return std::move(map);
}

template<>
QMap<qint64, double> DatabaseAdapter::getData(QString const& key, qint64 offset) const
{
    stmtSelectDataFloat.reset();
    stmtSelectDataFloat << toStdString(key);

    return fillMap<double>(stmtSelectDataFloat, offset);
}

template<>
QMap<qint64, QString> DatabaseAdapter::getData(QString const& key, qint64 offset) const
{
    stmtSelectDataString.reset();
    stmtSelectDataString << toStdString(key);

    return fillMap<QString>(stmtSelectDataString, offset);
}

template<>
QMap<qint64, double> DatabaseAdapter::getData(QString const& key, qint64 start, qint64 end, qint64 offset) const
{
    stmtSelectDataWithTimeFloat.reset();
    stmtSelectDataWithTimeFloat << toStdString(key) << start + offset << end + offset;

    return fillMap<double>(stmtSelectDataWithTimeFloat, offset);
}

template<>
QMap<qint64, QString> DatabaseAdapter::getData(QString const& key, qint64 start, qint64 end, qint64 offset) const
{
    stmtSelectDataWithTimeString.reset();
    stmtSelectDataWithTimeString << toStdString(key) << start + offset << end + offset;

    return fillMap<QString>(stmtSelectDataWithTimeString, offset);
}

void DatabaseAdapter::add(QString const& key, qint64 timestamp, Value const& value)
{
    Sqlite::PreparedStatement* stmt;
    switch(value.getType()) {
    case Value::DOUBLE:
        stmt = &stmtAddDataFloat;
        break;
    case Value::STRING:
        stmt = &stmtAddDataString;
        break;
    case Value::EMPTY:
        return;
    }

    stmt->reset();

    *stmt << toStdString(key) << timestamp << value;
    if(stmt->execute() != stmt->done())
        throw std::exception();
}

static std::string convert(EI::Value::Type t)
{
    switch(t)
    {
    case EI::Value::DOUBLE:
        return "DOUBLE";
        break;
    case EI::Value::STRING:
        return "STRING";
        break;
    case EI::Value::EMPTY:
        return "EMPTY";
        break;
    }

    return "EMPTY";
}

Sqlite::PreparedStatement DatabaseAdapter::getRawData(QStringList const& keys, qint64 start, qint64 end) const
{
    // TODO(steffen) offset beim abfragen beachten!
    QString where = "WHERE time>=? and time<=?";

    if(keys.length() > 0) {
        where += " and (name=?";
    }
    for(int i = 1; i < keys.length(); i++) {
        where += " OR name=?";
    }
    if(keys.length() > 0) {
        where += ") ";
    }

    QString query = "SELECT * FROM ( "
            " SELECT name, time, value FROM data_float " + where +
            " UNION ALL "
            " SELECT name, time, value FROM data_string " + where +
            " ) tmp ORDER BY tmp.time";

    Sqlite::PreparedStatement stmt = db.prepare(toStdString(query));

    for(int i=0; i < 2; i++) {
        stmt << start << end;
        foreach(QString key, keys) {
            stmt << toStdString(key);
        }
    }

    return std::move(stmt);
}

static EI::Value::Type convertType(std::string const& str)
{
    if(str == "DOUBLE")
        return EI::Value::DOUBLE;
    else if(str == "STRING")
        return EI::Value::STRING;
    return EI::Value::EMPTY;
}

void DatabaseAdapter::addSender(EI::Description const& desc)
{
    Sqlite::PreparedStatement stmtAddSender = db.prepare("INSERT OR REPLACE INTO senders (name, devicetype) VALUES (?,?)");
    stmtAddSender << desc.getSender() << desc.getDeviceType();
    if(stmtAddSender.execute() != stmtAddSender.done())
        throw std::exception();

    Sqlite::PreparedStatement stmtAddSeries = db.prepare("INSERT OR REPLACE INTO series (name, type, properties, misc, min, max, offset, sender) VALUES (?,?,?,?,?,?,?,?)");

    foreach(EI::DataSeriesInfoMap::value_type series, desc.getDataSeries())
    {
        //name TEXT, type TEXT, properties INT, misc TEXT, min FLOAT, max FLOAT, sender TEXT
        stmtAddSeries.reset();
        stmtAddSeries << series.first
                      << convert(series.second.getType())
                      << (sqlite3_int64)series.second.getProperties()
                      << series.second.getMisc()
                      << series.second.getMin()
                      << series.second.getMax()
                      << (sqlite3_int64)0 // default offset is zero
                      << desc.getSender();
        if(stmtAddSeries.execute() != stmtAddSeries.done())
            throw std::exception();
    }
}

QList<EI::Description> DatabaseAdapter::getSenders()
{
    Sqlite::PreparedStatement stmtSelectSender = db.prepare("SELECT * from senders");
    auto result = stmtSelectSender.execute();

    QList<QPair<std::string, std::string>> senders;
    std::for_each(result, stmtSelectSender.done(),
                  [&senders](Sqlite::Row r)
    {
        std::string name, device_type;
        r >> name >> device_type;
        senders.append(QPair<std::string, std::string>(name, device_type));
    });

    Sqlite::PreparedStatement stmtSelectSeries = db.prepare("SELECT name, type, properties, misc, min, max from series where sender=?");
    QList<EI::Description> list;
    foreach(auto const& p, senders)
    {
        // name TEXT, type TEXT, properties INT, misc TEXT, min FLOAT, max FLOAT, sender TEXT
        stmtSelectSeries.reset();
        stmtSelectSeries << p.first;
        EI::Description desc(p.first, p.second);
        auto series_result = stmtSelectSeries.execute();
        std::for_each(series_result, stmtSelectSeries.done(),
                      [&list, &desc](Sqlite::Row r)
        {
            std::string name, type, misc;
            sqlite3_int64 props;
            double min, max;
            r >> name
              >> type
              >> props
              >> misc
              >> min
              >> max;
            desc.addDataSeries(name, EI::DataSeriesInfo(convertType(type),props, misc, min, max));
        });
        list.append(desc);
    }

    return std::move(list);
}

void DatabaseAdapter::getMinMaxTimeStamp(qint64 &min, qint64 &max) const
{
    Sqlite::PreparedStatement stmt = db.prepare(
                "SELECT MIN(min), MAX(max) FROM "
                    "("
                    "SELECT MIN(time) AS min, MAX(time) AS max from data_float "
                        "UNION ALL "
                    "SELECT MIN(time) AS min, MAX(time) AS max from data_string"
                    ");"
                );
    Sqlite::Row r = (*stmt.execute());
    r >> min >> max;
}


void DatabaseAdapter::copy(QString other, qint64 begin, qint64 end)
{
    {
        QFile::remove(other);
        // Create and close a DB
        Sqlite::DB otherDB(toStdString(other));
        initDB(otherDB);
    }

    Sqlite::PreparedStatement stmt = db.prepare("ATTACH DATABASE ? AS other;");

    stmt << toStdString(other);

    if(stmt.execute() != stmt.done())
        throw std::exception();

    if(db.execute("INSERT OR REPLACE INTO other.senders SELECT * FROM main.senders;") != db.Done)
        throw std::exception();

    if(db.execute("INSERT OR REPLACE INTO other.series SELECT * FROM main.series;") != db.Done)
        throw std::exception();

    std::string where;

    if(begin != -1 || end != -1)
    {
        where = " WHERE time>=? and time<=?";
    }

    // Don't forget to adjust the offsets
    stmt = db.prepare("INSERT INTO other.data_string SELECT name, time + ?, value FROM main.data_string;" + where + ";");
    if(where.length() > 0) {
        stmt << -begin << begin << end;
    } else {
        stmt << qint64(0);
    }
    if(stmt.execute() != stmt.done())
        throw std::exception();

    stmt = db.prepare("INSERT INTO other.data_float SELECT name, time + ?, value FROM main.data_float" + where + ";");
    if(where.length() > 0) {
        stmt << -begin << begin << end;
    } else {
        stmt << qint64(0);
    }
    if(stmt.execute() != stmt.done())
        throw std::exception();

    // TODO should always execute
    if(db.execute("DETACH DATABASE other;") != db.Done)
        throw std::exception();
}

qint64 DatabaseAdapter::getOffset(const QString &series)
{
    Sqlite::PreparedStatement stmt = db.prepare("SELECT offset FROM series WHERE name=?;");
    stmt << toStdString(series);

    qint64 offset;

    Sqlite::PreparedStatement::QueryIterator it = stmt.execute();

    // no result, default to zero
    if(it == stmt.done())
        return 0;

    Sqlite::Row r = *it;

    r >> offset;

    return offset;
}

void DatabaseAdapter::changeOffset(const QString &series, qint64 offset)
{
    db.execute("BEGIN;");

    qint64 newOffset = offset - getOffset(series);

    Sqlite::PreparedStatement stmt = db.prepare("UPDATE data_float set time=time+? WHERE name=?;");
    stmt << newOffset << toStdString(series);
    if(stmt.execute() != stmt.done())
        throw std::exception();

    stmt = db.prepare("UPDATE data_string set time=time+? WHERE name=?;");
    stmt << newOffset << toStdString(series);
    if(stmt.execute() != stmt.done())
        throw std::exception();

    stmt = db.prepare("UPDATE series SET offset=? WHERE name=?;");
    stmt << offset << toStdString(series);
    if(stmt.execute() != stmt.done())
        throw std::exception();

    db.execute("END;");
}
