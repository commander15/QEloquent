#include "serializable.h"

#include <QEloquent/datamap.h>
#include <QEloquent/private/jsonserializer_p.h>
#include <QEloquent/private/yamlserializer_p.h>
#include <QEloquent/private/csvserializer_p.h>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

namespace QEloquent {

using namespace Private;

QString Serializable::serializationContext() const
{
    return QString();
}

QJsonObject Serializable::toJsonObject() const
{
    const QList<DataMap> maps = serialize();
    if (maps.isEmpty()) return QJsonObject();
    else return JsonSerializer::serializeMap(maps.first());
}

QJsonArray Serializable::toJsonArray() const
{
    const QList<DataMap> maps = serialize();
    if (maps.isEmpty()) return QJsonArray();
    else return JsonSerializer::serializeMaps(maps);
}

QJsonValue Serializable::toJsonValue() const
{
    const QList<DataMap> maps = serialize();
    if (maps.isEmpty()) return QJsonValue();

    if (isListSerializable())
        return JsonSerializer::serializeMaps(maps);
    else
        return JsonSerializer::serializeMap(maps.first());
}

QByteArray Serializable::toJson(SerializationFormat format) const
{
    const QList<DataMap> maps = serialize();

    QJsonDocument doc;
    if (isListSerializable())
        doc.setArray(JsonSerializer::serializeMaps(maps));
    else if (!maps.isEmpty())
        doc.setObject(JsonSerializer::serializeMap(maps.first()));
    else
        doc.setObject(QJsonObject());

    switch (format) {
    case SerializationFormat::Compact:
        return doc.toJson(QJsonDocument::Compact);

    case SerializationFormat::Pretty:
    case SerializationFormat::Beautified:
        return doc.toJson(QJsonDocument::Indented);
    }

    return QByteArray();
}

QByteArray Serializable::toCsv(SerializationFormat format) const
{
    const QList<DataMap> maps = serialize();
    return CsvSerializer::serializeMaps(maps, isListSerializable(), format);
}

QByteArray Serializable::toYaml() const
{
    const QList<DataMap> maps = serialize();
    if (maps.isEmpty()) return QByteArray();

    if (isListSerializable())
        return YamlSerializer::serializeMaps(maps);
    else
        return YamlSerializer::serializeMap(maps.first());
}

} // namespace QEloquent

QDebug operator<<(QDebug debug, const QEloquent::Serializable &serializable)
{
    // Serializer used for debugging (YAML)
    using Serializer = QEloquent::Private::YamlSerializer;

    const QString context = serializable.serializationContext();
    const QList<QEloquent::DataMap> maps = serializable.serialize();

    debug.noquote().nospace()
        << (context.isEmpty() ? context : context + '\n');

    if (serializable.isListSerializable())
        debug << Serializer::serializeMaps(maps);
    else
        debug << Serializer::serializeMap(maps.isEmpty() ? QEloquent::DataMap() : maps.first());

    return debug.resetFormat();
}

QDataStream &operator<<(QDataStream &out, const QEloquent::Serializable &serializable)
{
    const QList<QEloquent::DataMap> maps = serializable.serialize();
    return out << maps;
}
