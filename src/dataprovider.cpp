#include "dataprovider.h"

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

void DataProvider::onNewData(qint64 timestamp, const QString &fullDataSeriesName, const Value &value)
{
    if (dataSeries.contains(fullDataSeriesName)) {
        dataSeries.value(fullDataSeriesName)->addData(timestamp, value);
    } else {
        emit unknownDataSeries();
    }
}

void DataProvider::onNewDataSeries(const QString &deviceName, const QString &dataSeriesName, bool stateful)
{
    // TODO(Steffi) : Need to know which type of data series to create!
//    dataSeries.insert(deviceName + "." + dataSeriesName, new AbstractDataSeries(deviceName, dataSeriesName, stateful));
}
