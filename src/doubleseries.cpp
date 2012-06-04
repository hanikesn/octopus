#include "doubleseries.h"

#include "value.h"

DoubleSeries::DoubleSeries(const QString &deviceName, const QString &name, Data::Properties properties) :
    AbstractDataSeries(deviceName, name, properties)
{
}

void DoubleSeries::accept(DataSeriesVisitor *v)
{
    v->addGraph(*this);
}

void DoubleSeries::addData(qint64 timeStamp, const Value& value)
{
    if (value.getType() == Value::DOUBLE) {
        values.insert(timeStamp, value.asDouble());
    } else {
        emit illegalValueType();
    }
}

QList<double> DoubleSeries::getData(qint64 timeStamp) const
{
    return getData(timeStamp, timeStamp);
}

QList<double> DoubleSeries::getData(qint64 begin, qint64 end) const
{
    QList<double> data;

    QMap<qint64, double>::const_iterator i = values.constBegin();
    while (i != values.constEnd()) {
        if (begin <= i.key() && i.key() <= end) {
            data.append(i.value());
        }
    }

    return data;
}
