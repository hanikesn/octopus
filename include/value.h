#ifndef VALUE_H
#define VALUE_H

#include <QString>

class Value
{
public:
    enum Type {DOUBLE, STRING};

    explicit Value(const QString& value);
    explicit Value(double value);
    explicit Value(Value const& value);
    Value& operator=(const Value& other);
    ~Value() {}

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
