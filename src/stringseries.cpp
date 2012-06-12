#include "stringseries.h"

#include "value.h"

StringSeries::StringSeries(const DatabaseAdapter &db, const QString &deviceName, const QString &name, Data::Properties properties) :
    AbstractDataSeries(db, deviceName, name, properties)
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
    return db.getData<QString>(fullName());
}

QMap<qint64, QString> StringSeries::getData(qint64 begin, qint64 end) const
{
    QMap<qint64, QString> selection;

    QMap<qint64, QString>::const_iterator i = values.lowerBound(begin);
    while (i != values.upperBound(end)) {
        selection.insert(i.key(), i.value());
        i++;
    }

    return selection;
}
