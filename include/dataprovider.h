#ifndef DATAPROVIDER_H
#define DATAPROVIDER_H

#include "abstractdataseries.h"
#include "common.h"
#include "databaseadapter.h"

#include <QMap>
#include <QObject>
#include <QtGlobal>

class DataProvider : public QObject
{
    Q_OBJECT
public:
    DataProvider(QString const& filename);

    // TODO(Steffi): Remove (dummy func)
    void addData();

    /**
     * @return The full name of each known data series.
     */
    QList<QString> getDataSeriesList() const;

    /**
     * @return The data series with the given name.
     *      0 if there is no such data series.
     */
    AbstractDataSeries* getDataSeries(const QString &fullName) const;

signals:
    void unknownDataSeries();

    void newMax(qint64 maxTimestamp);

public slots:
    void onNewDataSeries(QString deviceName, QString dataSeriesName, Data::Properties properties);

    void onNewData(qint64 timestamp, QString fullDataSeriesName, Value value);

private:
    QMap<QString, AbstractDataSeries*> dataSeries;

    DatabaseAdapter db;

    qint64 currentMax;
};

#endif // DATAPROVIDER_H
