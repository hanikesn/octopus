#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H


class Serializable
{
public:
    virtual ~Serializable() {}

    virtual void save() = 0;

    virtual void load() = 0;
};

#endif // SERIALIZABLE_H
