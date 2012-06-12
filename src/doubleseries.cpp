#include "doubleseries.h"

#include "value.h"

#include <QDebug>

DoubleSeries::DoubleSeries(const DatabaseAdapter &db, const QString &deviceName, const QString &name, Data::Properties properties) :
    AbstractDataSeries(db, deviceName, name, properties)
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
    return db.getData<double>(fullName());
}

QMap<qint64, double> DoubleSeries::getData(qint64 begin, qint64 end) const
{
    QMap<qint64, double> selection;

    QMap<qint64, double>::const_iterator i = values.lowerBound(begin);
    while (i != values.upperBound(end)) {
        selection.insert(i.key(), i.value());
        i++;
    }

    return selection;
}
