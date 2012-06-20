#include "databaseadapter.h"
#include "value.h"
#include "common.h"
#include "DBUtil.h"

#include <QPair>
#include <QStringList>

DatabaseAdapter::DatabaseAdapter(const QString &file)
    : db(file.toLocal8Bit().data())
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

    if(db.execute("CREATE TABLE IF NOT EXISTS series (name TEXT, type TEXT, properties INT, misc TEXT, min FLOAT, max FLOAT, sender TEXT, FOREIGN KEY(sender) REFERENCES senders(name));") != Sqlite::DB::Done)
        throw std::exception();
    if(db.execute("CREATE INDEX IF NOT EXISTS sender_name_idx on series(sender, name);") != Sqlite::DB::Done)
        throw std::exception();

    stmtAddDataFloat = db.prepare("INSERT INTO data_float VALUES (?,?,?);");
    stmtAddDataString = db.prepare("INSERT INTO data_string VALUES (?,?,?);");
    stmtSelectDataWithTimeFloat = db.prepare("SELECT time, value FROM data_float WHERE name=? and time>=? and time<=?;");
    stmtSelectDataWithTimeString = db.prepare("SELECT time, value FROM data_string WHERE name=? and time>=? and time<=?;");
    stmtSelectDataFloat = db.prepare("SELECT time, value FROM data_float WHERE name=?;");
    stmtSelectDataString = db.prepare("SELECT time, value FROM data_string WHERE name=?;");

    stmtAddSender = db.prepare("INSERT OR REPLACE INTO senders VALUES (?,?)");
    stmtSelectSender = db.prepare("SELECT * from senders");

    stmtAddSeries = db.prepare("INSERT OR REPLACE INTO series VALUES (?,?,?,?,?,?,?)");
    stmtSelectSeries = db.prepare("SELECT * from series where sender=?");
}

template<typename T>
static QMap<qint64, T> fillMap(Sqlite::PreparedStatement& stmt)
{
    auto result = stmt.execute();

    QMap<qint64, T> map;
    std::for_each(result, stmt.done(),
                  [&map](Sqlite::Row r)
    {
        qint64 timestamp;
        T value;
        r >> timestamp >> value;
        map.insert(timestamp, value);
    });

    return std::move(map);
}

template<>
QMap<qint64, double> DatabaseAdapter::getData(QString const& key) const
{
    stmtSelectDataFloat.reset();
    stmtSelectDataFloat << toStdString(key);

    return fillMap<double>(stmtSelectDataFloat);
}

template<>
QMap<qint64, QString> DatabaseAdapter::getData(QString const& key) const
{
    stmtSelectDataString.reset();
    stmtSelectDataString << toStdString(key);

    return fillMap<QString>(stmtSelectDataString);
}

template<>
QMap<qint64, double> DatabaseAdapter::getData(QString const& key, qint64 start, qint64 end) const
{
    stmtSelectDataWithTimeFloat.reset();
    stmtSelectDataWithTimeFloat << toStdString(key) << start << end;

    return fillMap<double>(stmtSelectDataWithTimeFloat);
}

template<>
QMap<qint64, QString> DatabaseAdapter::getData(QString const& key, qint64 start, qint64 end) const
{
    stmtSelectDataWithTimeString.reset();
    stmtSelectDataWithTimeString << toStdString(key) << start << end;

    return fillMap<QString>(stmtSelectDataWithTimeString);
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

Sqlite::PreparedStatement DatabaseAdapter::getData(QStringList const& keys, qint64 start, qint64 end) const
{
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
    stmtAddSender.reset();
    stmtAddSender << desc.getSender() << desc.getDeviceType();
    if(stmtAddSender.execute() != stmtAddSender.done())
        throw std::exception();

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
                      << desc.getSender();
        if(stmtAddSeries.execute() != stmtAddSeries.done())
            throw std::exception();
    }
}

QList<EI::Description> DatabaseAdapter::getSenders()
{
    stmtSelectSender.reset();
    auto result = stmtSelectSender.execute();


    QList<QPair<std::string, std::string>> senders;
    std::for_each(result, stmtSelectSender.done(),
                  [&senders](Sqlite::Row r)
    {
        std::string name, device_type;
        r >> name >> device_type;
        senders.append(QPair<std::string, std::string>(name, device_type));
    });

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
            std::string name, type, misc, muell;
            sqlite3_int64 props;
            double min, max;
            r >> name
              >> type
              >> props
              >> misc
              >> min
              >> max
              >> muell;
            desc.addDataSeries(name, EI::DataSeriesInfo(convertType(type),props, misc, min, max));
        });
        list.append(desc);
    }

    return std::move(list);
}
