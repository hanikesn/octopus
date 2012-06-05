#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

#include "boost/property_tree/ptree.hpp"
#include <QString>

namespace btTree = boost::property_tree;

class Serializable
{
public:
    virtual ~Serializable() {}

    virtual void save(boost::property_tree::ptree *pt) = 0;

    virtual void load(boost::property_tree::ptree *pt) = 0;
};

class QStringTranslator
{
public:
    typedef std::string  internal_type;
    typedef QString      external_type;

    boost::optional<external_type> get_value( internal_type const &v )
    {
        return QString::fromUtf8(v.c_str());
    }

    boost::optional<internal_type> put_value( external_type const& v )
    {
        return std::string(v.toUtf8().constData());
    }
};

namespace boost{ namespace property_tree{
    template<>
    struct translator_between<std::string, QString>
    {
        typedef QStringTranslator type;
    };
}}



#endif // SERIALIZABLE_H
