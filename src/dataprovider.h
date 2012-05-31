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

public slots:
    void onNewDataSeries(const QString &deviceName, const QString &dataSeriesName, DataProperty::Properties properties);

    void onNewData(qint64 timestamp, const QString &fullDataSeriesName, const Value &value);

private:
    QMap<QString, AbstractDataSeries*> dataSeries;
};

#endif // DATAPROVIDER_H
