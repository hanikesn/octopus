#include "dataprovider.h"

#include "common.h"
#include "doubleseries.h"
#include "stringseries.h"
#include "value.h"

#include <cmath>

DataProvider::DataProvider(QString const& filename) :
    db(filename), currentMax(0)
{
    addData();
}

void DataProvider::addData()
{
    onNewDataSeries("Dummy", "Interpolatable.x", Data::INTERPOLATABLE);
    onNewDataSeries("Dummy", "Interpolatable.y", Data::INTERPOLATABLE);
    onNewDataSeries("Dummy-2", "Interpolatable.x", Data::INTERPOLATABLE);
    onNewDataSeries("Dummy-2", "Interpolatable.y", Data::INTERPOLATABLE);
    onNewDataSeries("Dummy", "Discrete", Data::STATEFUL);

    for (int j=0; j<500; ++j)
    {
      double d = j/15.0 * 5*3.14 + 0.01;
      onNewData(d*1000000, "Dummy.Interpolatable.x", Value(7*sin(d)/d + 2));
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

void DataProvider::onNewDataSeries(QString deviceName, QString dataSeriesName, Data::Properties properties)
{
    AbstractDataSeries *series;
    if (properties & Data::INTERPOLATABLE) {
        series = new DoubleSeries(deviceName, dataSeriesName, properties);
    } else {
        series = new StringSeries(deviceName, dataSeriesName, properties);
    }

    dataSeries.insert(deviceName + "." + dataSeriesName, series);
}

void DataProvider::onNewData(qint64 timestamp, QString fullDataSeriesName, Value value)
{
    db.add(fullDataSeriesName, timestamp, value);

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
