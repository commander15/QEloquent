#include "serializable.h"

#include <QEloquent/datamap.h>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

namespace QEloquent {

QString Serializable::serializationContext() const
{
    return QString();
}

QJsonObject Serializable::toJsonObject() const
{
    const QList<DataMap> maps = serialize();
    if (maps.isEmpty()) return QJsonObject();

    const DataMap &map = maps.first();
    return QJsonObject::fromVariantMap(map.toVariantMap());
}

QJsonArray Serializable::toJsonArray() const
{
    const QList<DataMap> maps = serialize();
    if (maps.isEmpty()) return QJsonArray();

    QJsonArray array;
    for (const DataMap &map : maps)
        array.append(QJsonObject::fromVariantMap(map.toVariantMap()));
    return array;
}

QJsonValue Serializable::toJsonValue() const
{
    if (isListSerializable())
        return toJsonArray();
    else
        return toJsonObject();
}

QByteArray Serializable::toJson(SerializationFormat format) const
{
    QJsonDocument doc;
    if (isListSerializable())
        doc.setArray(toJsonArray());
    else
        doc.setObject(toJsonObject());

    switch (format) {
    case SerializationFormat::Compact:
        return doc.toJson(QJsonDocument::Compact);

    case SerializationFormat::Pretty:
    case SerializationFormat::Beautified:
        return doc.toJson(QJsonDocument::Indented);
    }

    return QByteArray();
}

QJsonObject Serializable::jsonObjectFromMap(const DataMap &map)
{
    QJsonObject object;
    for (const DataMap::Pair &data : map)
        object.insert(data.first, QJsonValue::fromVariant(data.second));
    return object;
}

QByteArray Serializable::toCsv(SerializationFormat format) const
{
    const QList<DataMap> maps = serialize();
    if (maps.isEmpty()) return QByteArray();

    const QStringList keys = maps.first().keys();
    QByteArrayList data;
    const QByteArray separator(";");

    // Header added for pretty format
    if (format == SerializationFormat::Pretty) {
        QByteArrayList line;
        std::transform(keys.begin(), keys.end(), std::back_inserter(line), [](const QString &key) {
            return key.toUtf8();
        });

        data.append(line.join(separator));
    }

    // Data
    for (const DataMap &map : maps) {
        QByteArrayList line;
        for (const QString &key : keys)
            line.append(map.value(key).toByteArray());
        data.append(line.join(separator));
    }

    return data.join('\n');
}

} // namespace QEloquent

QDebug operator<<(QDebug debug, const QEloquent::Serializable &serializable)
{
    const QString context = serializable.serializationContext();

    debug.noquote().nospace()
        << (context.isEmpty() ? QString() : context + ' ')
        << serializable.serialize();

    return debug.resetFormat();
}

QDataStream &operator<<(QDataStream &out, const QEloquent::Serializable &serializable)
{
    const QList<QEloquent::DataMap> maps = serializable.serialize();
    return out << maps;
}
