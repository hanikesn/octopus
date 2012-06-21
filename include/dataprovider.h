#ifndef DATAPROVIDER_H
#define DATAPROVIDER_H

#include "common.h"
#include "databaseadapter.h"
#include "value.h"

#include <EIDescription.h>
#include <QMap>
#include <QObject>
#include <QtGlobal>
#include <memory>

class AbstractDataSeries;

class DataProvider : public QObject
{
    Q_OBJECT
public:
    DataProvider(QString const& filename, QObject*);
    ~DataProvider();

    /**
     * @return The full name of each known data series.
     */
    QList<QString> getDataSeriesList() const;

    /**
     * @return The data series with the given name.
     *      0 if there is no such data series.
     */
    AbstractDataSeries* getDataSeries(const QString &fullName) const;

    /**
     * @return the filename of the used database
     */
    QString getDBFileName();

    /**
     * @brief Moves the database to a new location
     */
    void moveDB(QString const& newFilename);

    DatabaseAdapter const& getDB() const;

    void closeDB();

signals:
    void unknownDataSeries();

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
};

#endif // DATAPROVIDER_H
