#include "doubleseries.h"

#include "value.h"

#include <QDebug>

DoubleSeries::DoubleSeries(const QString &deviceName, const QString &name, Data::Properties properties) :
    AbstractDataSeries(deviceName, name, properties)
{
}

void DoubleSeries::accept(DataSeriesVisitor *v)
{
    v->addGraph(*this);
}

void DoubleSeries::addData(qint64 timestamp, const Value& value)
{
    if (value.getType() == Value::DOUBLE) {
        values.insert(timestamp, value.asDouble());
        emit newData(timestamp);
    } else {
        emit illegalValueType();
    }
}

double DoubleSeries::getData(qint64 timestamp) const
{
    // TODO(Steffi): Check! Was ist der Rückgabewert, wenn es den Wert nicht gibt? Sollte !=0 sein!
    return values.value(timestamp);
}

QMap<qint64, double> DoubleSeries::getData() const
{
    return values;
}

QMap<qint64, double> DoubleSeries::getData(qint64 begin, qint64 end) const
{
    QMap<qint64, double> selection;

    QMap<qint64, double>::const_iterator i = values.constBegin();
    while (i != values.constEnd()) {
        if (begin <= i.key() && i.key() <= end) {
            selection.insert(i.key(), i.value());
        }
    }

    return selection;
}
