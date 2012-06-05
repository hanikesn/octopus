#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

#include "boost/property_tree/ptree.hpp"


class Serializable
{
public:
    virtual ~Serializable() {}

    virtual void save(boost::property_tree::ptree *pt) = 0;

    virtual void load(boost::property_tree::ptree *pt) = 0;
};

#endif // SERIALIZABLE_H
