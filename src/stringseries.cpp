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

QString StringSeries::getData(qint64 timestamp) const
{
    // TODO(Steffi): Check! Was ist der Rückgabewert, wenn es den Wert nicht gibt? Sollte !=0 sein!
    return values.value(timestamp);
}

QMap<qint64, QString> StringSeries::getData() const
{
    return values;
}

QMap<qint64, QString> StringSeries::getData(qint64 begin, qint64 end) const
{
    QMap<qint64, QString> selection;

    QMap<qint64, QString>::const_iterator i = values.constBegin();
    while (i != values.constEnd()) {
        if (begin <= i.key() && i.key() <= end) {
            selection.insert(i.key(), i.value());
        }
    }

    return selection;
}
