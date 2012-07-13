#ifndef COMMON_H
#define COMMON_H

#include <QString>

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
