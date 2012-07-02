#include "dataprovider.h"

#include "common.h"
#include "doubleseries.h"
#include "stringseries.h"
#include "value.h"

#include <QDir>
#include <cmath>

DataProvider::DataProvider(QString const& file, QObject *parent) :
    QObject(parent),
    currentMax(0),
    temporaryDB(file.isNull())
{
    setObjectName("DataProvider");

    if(temporaryDB) {
        filename = QDir::tempPath() + "/Octopus-" + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmsszzz");
    } else {
        filename = file;
    }

    db = std::unique_ptr<DatabaseAdapter>(new DatabaseAdapter(filename));

    foreach(EI::Description const& d, db->getSenders())
    {
        foreach(EI::DataSeriesInfoMap::value_type const& info, d.getDataSeries())
        {
            addSeries(fromStdString(d.getSender()), fromStdString(info.first), info.second);
        }
    }

    qint64 min, max;
    db->getMinMaxTimeStamp(min, max);
}

DataProvider::~DataProvider()
{
    foreach(AbstractDataSeries* d, dataSeries)
    {
        d->deleteLater();
    }

    if(temporaryDB) {
        db.reset();
        QFile::remove(filename);
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
        seriesList << tmp;
    }
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

        getDataSeries(name)->defaultScaleType = (PlotSettings::ScaleType) defScaleType;
        getDataSeries(name)->setOffset(getDB().getOffset(name));
    }
    qint64 min;
    qint64 max;
    getDB().getMinMaxTimeStamp(min, max);
    emit newMax(max);

}

QList<QString> DataProvider::getDataSeriesList() const
{
    return dataSeries.keys();
}

QString DataProvider::getDBFileName()
{
    return filename;
}

DatabaseAdapter &DataProvider::getDB()
{
    return *db;
}

const DatabaseAdapter &DataProvider::getDB() const
{
    return *db;
}

void DataProvider::copyDB(QString filename, qint64 begin, qint64 end)
{
    db->copy(filename, begin, end);
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

    QString id = device_name + "." + name;

    // Don't replace already existing dataseries
    if(dataSeries.contains(id))
        return;

    EI::DataSeriesInfo::Properties props = info.getProperties();
    if (props & EI::DataSeriesInfo::INTERPOLATABLE) {
        series = new DoubleSeries(*this, device_name, name, convert(props));
    } else {
        series = new StringSeries(*this, device_name, name, convert(props));
    }

    dataSeries.insert(id, series);
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
