#include "databaseadapter.h"
#include "value.h"
#include "common.h"

DatabaseAdapter::DatabaseAdapter(const QString &file)
    : db(file.toLocal8Bit().data())
{
    db.execute("PRAGMA journal_mode=WAL");
    db.execute("PRAGMA locking_mode=EXCLUSIVE");
    db.execute("PRAGMA synchronous=OFF");

    if(db.execute("CREATE TABLE IF NOT EXISTS data (name TEXT, time INT, value BLOB);") != Sqlite::DB::Done)
        throw std::exception();
    if(db.execute("CREATE TABLE IF NOT EXISTS series (name TEXT, properties INT);") != Sqlite::DB::Done)
        throw std::exception();

    if(db.execute("CREATE INDEX IF NOT EXISTS time_name_idx on data(name,time);") != Sqlite::DB::Done)
        throw std::exception();

    stmtAdd = db.prepare("INSERT INTO data VALUES (?,?,?);");
    stmtSelectTime = db.prepare("SELECT time, value FROM data WHERE name=? and time<=? and time>=?;");
    stmtSelect = db.prepare("SELECT time, value FROM data WHERE name=?;");
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
    stmtSelect.reset();
    stmtSelect << toStdString(key);

    return fillMap<T>(stmtSelect);
}

template QMap<qint64, double> DatabaseAdapter::getData<double>(QString const& key, qint64 start, qint64 end) const;
template QMap<qint64, QString> DatabaseAdapter::getData<QString>(QString const& key, qint64 start, qint64 end) const;

template<typename T>
QMap<qint64, T> DatabaseAdapter::getData(QString const& key, qint64 start, qint64 end) const
{
    stmtSelectTime.reset();
    stmtSelectTime << toStdString(key) << start << end;

    return fillMap<T>(stmtSelectTime);
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
    stmtAdd.reset();
    stmtAdd << toStdString(key) << timestamp << value;
    if(stmtAdd.execute() != stmtAdd.done())
        throw std::exception();
}
