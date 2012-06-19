#ifndef VALUE_H
#define VALUE_H

#include <QString>

namespace EI {
    class Value;
}

class Value
{
public:
    enum Type {DOUBLE, STRING, EMPTY};

    Value();
    explicit Value(EI::Value const& value);
    explicit Value(const QString& value);
    explicit Value(double value);

    Type getType() const;

    /**
     * @return The value as a string if the object is of type STRING or DOUBLE.
     *      An empty string otherwise.
     *      If the object is of type DOUBLE, the value will be converted to a
     *      string with the maximum possible precision.
     */
    QString asString() const;

    /**
     * @return The value as a double if the object is of type STRING or DOUBLE.
     *      0.0 otherwise or if the conversion of string to double doesn't work.
     */
    double asDouble() const;

private:

    Type type;
    double d;
    QString s;
};

#endif // VALUE_H
