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
        emit newData(timestamp);
    } else {
        emit illegalValueType();
    }
}

QString StringSeries::getData(qint64 timestamp) const
{
    // TODO(Steffi): Check! Was ist der Rückgabewert, wenn es den Wert nicht gibt? Sollte !=0 sein!
    return getData(timestamp, timestamp).value(timestamp);
}

QMap<qint64, QString> StringSeries::getData() const
{
    return db.getData<QString>(fullName());
}

QMap<qint64, QString> StringSeries::getData(qint64 begin, qint64 end) const
{
    return db.getData<QString>(fullName(), begin, end);
}
