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
    QMap<qint64, T> getData(QString const& key) const;
    template<typename T>
    QMap<qint64, T> getData(QString const& key, qint64 start, qint64 end) const;

private:
    Sqlite::DB db;

    mutable Sqlite::PreparedStatement stmtAdd;
    mutable Sqlite::PreparedStatement stmtSelectTime;
    mutable Sqlite::PreparedStatement stmtSelect;
};

#endif // DATABASEADAPTER_H
