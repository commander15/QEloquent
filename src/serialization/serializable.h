#ifndef QELOQUENT_SERIALIZABLE_H
#define QELOQUENT_SERIALIZABLE_H

#include <QEloquent/global.h>
#include <QEloquent/serialization.h>

class QJsonObject;
class QJsonArray;
class QJsonValue;
class QDataStream;

namespace QEloquent {

class DataMap;

class QELOQUENT_EXPORT Serializable
{
public:
    virtual ~Serializable() = default;

    virtual QString serializationContext() const;
    virtual bool isListSerializable() const = 0;
    virtual QList<DataMap> serialize() const = 0;

    QJsonObject toJsonObject() const;
    QJsonArray toJsonArray() const;
    QJsonValue toJsonValue() const;
    QByteArray toJson(SerializationFormat format = SerializationFormat::Compact) const;
    static QJsonObject jsonObjectFromMap(const DataMap &map);

    QByteArray toCsv(SerializationFormat format = SerializationFormat::Compact) const;
};

} // namespace QEloquent

QELOQUENT_EXPORT QDebug operator<<(QDebug debug, const QEloquent::Serializable &serializable);
QELOQUENT_EXPORT QDataStream &operator<<(QDataStream &out, const QEloquent::Serializable &serializable);

#endif // QELOQUENT_SERIALIZABLE_H
