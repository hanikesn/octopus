#ifndef COMMON_H
#define COMMON_H

#include <QFlags>

namespace Data
{
    enum Property {
        INTERPOLATABLE  =1,
        STATEFUL        =2
    };
    Q_DECLARE_FLAGS(Properties, Property)
}

Q_DECLARE_OPERATORS_FOR_FLAGS(Data::Properties)

#endif // COMMON_H
