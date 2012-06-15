#include "databaseadapter.h"
#include "value.h"
#include "common.h"

DatabaseAdapter::DatabaseAdapter(const QString &file)
    : db(file.toLocal8Bit().data())
{
    db.execute("PRAGMA journal_mode=WAL;");
    db.execute("PRAGMA locking_mode=EXCLUSIVE;");
    db.execute("PRAGMA synchronous=OFF;");
    db.execute("PRAGMA foreign_keys = ON;");

    if(db.execute("CREATE TABLE IF NOT EXISTS data (name TEXT, time INT, value BLOB);") != Sqlite::DB::Done)
        throw std::exception();
    if(db.execute("CREATE INDEX IF NOT EXISTS time_name_idx on data(name,time);") != Sqlite::DB::Done)
        throw std::exception();

    if(db.execute("CREATE TABLE IF NOT EXISTS senders (name TEXT PRIMARY KEY, devicetype TEXT);") != Sqlite::DB::Done)
        throw std::exception();
    if(db.execute("CREATE INDEX IF NOT EXISTS name_idx on senders(name);") != Sqlite::DB::Done)
        throw std::exception();

    if(db.execute("CREATE TABLE IF NOT EXISTS series (sender TEXT, name TEXT, type TEXT, properties INT, misc TEXT, min FLOAT, max FLOAT, FOREIGN KEY(sender) REFERENCES senders(name));") != Sqlite::DB::Done)
        throw std::exception();
    if(db.execute("CREATE INDEX IF NOT EXISTS sender_name_idx on series(sender, name);") != Sqlite::DB::Done)
        throw std::exception();

    stmtAddData = db.prepare("INSERT INTO data VALUES (?,?,?);");
    stmtSelectDataWithTime = db.prepare("SELECT time, value FROM data WHERE name=? and time<=? and time>=?;");
    stmtSelectData = db.prepare("SELECT time, value FROM data WHERE name=?;");

    stmtAddSender = db.prepare("INSERT OR REPLACE INTO senders VALUES (?,?)");
    stmtSelectSender = db.prepare("SELECT * from senders");

    stmtAddSeries = db.prepare("INSERT OR REPLACE INTO series VALUES (?,?,?,?,?,?,?)");
    stmtSelectSeries = db.prepare("SELECT * from series");
}

template QMap<qint64, double> DatabaseAdapter::getData<double>(QString const& key) const;
template QMap<qint64, QString> DatabaseAdapter::getData<QString>(QString const& key) const;

static Sqlite::Row& operator >>(Sqlite::Row& row, QString& value)
{
    std::string str;
    row >> str;
    value = fromStdString(str);
    return row;
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

template<typename T>
QMap<qint64, T> DatabaseAdapter::getData(QString const& key) const
{
    stmtSelectData.reset();
    stmtSelectData << toStdString(key);

    return fillMap<T>(stmtSelectData);
}

template QMap<qint64, double> DatabaseAdapter::getData<double>(QString const& key, qint64 start, qint64 end) const;
template QMap<qint64, QString> DatabaseAdapter::getData<QString>(QString const& key, qint64 start, qint64 end) const;

template<typename T>
QMap<qint64, T> DatabaseAdapter::getData(QString const& key, qint64 start, qint64 end) const
{
    stmtSelectDataWithTime.reset();
    stmtSelectDataWithTime << toStdString(key) << start << end;

    return fillMap<T>(stmtSelectDataWithTime);
}

template void DatabaseAdapter::add<Value>(QString const& key, qint64 timestamp, Value const& value);

static Sqlite::PreparedStatement& operator <<(Sqlite::PreparedStatement& stmt, Value const& value)
{
    switch(value.getType())
    {
    case Value::DOUBLE:
        stmt << value.asDouble();
        break;
    case Value::STRING:
        auto const& val = value.asString().toUtf8();
        stmt << std::string(val.constData(), val.length());
        break;
    }

    return stmt;
}

template<typename T>
void DatabaseAdapter::add(QString const& key, qint64 timestamp, T const& value)
{
    stmtAddData.reset();
    stmtAddData << toStdString(key) << timestamp << value;
    if(stmtAddData.execute() != stmtAddData.done())
        throw std::exception();
}

void DatabaseAdapter::addSender(QString const& name, QString const& device_type)
{
    stmtAddSender.reset();
    stmtAddSender << toStdString(name) << toStdString(device_type);
    if(stmtAddSender.execute() != stmtAddSender.done())
        throw std::exception();
}

QMap<QString, QString> DatabaseAdapter::getSenders()
{
    stmtSelectSender.reset();

    auto result = stmtSelectSender.execute();

    QMap<QString, QString> map;
    std::for_each(result, stmtSelectSender.done(),
                  [&map](Sqlite::Row r)
    {
        QString name;
        QString device_type;
        r >> name >> device_type;
        map.insert(name, device_type);
    });

    return std::move(map);
}
