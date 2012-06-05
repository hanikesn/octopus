#include "dataprovider.h"

#include "common.h"
#include "doubleseries.h"
#include "stringseries.h"
#include "value.h"

DataProvider::DataProvider() :
    currentMax(0)
{
    addData();
}

void DataProvider::addData()
{
    onNewDataSeries("Dummy", "Interpolatable", Data::INTERPOLATABLE);
    onNewDataSeries("Dummy", "Discrete", Data::STATEFUL);

    for (int j=0; j<500; ++j)
    {
      double d = j/15.0 * 5*3.14 + 0.01;
      onNewData(d*1000000, "Dummy.Interpolatable", Value(7*sin(d)/d + 2));
      onNewData(d*1000000, "Dummy.Discrete", Value("ping"));
    }
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

        if (timestamp > currentMax) {
            currentMax = timestamp;
            emit newMax(timestamp);
        }
    } else {
        emit unknownDataSeries();
    }
}
