#include "abstractdataseries.h"

#include "value.h"

AbstractDataSeries::AbstractDataSeries(const QString &deviceName, const QString &name, DataProperty::Properties properties):
    deviceName(deviceName),
    dataSeriesName(name),
    props(properties)
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

DataProperty::Properties AbstractDataSeries::properties() const
{
    return props;
}
