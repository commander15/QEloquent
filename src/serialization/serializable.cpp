#include "serializable.h"

#include <QEloquent/datamap.h>
#include <QEloquent/private/jsonserializer_p.h>
#include <QEloquent/private/yamlserializer_p.h>
#include <QEloquent/private/csvserializer_p.h>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonDocument>

namespace QEloquent {

using namespace Private;

/*!
 * \class Serializable
 * \brief Interface for objects that can be serialized to various formats.
 *
 * The Serializable interface provides a unified way to convert an object's state
 * into common serialized representations such as JSON, CSV, YAML, and binary streams.
 *
 * Implementations typically override the protected \c serialize() method to expose
 * their data as a list of DataMap objects (key-value property maps), while the public
 * methods convert this data into the desired output format.
 *
 * \note This interface is designed to be used with models or other data objects that
 *       need to be serialized for storage, transmission, debugging, or API responses.
 *
 * \sa Deserializable for the opposite direction (deserialization).
 */

/*!
 * \brief Returns the serialization context for this object.
 *
 * This optional context string can be used to provide additional information
 * during serialization (e.g., for logging, debugging, or format-specific headers).
 * The default implementation returns an empty string.
 *
 * Subclasses may override this method to return a meaningful context.
 *
 * \return A QString providing context about the serialized data.
 */
QString Serializable::serializationContext() const
{
    return QString();
}

/*!
 * \fn Serializable::isListSerializable
 * \brief Indicates whether this object should be serialized as a list/array.
 *
 * Returns \c true if the object represents a collection of items (e.g., multiple rows,
 * records, or entities), which should be serialized as an array in formats like JSON.
 *
 * Returns \c false if the object represents a single item/entity, which should be
 * serialized as a single object/map.
 *
 * The return value affects the output of methods like \c toJsonValue(), \c toJson(),
 * and \c toYaml(): \c true produces an array, \c false produces a single object.
 *
 * The default implementation returns \c false.
 *
 * Subclasses must override this method to reflect their data structure.
 *
 * \return \c true if list/array serialization is appropriate, \c false otherwise.
 */

/*!
 * \fn Serializable::serialize
 * \brief Serializes the object's state into a list of data maps.
 *
 * This pure virtual method must be implemented by subclasses to expose their
 * internal state as a list of \c DataMap objects (key-value property maps).
 *
 * Each \c DataMap in the returned list represents one row, record, or entity.
 * Keys are property/field names, and values are the corresponding data.
 *
 * - For single-entity objects: return a list with one map.
 * - For collections/lists: return a list with one map per item.
 *
 * The returned data is used by all public serialization methods (\c toJson(),
 * \c toYaml(), \c toCsv(), etc.) and by the streaming operators.
 *
 * \return A list of \c DataMap objects representing the object's serialized state.
 *         An empty list indicates no data to serialize.
 */

/*!
 * \brief Serializes the object to a JSON object.
 *
 * Converts the serialized data maps to a single QJsonObject.
 * If the object is list-serializable, only the first map is used.
 * Returns an empty object if no data is available.
 *
 * \return A QJsonObject representing the serialized state.
 */
QJsonObject Serializable::toJsonObject() const
{
    const QList<DataMap> maps = serialize();
    if (maps.isEmpty()) return QJsonObject();
    else return JsonSerializer::serializeMap(maps.first());
}

/*!
 * \brief Serializes the object to a JSON array.
 *
 * Converts all serialized data maps to a QJsonArray.
 * Returns an empty array if no data is available.
 *
 * \return A QJsonArray containing the serialized data.
 */
QJsonArray Serializable::toJsonArray() const
{
    const QList<DataMap> maps = serialize();
    if (maps.isEmpty()) return QJsonArray();
    else return JsonSerializer::serializeMaps(maps);
}

/*!
 * \brief Serializes the object to a JSON value (object or array).
 *
 * Automatically chooses between QJsonObject and QJsonArray based on
 * whether the object is list-serializable.
 * Returns an empty value if no data is available.
 *
 * \return A QJsonValue containing either an object or array representation.
 */
QJsonValue Serializable::toJsonValue() const
{
    const QList<DataMap> maps = serialize();
    if (maps.isEmpty()) return QJsonValue();
    if (isListSerializable())
        return JsonSerializer::serializeMaps(maps);
    else
        return JsonSerializer::serializeMap(maps.first());
}

/*!
 * \brief Serializes the object to JSON byte array.
 *
 * Converts the serialized data to JSON format with the specified style.
 * Supports both compact and pretty-printed output.
 *
 * \param format The desired JSON formatting style (Compact or Pretty/Indented).
 * \return A QByteArray containing the JSON representation.
 */
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

/*!
 * \brief Serializes the object to CSV byte array.
 *
 * Converts the serialized data maps to CSV format using the specified settings.
 *
 * \param format The CSV formatting style (e.g., Compact or Pretty).
 * \return A QByteArray containing the CSV representation.
 */
QByteArray Serializable::toCsv(SerializationFormat format) const
{
    const QList<DataMap> maps = serialize();
    return CsvSerializer::serializeMaps(maps, isListSerializable(), format);
}

/*!
 * \brief Serializes the object to YAML byte array.
 *
 * Converts the serialized data to YAML format.
 * Returns an empty array if no data is available.
 *
 * \return A QByteArray containing the YAML representation.
 */
QByteArray Serializable::toYaml() const
{
    const QList<DataMap> maps = serialize();
    if (maps.isEmpty()) return QByteArray();
    if (isListSerializable())
        return YamlSerializer::serializeMaps(maps);
    else
        return YamlSerializer::serializeMap(maps.first());
}

/*!
 * \brief Streaming output operator for debugging.
 *
 * Writes a human-readable YAML representation of the serialized object
 * to the debug stream, optionally prefixed with a context string.
 *
 * \param debug The QDebug stream to write to.
 * \param serializable The Serializable object to debug.
 * \return The debug stream (for chaining).
 */
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

/*!
 * \brief Streaming output operator for serialization.
 *
 * Writes the serialized data maps to the QDataStream.
 * This enables binary serialization of Serializable objects.
 *
 * \param out The output QDataStream.
 * \param serializable The Serializable object to serialize.
 * \return The output stream (for chaining).
 *
 * \sa operator>>(QDataStream&, Deserializable&) for deserialization.
 */
QDataStream &operator<<(QDataStream &out, const QEloquent::Serializable &serializable)
{
    const QList<QEloquent::DataMap> maps = serializable.serialize();
    return out << maps;
}

}

/*!
 * \brief Streaming output operator for debugging.
 *
 * Writes a human-readable YAML representation of the serialized object
 * to the debug stream, optionally prefixed with a context string.
 *
 * \param debug The QDebug stream to write to.
 * \param serializable The Serializable object to debug.
 * \return The debug stream (for chaining).
 */
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

/*!
 * \brief Streaming output operator for serialization.
 *
 * Writes the serialized data maps to the QDataStream.
 * This enables binary serialization of Serializable objects.
 *
 * \param out The output QDataStream.
 * \param serializable The Serializable object to serialize.
 * \return The output stream (for chaining).
 *
 * \sa operator>>(QDataStream&, Deserializable&) for deserialization.
 */
QDataStream &operator<<(QDataStream &out, const QEloquent::Serializable &serializable)
{
    const QList<QEloquent::DataMap> maps = serializable.serialize();
    return out << maps;
}
