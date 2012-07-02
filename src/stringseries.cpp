#include "stringseries.h"

#include "dataprovider.h"

#include "value.h"

StringSeries::StringSeries(DataProvider &dp, const QString &deviceName, const QString &name, Data::Properties properties) :
    AbstractDataSeries(dp, deviceName, name, properties)
{
    defaultScaleType = PlotSettings::NOT_SCALABLE;
}

void StringSeries::accept(DataSeriesVisitor *v)
{
    v->addGraph(*this);
}

void StringSeries::addData(qint64 timestamp, const Value& value)
{
    if (value.getType() == Value::STRING) {
        emit newData(timestamp);
    } else {
        emit illegalValueType();
    }
}

QMap<qint64, QString> StringSeries::getData() const
{
    return dp.getDB().getData<QString>(fullName());
}

QMap<qint64, QString> StringSeries::getData(qint64 begin, qint64 end) const
{
    return dp.getDB().getData<QString>(fullName(), begin, end);
}
