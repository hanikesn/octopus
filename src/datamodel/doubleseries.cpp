#include "datamodel/doubleseries.h"

#include "common/value.h"
#include "datamodel/dataprovider.h"

DoubleSeries::DoubleSeries(DataProvider &dp, const QString &deviceName, const QString &name, Data::Properties properties) :
    AbstractDataSeries(dp, deviceName, name, properties)
{
    defaultScaleType = PlotSettings::LINSCALE;

    setObjectName("DoubleSeries");
}

void DoubleSeries::accept(DataSeriesVisitor *v)
{
    v->addGraph(*this);
}

void DoubleSeries::addData(qint64 timestamp, const Value& value)
{
    if (value.getType() == Value::DOUBLE) {
        emit newData(timestamp);
    }
}

QMap<qint64, double> DoubleSeries::getData() const
{
    return dp.getDB().getData<double>(fullName());
}

QMap<qint64, double> DoubleSeries::getData(qint64 begin, qint64 end) const
{
    return dp.getDB().getData<double>(fullName(), begin, end);
}
