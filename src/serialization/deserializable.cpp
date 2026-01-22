#include "deserializable.h"

#include <QEloquent/datamap.h>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QSqlRecord>

namespace QEloquent {

/*!
 * \class Deserializable
 * \brief Interface for objects that can be populated from various data sources.
 *
 * The Deserializable interface provides a unified way to deserialize data into an object
 * from multiple formats, including key-value maps, JSON, SQL records, and raw serialized data.
 *
 * Implementations typically override the protected \c deserialize() method to handle
 * the actual population logic, while the public \c fill() overloads provide convenient
 * entry points for different input types.
 *
 * \note This interface is designed to be used with models or other data objects that
 *       need to be hydrated from external sources (database queries, API responses,
 *       serialized data, etc.).
 *
 * \sa Serializable for the opposite direction (serialization).
 */

/*!
 * \brief Fills the deserializable object from a single data map.
 *
 * Deserializes the provided key-value pairs into the object's properties/fields.
 * This is a convenience overload that wraps the map in a single-item list.
 *
 * \param values A map of property/field names to values.
 */
void Deserializable::fill(const DataMap &values)
{
    deserialize({ values }, false);
}

/*!
 * \brief Fills the deserializable object from a list of data maps.
 *
 * Deserializes multiple rows/maps into the object or a collection it manages.
 * Typically used for batch loading or filling from query results.
 *
 * \param data List of data maps (each map represents one row/object).
 */
void Deserializable::fill(const QList<DataMap> &data)
{
    deserialize(data, false);
}

/*!
 * \brief Fills the deserializable object from a JSON object.
 *
 * Converts the JSON object to a DataMap and deserializes it.
 * Treats the object as a single row/instance.
 *
 * \param object A QJsonObject containing property/field names and values.
 */
void Deserializable::fill(const QJsonObject &object)
{
    fill({ object });
}

/*!
 * \brief Fills the deserializable object from a JSON array.
 *
 * Converts each JSON object in the array to a DataMap and deserializes them.
 * Suitable for filling from JSON arrays representing multiple rows/instances.
 *
 * \param array A QJsonArray of objects.
 */
void Deserializable::fill(const QJsonArray &array)
{
    QList<DataMap> maps;
    for (const QJsonValue &value : array) {
        const QJsonObject object = value.toObject();
        const QStringList keys = object.keys();
        DataMap map;
        for (const QString &key : keys)
            map.insert(key, object.value(key).toVariant());
        maps.append(map);
    }
    deserialize(maps, false);
}

/*!
 * \brief Fills the deserializable object from a JSON value (object or array).
 *
 * Automatically detects whether the value is a JSON object or array
 * and delegates to the appropriate overload.
 *
 * \param value A QJsonValue containing either an object or array.
 */
void Deserializable::fill(const QJsonValue &value)
{
    if (value.isObject())
        fill(value.toObject());
    else
        fill(value.toArray());
}

/*!
 * \brief Fills the deserializable object from a single SQL record.
 *
 * Converts the QSqlRecord to a DataMap and deserializes it.
 * Convenience overload for loading a single database row.
 *
 * \param record A QSqlRecord containing column names and values.
 */
void Deserializable::fill(const QSqlRecord &record)
{
    fill(QList<QSqlRecord>() << record);
}

/*!
 * \brief Fills the deserializable object from a list of SQL records.
 *
 * Converts each QSqlRecord to a DataMap and deserializes them.
 * Commonly used for batch-loading query results.
 *
 * \param records List of QSqlRecord objects (each representing one row).
 */
void Deserializable::fill(const QList<QSqlRecord> &records)
{
    QList<DataMap> maps;
    for (const QSqlRecord &record : records) {
        DataMap map;
        for (int i = 0; i < record.count(); ++i)
            map.insert(record.fieldName(i), record.value(i));
        maps.append(map);
    }
    deserialize(maps, true);
}

/*!
 * \brief Fills the deserializable object from raw serialized data.
 *
 * Deserializes the data according to the specified type and format,
 * then populates the object's properties/fields.
 *
 * \param data Raw byte array containing serialized content.
 * \param type The data format/type (Json or Csv).
 * \param format Optional format settings (e.g. CSV delimiter, header).
 */
void Deserializable::fill(const QByteArray &data, DataType type, SerializationFormat format)
{
    switch (type) {
    case Json:
        fillJson(data);
        break;
    case Csv:
        fillCsv(data, format);
        break;
    }
}

/*!
 * \fn Deserializable::deserialize
 * \brief Fill maps into this object.
 * \param maps The maps to be filled.
 * \param all Must fill all or only allowed, this depends on the super class.
 * \note all = false for Model means 'only fill fillable properties/fields'
 */

void Deserializable::fillJson(const QByteArray &data)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);

    if (error.error != QJsonParseError::NoError) {
        // ...
        return;
    }

    if (doc.isObject())
        fill(doc.object());
    else if (doc.isArray())
        fill(doc.array());
}

void Deserializable::fillCsv(const QByteArray &data, SerializationFormat format, char separator)
{
    QStringList headers;
    const int startLine = (format == SerializationFormat::Compact ? 0 : 1);
    const QByteArrayList lines = data.trimmed().split('\n');
    if (lines.empty()) return;

    if (startLine == 1) {
        // We consider line 0 as header
        const QByteArrayList line = lines.first().split(separator);
        std::transform(line.begin(), line.end(), std::back_inserter(headers), [](const QByteArray &item) {
            return QString::fromUtf8(item.trimmed());
        });
    }

    auto extractLine = [&lines, &separator, &headers](int index) -> DataMap {
        const QByteArray &raw = lines.at(index);
        const QByteArrayList line = raw.split(separator);

        DataMap map;
        for (int i(0); i < line.size(); ++i) {
            if (i < headers.size())
                map.insert(headers.at(i), line.at(i));
            else
                headers.append("Column " + QString::number(i + 1));
        }
        return map;
    };

    QList<DataMap> maps;
    for (int i(startLine); i < lines.size(); ++i)
        maps.append(extractLine(i));
    deserialize(maps, false);
}

} // namespace QEloquent

/*!
 * \brief Stream extraction operator for deserializing into a Deserializable object.
 *
 * Reads a serialized QList<DataMap> from the QDataStream and uses it to populate
 * the target Deserializable object via its \c deserialize() method.
 *
 * The \c fromDatabase parameter is set to \c true, indicating that the data
 * originates from a database source (which may influence timestamp handling,
 * default values, or other database-specific behavior in the implementation).
 *
 * Typical usage:
 * \code
 * QDataStream stream(&byteArray, QIODevice::ReadOnly);
 * MyDeserializableObject obj;
 * stream >> obj;
 * \endcode
 *
 * \param in The input QDataStream containing serialized DataMap list.
 * \param deserializable The object to deserialize into.
 * \return Reference to \a in (enables chaining, e.g. stream >> obj1 >> obj2).
 *
 * \note
 * - The stream must be in a readable state before calling this operator.
 * - If the stream status becomes non-Ok (e.g. ReadPastEnd), the deserialization
 *   will be partial and the object's state may be inconsistent.
 * - Implementations of \c deserialize() should be idempotent and safe for partial data.
 *
 * \sa operator<<(QDataStream&, const Deserializable&) for serialization.
 */
QDataStream &operator>>(QDataStream &in, QEloquent::Deserializable &deserializable)
{
    QList<QEloquent::DataMap> maps;
    in >> maps;
    deserializable.deserialize(maps, true);
    return in;
}
