#include "dataprovider.h"

#include "common.h"
#include "doubleseries.h"
#include "stringseries.h"
#include "value.h"

#include <QDebug>
#include <QDir>
#include <cmath>

DataProvider::DataProvider(QString const& filename) :
    db(new DatabaseAdapter(filename)), filename(filename), currentMax(0)
{
}

QList<QString> DataProvider::getDataSeriesList() const
{
    return dataSeries.keys();
}

QString DataProvider::getDBFileName()
{
    return filename;
}

void DataProvider::moveDB(QString newFilename)
{
    // delete the dbadapter so that the db connection is closed
    db.reset();

    if(QDir().rename(filename, newFilename)) {
        QDir().remove(filename + "-wal");
        QDir().remove(filename + "-shm");
        filename = newFilename;
    }

    db = std::unique_ptr<DatabaseAdapter>(new DatabaseAdapter(filename));
}

AbstractDataSeries* DataProvider::getDataSeries(const QString &fullName) const
{
    return dataSeries.value(fullName);
}

void DataProvider::onNewSender(EIDescriptionWrapper desc)
{
    const QString deviceName = fromStdString(desc.d.getSender());
    db->addSender(deviceName, fromStdString(desc.d.getDeviceType()));

    auto const& series = desc.d.getDataSeries();

    std::for_each(series.begin(), series.end(),
            [this, &deviceName](EI::DataSeriesInfoMap::value_type p)
    {
      AbstractDataSeries* series;
      EI::DataSeriesInfo::Properties props = p.second.getProperties();
      if (props & EI::DataSeriesInfo::INTERPOLATABLE) {
          series = new DoubleSeries(*db, deviceName, fromStdString(p.first), convert(props));
      } else {
          series = new StringSeries(*db, deviceName, fromStdString(p.first), convert(props));
      }

      dataSeries.insert(deviceName + "." + fromStdString(p.first), series);
    });
}

void DataProvider::onNewData(qint64 timestamp, QString fullDataSeriesName, Value value)
{
    db->add(fullDataSeriesName, timestamp, value);

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
