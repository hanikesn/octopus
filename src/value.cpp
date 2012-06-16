#include "value.h"
#include <EIValue.h>

Value::Value()
    : type(EMPTY)
{

}

Value::Value(EI::Value const& value)
{
    auto t = value.getType();
    if(t == EI::Value::DOUBLE) {
        type = DOUBLE;
        d = value.asDouble();
    } else if(t == EI::Value::STRING) {
        type = STRING;
        s = QString::fromUtf8(value.asString().c_str());
    }
}

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
        return "";
    }
}

double Value::asDouble() const
{
    if (type == DOUBLE) {
        return d;
    } else {
        // TODO(Steffi)
        return 0.0;
    }
}
