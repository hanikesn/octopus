#ifndef VALUE_H
#define VALUE_H

#include <QString>

namespace EI {
    class Value;
}

class Value
{
public:
    enum Type {DOUBLE, STRING};

    explicit Value(EI::Value const& value);
    explicit Value(const QString& value);
    explicit Value(double value);

    Type getType() const;

    QString asString() const;
    double asDouble() const;

private:
    Value();

    Type type;
    double d;
    QString s;
};

#endif // VALUE_H
