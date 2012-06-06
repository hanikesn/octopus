#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

class Serializable
{
public:
    virtual ~Serializable() {}

    virtual void save(QVariantMap *qvm) = 0;

    virtual void load(QVariantMap *qvm) = 0;
};
#endif // SERIALIZABLE_H
