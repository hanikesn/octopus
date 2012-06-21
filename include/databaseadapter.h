#ifndef DATABASEADAPTER_H
#define DATABASEADAPTER_H

#include "sqlite.h"
#include "metadata.h"

#include <QMap>

class QString;
class Value;

class DatabaseAdapter
{
public:
    DatabaseAdapter(QString const& file);

    void add(QString const& key, qint64 timestamp, ::Value const& value);

    template<typename T>
    QMap<qint64, T> getData(QString const& key) const;
    template<typename T>
    QMap<qint64, T> getData(QString const& key, qint64 start, qint64 end) const;

    /**
     * Returns a query iterator for (name, time, value) from the db, the caller has to check the type of third column
     */
    Sqlite::PreparedStatement getData(QStringList const& keys, qint64 start, qint64 end) const;

    void addSender(EI::Description const& desc);
    QList<EI::Description> getSenders();

private:
    Sqlite::DB db;

    mutable Sqlite::PreparedStatement stmtAddDataString;
    mutable Sqlite::PreparedStatement stmtAddDataFloat;
    mutable Sqlite::PreparedStatement stmtSelectDataWithTimeString;
    mutable Sqlite::PreparedStatement stmtSelectDataWithTimeFloat;
    mutable Sqlite::PreparedStatement stmtSelectDataString;
    mutable Sqlite::PreparedStatement stmtSelectDataFloat;

    mutable Sqlite::PreparedStatement stmtAddSender;
    mutable Sqlite::PreparedStatement stmtSelectSender;

    mutable Sqlite::PreparedStatement stmtAddSeries;
    mutable Sqlite::PreparedStatement stmtSelectSeries;
};

#endif // DATABASEADAPTER_H
