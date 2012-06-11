#include "databaseadapter.h"

DatabaseAdapter::DatabaseAdapter(const QString &file)
    : db(file.toLocal8Bit().data())
{
    if(db.execute("CREATE TABLE IF NOT EXISTS data (name TEXT, time INT, value BLOB);") != Sqlite::DB::Done)
        throw std::exception();
    if(db.execute("CREATE TABLE IF NOT EXISTS series (name TEXT, properties INT);") != Sqlite::DB::Done)
        throw std::exception();

    if(db.execute("CREATE INDEX IF NOT EXISTS time_name_idx on data(name,time);") != Sqlite::DB::Done)
        throw std::exception();
}
