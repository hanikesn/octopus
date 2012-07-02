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
    Sqlite::PreparedStatement getRawData(QStringList const& keys, qint64 start, qint64 end) const;

    void addSender(EI::Description const& desc);
    QList<EI::Description> getSenders();

    void getMinMaxTimeStamp(qint64& min, qint64& max) const;

    void copy(QString other, qint64 begin, qint64 end);

    qint64 getOffset(const QString& series) const;

    void changeOffset(const QString& series, qint64 offset);

private:
    void initDB(Sqlite::DB& db);

    Sqlite::DB db;

    mutable Sqlite::PreparedStatement stmtAddDataString;
    mutable Sqlite::PreparedStatement stmtAddDataFloat;
    mutable Sqlite::PreparedStatement stmtSelectDataWithTimeString;
    mutable Sqlite::PreparedStatement stmtSelectDataWithTimeFloat;
    mutable Sqlite::PreparedStatement stmtSelectDataString;
    mutable Sqlite::PreparedStatement stmtSelectDataFloat;
};

#endif // DATABASEADAPTER_H
