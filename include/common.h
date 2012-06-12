#ifndef COMMON_H
#define COMMON_H

#include <QFlags>
#include <QString>

namespace Data
{
    enum Property {
        INTERPOLATABLE  =1,
        STATEFUL        =2
    };
    Q_DECLARE_FLAGS(Properties, Property)
}

Q_DECLARE_OPERATORS_FOR_FLAGS(Data::Properties)

inline QString fromStdString(std::string const& str)
{
    return QString::fromUtf8(str.c_str(), str.length());
}

inline std::string toStdString(QString const& str)
{
    auto const& data = str.toUtf8();
    return std::string(data.constData(), data.length());
}

#endif // COMMON_H
