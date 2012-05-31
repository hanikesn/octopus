#include "abstractdataseries.h"

#include "value.h"

AbstractDataSeries::AbstractDataSeries(const QString &deviceName, const QString &name, bool stateful):
    deviceName(deviceName),
    dataSeriesName(name),
    stateful(stateful)
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

bool AbstractDataSeries::isStateful() const
{
    return stateful;
}
