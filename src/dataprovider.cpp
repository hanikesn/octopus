#include "dataprovider.h"

#include "common.h"
#include "doubleseries.h"
#include "stringseries.h"

DataProvider::DataProvider()
{
}

QList<QString> DataProvider::getDataSeriesList() const
{
    return dataSeries.keys();
}

AbstractDataSeries* DataProvider::getDataSeries(const QString &fullName) const
{
    return dataSeries.value(fullName);
}

void DataProvider::onNewDataSeries(const QString &deviceName, const QString &dataSeriesName, Data::Properties properties)
{
    AbstractDataSeries *series;
    if (properties & Data::INTERPOLATABLE) {
        series = new DoubleSeries(deviceName, dataSeriesName, properties);
    } else {
        series = new StringSeries(deviceName, dataSeriesName, properties);
    }

    dataSeries.insert(deviceName + "." + dataSeriesName, series);
}

void DataProvider::onNewData(qint64 timestamp, const QString &fullDataSeriesName, const Value &value)
{
    if (dataSeries.contains(fullDataSeriesName)) {
        dataSeries.value(fullDataSeriesName)->addData(timestamp, value);
    } else {
        emit unknownDataSeries();
    }
}
