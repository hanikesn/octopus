#ifndef METADATA_H
#define METADATA_H

#include <QFlags>
#include <EIDescription.h>
#include <QMetaType>

namespace Data
{
    enum Property {
        INTERPOLATABLE  =1,
        STATEFUL        =2
    };
    Q_DECLARE_FLAGS(Properties, Property)
}

Q_DECLARE_OPERATORS_FOR_FLAGS(Data::Properties)

inline Data::Properties convert(EI::DataSeriesInfo::Properties prop)
{
    Data::Properties result;

    if(prop & EI::DataSeriesInfo::INTERPOLATABLE)
        result |= Data::INTERPOLATABLE;

    if(prop & EI::DataSeriesInfo::STATEFUL)
        result |= Data::STATEFUL;

    return result;
}

// EI::Description has no default constructor, but Qt needs this, therefore we have to use a wrapper
struct EIDescriptionWrapper
{
    EIDescriptionWrapper() : d(EI::Description("","")) {}
    EIDescriptionWrapper(EI::Description const& d) : d(d) {}

    EI::Description d;
};

#endif // METADATA_H
