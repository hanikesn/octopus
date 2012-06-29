#include "abstractdataseries.h"

#include "value.h"

AbstractDataSeries::AbstractDataSeries(const DataProvider &dp, const QString &deviceName, const QString &name, Data::Properties properties):
    offset(0),
    defaultScaleType(PlotSettings::NOT_SCALABLE),
    deviceName(deviceName),
    dataSeriesName(name),
    props(properties),
    dp(dp)
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
