#ifndef DATABASEADAPTER_H
#define DATABASEADAPTER_H

#include "sqlite.h"

#include <QString>
#include <QMap>

class DatabaseAdapter
{
public:
    DatabaseAdapter(QString const& file);

    template<typename T>
    void add(QString const& key, qint64 timestamp, T const& value);


    template<typename T>
    QMap<qint64, T> getData(QString const& key);
    template<typename T>
    QMap<qint64, T> getData(QString const& key, qint64 start, qint64 end);

private:
    Sqlite::DB db;

    Sqlite::PreparedStatement stmtAdd;
    Sqlite::PreparedStatement stmtSelectTime;
    Sqlite::PreparedStatement stmtSelect;
};

#endif // DATABASEADAPTER_H
