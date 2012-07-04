#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

#include <QVariant>

class Serializable
{
public:
    virtual ~Serializable() {}

    /**
      * Saves the classes settings to the specified map.
      * @param qvm The map to save the settings in.
      */
    virtual void save(QVariantMap *qvm) = 0;

    /**
      * Loads the classes settings from the specified map.
      * @param qvm The map to load the settings from.
      */
    virtual void load(QVariantMap *qvm) = 0;
};
#endif // SERIALIZABLE_H
