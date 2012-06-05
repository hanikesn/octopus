#ifndef DATAPROVIDER_H
#define DATAPROVIDER_H

#include "abstractdataseries.h"
#include "common.h"

#include <QMap>
#include <QObject>
#include <QtGlobal>

class DataProvider : public QObject
{
    Q_OBJECT
public:
    DataProvider();

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
    void onNewDataSeries(const QString &deviceName, const QString &dataSeriesName, Data::Properties properties);

    void onNewData(qint64 timestamp, const QString &fullDataSeriesName, const Value &value);

private:
    QMap<QString, AbstractDataSeries*> dataSeries;

    qint64 currentMax;
};

#endif // DATAPROVIDER_H
