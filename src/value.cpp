#include "value.h"

Value::Value(const QString &value):
    type(STRING),
    s(value)
{
}

Value::Value(double value):
    type(DOUBLE),
    d(value)
{
}

Value::Value(const Value& value):
    type(value.getType())
{
    if (value.getType() == STRING) {
        s = value.asString();
    } else if (value.getType() == DOUBLE) {
        d = value.asDouble();
    }
}

Value& Value::operator=(const Value& other)
{
    type = other.type;
    s = other.s;
    d = other.d;

    return *this;
}

Value::Type Value::getType() const
{
    return type;
}

QString Value::asString() const
{
    if (type == STRING) {
        return s;
    } else {
        // TODO(Steffi)
    }
}

double Value::asDouble() const
{
    if (type == DOUBLE) {
        return d;
    } else {
        // TODO(Steffi)
    }
}
