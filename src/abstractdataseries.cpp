#include "abstractdataseries.h"

#include "value.h"

AbstractDataSeries::AbstractDataSeries(const DatabaseAdapter &db, const QString &deviceName, const QString &name, Data::Properties properties):
    deviceName(deviceName),
    dataSeriesName(name),
    props(properties),
    db(db)
{
}

QString AbstractDataSeries::device() const
{
    return deviceName;
}

QString AbstractDataSeries::name() const
{
    return dataSeriesName;
}

QString AbstractDataSeries::fullName() const
{
    return deviceName + "." + dataSeriesName;
}

Data::Properties AbstractDataSeries::properties() const
{
    return props;
}
