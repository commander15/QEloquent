#include "datamap.h"

#include <QEloquent/private/yamlserializer_p.h>

#include <QJsonObject>

namespace QEloquent {

/*!
 * \class DataMap
 * \brief Lightweight, ordered key-value container used for serialization and deserialization.
 *
 * DataMap is a hybrid map that combines the benefits of a list (preserves insertion order)
 * and a hash map (fast key lookup). It is the core data structure used by QEloquent
 * for model serialization, deserialization, and data transfer between layers.
 *
 * - Keys are always QString
 * - Values are QVariant (can hold primitives, nested DataMap, QList<DataMap>, etc.)
 * - Insertion order is preserved (important for serialization formats like JSON/YAML/CSV)
 * - Fast lookup via internal index (QHash<QString, int>)
 *
 * DataMap is copy-on-write friendly and efficient for small-to-medium datasets.
 * It is **not** thread-safe for mutation (use with care in multi-threaded code).
 *
 * \sa Serializable and Deserializable for using DataMap in model serialization.
 * \sa QVariantMap for the standard Qt key-value container.
 */

/*!
 * \brief Default constructor.
 *
 * Creates an empty data map.
 */
DataMap::DataMap()
{}

/*!
 * \brief Constructs a DataMap from an initializer list of key-value pairs.
 *
 * \param data List of pairs (QString key, QVariant value).
 */
DataMap::DataMap(const std::initializer_list<Pair> &data)
    : m_data(data)
    , m_index(generateIndex(m_data))
{}

/*!
 * \brief Returns a list of all keys in insertion order.
 *
 * \return Ordered list of keys.
 */
QList<DataMap::Key> DataMap::keys() const
{
    QList<Key> keys;
    std::transform(m_data.begin(), m_data.end(), std::back_inserter(keys), [](const Pair &p) {
        return p.first;
    });
    return keys;
}

/*!
 * \brief Returns a list of all values in insertion order.
 *
 * \return Ordered list of values.
 */
QList<DataMap::Value> DataMap::values() const
{
    QList<Value> values;
    std::transform(m_data.begin(), m_data.end(), std::back_inserter(values), [](const Pair &p) {
        return p.second;
    });
    return values;
}

/*!
 * \brief Checks whether the map contains a specific key.
 *
 * \param key The key to look for.
 * \return \c true if the key exists, \c false otherwise.
 */
bool DataMap::contains(const Key &key) const
{
    return m_index.contains(key);
}

/*!
 * \brief Returns the value associated with a key.
 *
 * \param key The key to look up.
 * \return The stored value, or invalid QVariant if key not found.
 */
DataMap::Value DataMap::value(const Key &key) const
{
    const int index = m_index.value(key);
    return m_data.value(index).second;
}

/*!
 * \brief Inserts or updates a key-value pair.
 *
 * If the key already exists, updates the value in place.
 * If not, appends a new pair at the end (preserves order).
 *
 * \param key The key to insert/update.
 * \param value The value to associate with the key.
 */
void DataMap::insert(const Key &key, const Value &value)
{
    if (m_index.contains(key)) {
        m_data[m_index.value(key)].second = value;
    } else {
        const int index = m_data.size();
        m_data.append({ key, value });
        m_index.insert(key, index);
    }
}

/*!
 * \brief Inserts a nested DataMap under the given key.
 *
 * Convenience overload that wraps the map in QVariant.
 *
 * \param key The key to insert/update.
 * \param map Nested DataMap to store.
 */
void DataMap::insert(const Key &key, const DataMap &map)
{
    insert(key, QVariant::fromValue(map));
}

/*!
 * \brief Inserts a list of nested DataMaps under the given key.
 *
 * Convenience overload that wraps the list in QVariant.
 *
 * \param key The key to insert/update.
 * \param maps List of nested DataMaps to store.
 */
void DataMap::insert(const Key &key, const QList<DataMap> &maps)
{
    insert(key, QVariant::fromValue(maps));
}

/*!
 * \brief Merges another DataMap into this one.
 *
 * Inserts all key-value pairs from \a map.
 * Existing keys are updated; new keys are appended.
 *
 * \param map The DataMap to merge from.
 */
void DataMap::insert(const DataMap &map)
{
    for (const DataMapPair &pair : map)
        insert(pair.first, pair.second);
}

/*!
 * \brief Removes a key-value pair by key.
 *
 * If the key exists, removes it and updates internal indexes.
 * If not found, does nothing (no-op).
 *
 * \param key The key to remove.
 */
void DataMap::remove(const Key &key)
{
    // We avoid wasting CPU time if the key doesn't exist
    if (!m_index.contains(key)) return;

    // Removing index and data for the key
    const int removedIndex = m_index.take(key);
    m_data.remove(removedIndex);

    // Updating indexes after the removed one
    std::for_each(m_index.begin(), m_index.end(), [&removedIndex](int &index) {
        if (index > removedIndex) --index;
    });
}

/*!
 * \brief Clears all key-value pairs.
 *
 * Removes all data and resets internal indexes.
 */
void DataMap::clear()
{
    m_data.clear();
    m_index.clear();
}

/*!
 * \brief Recursively applies a callback to all key-value pairs.
 *
 * Traverses the map and any nested DataMap or QList<DataMap> values.
 *
 * \param callback Function called for each leaf pair (key, value, parent map).
 * \param depth Maximum recursion depth (prevents infinite nesting).
 */
void DataMap::forEach(const std::function<void (const Pair &, const DataMap &)> &callback, int depth) const
{
    if (depth <= 0)
        return;
    for (const Pair &item : m_data) {
        const char *typeName = item.second.typeName();
        if (strcmp("QEloquent::DataMap", typeName) == 0) {
            const DataMap subMap = item.second.value<DataMap>();
            subMap.forEach([&callback, &subMap](const Pair &subItem, const DataMap &) { callback(subItem, subMap); }, depth - 1);
            continue;
        }
        if (strcmp("QList<QEloquent::DataMap>", typeName) == 0) {
            const QList<DataMap> subMaps = item.second.value<QList<DataMap>>();
            for (const DataMap &subMap : subMaps)
                subMap.forEach([&callback, &subMap](const Pair &subItem, const DataMap &) { callback(subItem, subMap); }, depth - 1);
            continue;
        }
        callback(item, DataMap());
    }
}

/*!
 * \brief Applies a callback to each top-level pair (non-recursive).
 *
 * Calls the callback for every key-value pair at the root level only.
 *
 * \param callback Function to call for each pair.
 * \param depth Unused in this overload (kept for compatibility).
 */
void DataMap::forEach(const std::function<void (Pair &)> &callback, int depth)
{
    std::for_each(m_data.begin(), m_data.end(), callback);
}

/*!
 * \brief Removes all key-value pairs that match a predicate.
 *
 * \param pred Predicate that returns true for pairs to remove.
 */
void DataMap::removeIf(const std::function<bool (const Pair &)> &pred)
{
    m_data.removeIf(pred);
}

/*!
 * \brief Converts the DataMap to a QVariantMap (recursive).
 *
 * Nested DataMap and QList<DataMap> are recursively converted.
 *
 * \return QVariantMap suitable for QJsonObject or other Qt serialization.
 */
QVariantMap DataMap::toVariantMap() const
{
    QVariantMap map;
    for (const Pair &item : m_data) {
        const char *typeName = item.second.typeName();
        if (strcmp("QEloquent::DataMap", typeName) == 0) {
            const DataMap subMap = item.second.value<DataMap>();
            map.insert(item.first, subMap.toVariantMap());
            continue;
        }
        if (strcmp("QList<QEloquent::DataMap>", typeName) == 0) {
            const QList<DataMap> subMaps = item.second.value<QList<DataMap>>();
            QList<QVariant> maps;
            std::transform(subMaps.begin(), subMaps.end(), std::back_inserter(maps), [](const DataMap &map) {
                return QVariant(map.toVariantMap());
            });
            map.insert(item.first, maps);
            continue;
        }
        map.insert(item.first, item.second);
    }
    return map;
}

/*!
 * \brief Constructs a DataMap from a QVariantMap.
 *
 * \param map QVariantMap to convert from.
 * \return New DataMap with the same key-value pairs.
 */
DataMap DataMap::fromVariantMap(const QVariantMap &map)
{
    DataMap output;
    const QStringList fields = map.keys();
    for (const QString &field : fields)
        output.insert(field, map.value(field));
    return output;
}

/*!
 * \brief Rebuilds the internal key-to-index hash.
 *
 * Called internally after structural changes (e.g. remove, insert).
 */
void DataMap::computeIndexes()
{
    m_index = generateIndex(m_data);
}

/*!
 * \brief Extracts a nested DataMap from a QVariant.
 *
 * Convenience method for safe extraction.
 *
 * \param var QVariant containing a DataMap.
 * \return The extracted DataMap (empty if conversion fails).
 */
DataMap DataMap::fromVariant(const QVariant &var)
{
    return var.value<DataMap>();
}

/*!
 * \brief Returns a const reference to the internal vector of pairs.
 *
 * For internal use or advanced inspection.
 *
 * \return Const reference to the ordered list of key-value pairs.
 */
const QVector<DataMap::Pair> &DataMap::intenalData() const
{
    return m_data;
}

/*!
 * \brief Returns a mutable reference to the internal vector of pairs.
 *
 * For internal use only — modifying this directly bypasses index maintenance.
 *
 * \return Mutable reference to the ordered list of key-value pairs.
 */
QVector<DataMap::Pair> &DataMap::intenalData()
{
    return m_data;
}

/*!
 * \brief Returns a const pointer to the internal vector.
 *
 * \return Const pointer to the ordered list of pairs.
 */
const QVector<DataMap::Pair> *DataMap::constList() const
{
    return &m_data;
}

/*!
 * \brief Returns a mutable pointer to the internal vector.
 *
 * \return Mutable pointer to the ordered list of pairs.
 */
QVector<DataMap::Pair> *DataMap::mutableList()
{
    return &m_data;
}

/*!
 * \brief Generates the key-to-index lookup hash from a vector of pairs.
 *
 * \param data The list of pairs to index.
 * \return QHash mapping each key to its index in the vector.
 */
QHash<DataMap::Key, int> DataMap::generateIndex(const QVector<Pair> &data)
{
    QHash<DataMap::Key, int> index;
    for (int i = 0; i < data.size(); ++i)
        index.insert(data.at(i).first, i);
    return index;
}

} // namespace QEloquent

QDebug operator<<(QDebug debug, const QEloquent::DataMap &map)
{
    // Serializer used for ddebuging (YAML)
    using Serializer = QEloquent::Private::YamlSerializer;

    const QByteArray output = Serializer::serializeMap(map);
    debug.noquote().nospace() << output;
    return debug.resetFormat();
}

QDataStream &operator<<(QDataStream &out, const QEloquent::DataMap &map)
{
    return out << map.intenalData();
}

QDataStream &operator>>(QDataStream &in, QEloquent::DataMap &map)
{
    in >> map.intenalData();
    map.computeIndexes();
    return in;
}
