#include "stringseries.h"

#include "value.h"

StringSeries::StringSeries(const QString &deviceName, const QString &name, DataProperty::Properties properties) :
    AbstractDataSeries(deviceName, name, properties)
{
}

void StringSeries::addData(qint64 timeStamp, const Value& value)
{
    if (value.getType() == Value::STRING) {
        values.insert(timeStamp, value.asString());
    } else {
        emit illegalValueType();
    }
}

QList<QString> StringSeries::getData(qint64 timeStamp) const
{
    return getData(timeStamp, timeStamp);
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
