#include "doubleseries.h"

#include "value.h"

#include <QDebug>

DoubleSeries::DoubleSeries(const DataProvider &dp, const QString &deviceName, const QString &name, Data::Properties properties) :
    AbstractDataSeries(dp, deviceName, name, properties)
{
}

void DoubleSeries::accept(DataSeriesVisitor *v)
{
    v->addGraph(*this);
}

void DoubleSeries::addData(qint64 timestamp, const Value& value)
{
    if (value.getType() == Value::DOUBLE) {
        emit newData(timestamp);
    } else {
        emit illegalValueType();
    }
}

double DoubleSeries::getData(qint64 timestamp) const
{
    // TODO(Steffi): Check! Was ist der Rückgabewert, wenn es den Wert nicht gibt? Sollte !=0 sein!
    return getData(timestamp, timestamp).value(timestamp);
}

QMap<qint64, double> DoubleSeries::getData() const
{
    return dp.getDB().getData<double>(fullName());
}

QMap<qint64, double> DoubleSeries::getData(qint64 begin, qint64 end) const
{
    return dp.getDB().getData<double>(fullName(), begin, end);
}
