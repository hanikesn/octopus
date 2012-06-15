#ifndef DATABASEADAPTER_H
#define DATABASEADAPTER_H

#include "sqlite.h"
#include "metadata.h"
#include "value.h"

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

    void addSender(EI::Description const& desc);
    QList<EI::Description> getSenders();

private:
    Sqlite::DB db;

    mutable Sqlite::PreparedStatement stmtAddData;
    mutable Sqlite::PreparedStatement stmtSelectDataWithTime;
    mutable Sqlite::PreparedStatement stmtSelectData;

    mutable Sqlite::PreparedStatement stmtAddSender;
    mutable Sqlite::PreparedStatement stmtSelectSender;

    mutable Sqlite::PreparedStatement stmtAddSeries;
    mutable Sqlite::PreparedStatement stmtSelectSeries;
};

#endif // DATABASEADAPTER_H
