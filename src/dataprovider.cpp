#include "dataprovider.h"

#include "common.h"
#include "doubleseries.h"
#include "stringseries.h"
#include "value.h"

#include <QDir>
#include <cmath>

DataProvider::DataProvider(QString const& filename, QObject *parent) :
    QObject(parent), db(new DatabaseAdapter(filename)), filename(filename), currentMax(0)
{
    QList<EI::Description> list = db->getSenders();

    foreach(EI::Description const& d, list)
    {
        foreach(EI::DataSeriesInfoMap::value_type const& info, d.getDataSeries())
        {
            addSeries(fromStdString(d.getSender()), fromStdString(info.first), info.second);
        }
    }
}

DataProvider::~DataProvider()
{
    foreach(AbstractDataSeries* d, dataSeries)
    {
        d->deleteLater();
    }
}

void DataProvider::closeDB()
{
    db.reset();
}

void DataProvider::save(QVariantMap *qvm)
{
    QVariantMap dataProvider;
    QVariantList seriesList;
    foreach (AbstractDataSeries *ads, dataSeries) {
        QVariantMap tmp;
        tmp.insert("name", ads->fullName());
        tmp.insert("scaling", ads->defaultScaleType);
        tmp.insert("offset", ads->offset);
        seriesList << tmp;
    }
    dataProvider.insert("dbfile", getDBFileName());
    dataProvider.insert("dataSeries", seriesList);
    qvm->insert("dataProvider", dataProvider);
}

void DataProvider::load(QVariantMap *qvm)
{
    QVariantMap dataProvider = qvm->find("dataProvider").value().toMap();
    QVariantList seriesList = dataProvider.find("dataSeries").value().toList();
    foreach (QVariant series, seriesList) {
        QVariantMap seriesMap = series.toMap();

        int defScaleType = seriesMap.find("scaling").value().toInt();
        QString name(seriesMap.find("name").value().toString());
        int offset = seriesMap.find("offset").value().toInt();

        getDataSeries(name)->defaultScaleType = (PlotSettings::ScaleType) defScaleType;
        getDataSeries(name)->offset = offset;
    }

}

QList<QString> DataProvider::getDataSeriesList() const
{
    return dataSeries.keys();
}

QString DataProvider::getDBFileName()
{
    return filename;
}

const DatabaseAdapter &DataProvider::getDB() const
{
    return *db;
}

void DataProvider::moveDB(QString const& newFilename)
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

QList<AbstractDataSeries*> DataProvider::getDataSeries() const
{
    return dataSeries.values();
}

void DataProvider::onNewSender(EIDescriptionWrapper desc)
{
    if(!db)
        return;
    const QString deviceName = fromStdString(desc.d.getSender());
    db->addSender(desc.d);

    auto const& series = desc.d.getDataSeries();

    std::for_each(series.begin(), series.end(),
            [this, &deviceName](EI::DataSeriesInfoMap::value_type p)
    {
        addSeries(deviceName, fromStdString(p.first), p.second);
    });
}

void DataProvider::addSeries(QString const& device_name, QString const& name, EI::DataSeriesInfo const& info)
{
    AbstractDataSeries* series;
    EI::DataSeriesInfo::Properties props = info.getProperties();
    if (props & EI::DataSeriesInfo::INTERPOLATABLE) {
        series = new DoubleSeries(*this, device_name, name, convert(props));
    } else {
        series = new StringSeries(*this, device_name, name, convert(props));
    }

    dataSeries.insert(device_name + "." + name, series);
}

void DataProvider::onNewData(qint64 timestamp, QString fullDataSeriesName, Value value)
{
    if(!db)
        return;
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
