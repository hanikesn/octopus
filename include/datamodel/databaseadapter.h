#ifndef DATABASEADAPTER_H
#define DATABASEADAPTER_H

#include "common/metadata.h"
#include "datamodel/sqlite.h"

#include <QMap>

class QString;
class Value;

/**
 * @brief This class provides a thick wrapper around the underlying sqlite db.
 */
class DatabaseAdapter
{
public:
    DatabaseAdapter(QString const& file);

    /**
     * @brief Add a Value to the db. The offset of the series will be taken into account
     */
    void add(QString const& series, qint64 timestamp, ::Value const& value);

    template<typename T>
    QMap<qint64, T> getData(QString const& key) const;
    template<typename T>
    QMap<qint64, T> getData(QString const& key, qint64 start, qint64 end) const;

    /**
     * Returns a query iterator for (name, time, value) from the db, the caller has to check the type of the third column
     */
    Sqlite::PreparedStatement getRawData(QStringList const& keys, qint64 start, qint64 end) const;

    void addSender(EI::Description const& desc);
    QList<EI::Description> getSenders();

    /**
     * @brief Get the minimum and maximum timestamp of all the data stored
     */
    void getMinMaxTimeStamp(qint64& min, qint64& max) const;

    /**
     * @brief Copy parts of the db to another db. if begin==end==-1 the whole db is copied.
     */
    void copy(QString other, qint64 begin, qint64 end);

    /**
     * @brief Get the current offset of a dataseries
     */
    qint64 getOffset(const QString& series) const;

    /**
     * @brief Adjust the timestamps of a series with offset. A previously set offset is taken into account.
     */
    void changeOffset(const QString& series, qint64 offset);

private:
    void initDB(Sqlite::DB& db);

    Sqlite::DB db;

    /**
     * Some prepared statements will be cached for faster access
     */
    mutable Sqlite::PreparedStatement stmtAddDataString;
    mutable Sqlite::PreparedStatement stmtAddDataFloat;
    mutable Sqlite::PreparedStatement stmtSelectDataWithTimeString;
    mutable Sqlite::PreparedStatement stmtSelectDataWithTimeFloat;
    mutable Sqlite::PreparedStatement stmtSelectDataString;
    mutable Sqlite::PreparedStatement stmtSelectDataFloat;
};

#endif // DATABASEADAPTER_H
