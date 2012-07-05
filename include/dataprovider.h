/*
  The data provider provides information on which data series are currently available.
  It also provides access to the database.
*/

#ifndef DATAPROVIDER_H
#define DATAPROVIDER_H

#include "common.h"
#include "databaseadapter.h"
#include "serializable.h"
#include "value.h"

#include <EIDescription.h>
#include <QMap>
#include <QObject>
#include <QtGlobal>
#include <memory>

class AbstractDataSeries;

/**
 * @brief This class provides easy access to the underlying DB
 */
class DataProvider : public QObject, public Serializable
{
    Q_OBJECT
public:
    DataProvider(QString const& filename, QObject*);
    ~DataProvider();

    /**
     * @return A list containing the full name of each data series.
     */
    QList<QString> getDataSeriesList() const;

    /**
     * @return The data series with the given name.
     *      0 if there is no such data series.
     */
    AbstractDataSeries* getDataSeries(const QString &fullName) const;

    /**
     * @return A list of all data series.
     */
    QList<AbstractDataSeries*> getDataSeries() const;

    /**
     * Change the offset of a series
     */
    void changeOffset(const QString &dataSeriesName, qint64 offset);

    /**
     * @return the filename of the used database
     */
    QString getDBFileName();

    /**
     * @brief Copy all data within a given range to a new DB
     * if range is -1, -1 copy all data
     */
    void copyDB(QString filename, qint64 begin, qint64 end);

    DatabaseAdapter const& getDB() const;

    /**
     * close the DB explicitly this is used because if we use deleteLater the DB would be closed to late.
     */
    void closeDB();

    void save(QVariantMap *qvm, qint64 , qint64 );
    void load(QVariantMap *qvm);

signals:
    void unknownDataSeries();

    /**
     * emitted when a new data packet arrived
     */
    void newMax(qint64 maxTimestamp);

public slots:
    void onNewSender(EIDescriptionWrapper);

    void onNewData(qint64 timestamp, QString fullDataSeriesName, Value value);

private:
    void addSeries(QString const& device_name, QString const& name, EI::DataSeriesInfo const& info);

    QMap<QString, AbstractDataSeries*> dataSeries;

    std::unique_ptr<DatabaseAdapter> db;

    QString filename;

    qint64 currentMax;

    bool temporaryDB;
};

#endif // DATAPROVIDER_H
