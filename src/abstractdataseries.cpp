#include "abstractdataseries.h"

#include "dataprovider.h"
#include "value.h"

AbstractDataSeries::AbstractDataSeries(const DataProvider &dp, const QString &deviceName, const QString &name, Data::Properties properties):
    defaultScaleType(PlotSettings::NOT_SCALABLE),
    dp(dp),
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

Data::Properties AbstractDataSeries::properties() const
{
    return props;
}

qint64 AbstractDataSeries::offset() const
{
    return dp.getDB().getOffset(fullName());
}

void AbstractDataSeries::setOffset(qint64 newOffset)
{
    if (newOffset != offset()) {
        dp.changeOffset(fullName(), newOffset);
        emit offsetChanged();
    }
}
