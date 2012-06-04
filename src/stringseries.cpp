#include "stringseries.h"

#include "value.h"

StringSeries::StringSeries(const QString &deviceName, const QString &name, Data::Properties properties) :
    AbstractDataSeries(deviceName, name, properties)
{
}

void StringSeries::accept(DataSeriesVisitor *v)
{
    v->addGraph(*this);
}

void StringSeries::addData(qint64 timestamp, const Value& value)
{
    if (value.getType() == Value::STRING) {
        values.insert(timestamp, value.asString());
        emit newData(timestamp);
    } else {
        emit illegalValueType();
    }
}

QList<QString> StringSeries::getData(qint64 timestamp) const
{
    return getData(timestamp, timestamp);
}

QList<QString> StringSeries::getData(qint64 begin, qint64 end) const
{
    QList<QString> data;

    QMap<qint64, QString>::const_iterator i = values.constBegin();
    while (i != values.constEnd()) {
        if (begin <= i.key() && i.key() <= end) {
            data.append(i.value());
        }
    }

    return data;
}
