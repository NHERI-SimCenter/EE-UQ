#ifndef JSONSERIALIZABLE_H
#define JSONSERIALIZABLE_H
#include <QJsonObject>

class AbstractJsonSerializable
{
public:
    AbstractJsonSerializable(){};

    virtual ~AbstractJsonSerializable(){};

    virtual QJsonObject serialize() const = 0;

    virtual void deserialize(const QJsonObject& json) = 0;
};

#endif // JSONSERIALIZABLE_H
