#include "datamap.h"

#include <QEloquent/private/yamlserializer_p.h>

#include <QJsonObject>

namespace QEloquent {

DataMap::DataMap()
{}

DataMap::DataMap(const std::initializer_list<Pair> &data)
    : m_data(data)
    , m_index(generateIndex(m_data))
{}

QList<DataMap::Key> DataMap::keys() const
{
    QList<Key> keys;
    std::transform(m_data.begin(), m_data.end(), std::back_inserter(keys), [](const Pair &p) {
        return p.first;
    });
    return keys;
}

QList<DataMap::Value> DataMap::values() const
{
    QList<Value> values;
    std::transform(m_data.begin(), m_data.end(), std::back_inserter(values), [](const Pair &p) {
        return p.second;
    });
    return values;
}

bool DataMap::contains(const Key &key) const
{
    return m_index.contains(key);
}

DataMap::Value DataMap::value(const Key &key) const
{
    const int index = m_index.value(key);
    return m_data.value(index).second;
}

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

void DataMap::insert(const Key &key, const DataMap &map)
{
    insert(key, QVariant::fromValue(map));
}

void DataMap::insert(const Key &key, const QList<DataMap> &maps)
{
    insert(key, QVariant::fromValue(maps));
}

void DataMap::insert(const DataMap &map)
{
    for (const DataMapPair &pair : map)
        insert(pair.first, pair.second);
}

void DataMap::remove(const Key &key)
{
    // We avoid wasting CPU time if the key doesn't exists
    if (!m_index.contains(key)) return;

    // Removing index and data for the key
    const int removedIndex = m_index.take(key);
    m_data.remove(removedIndex);

    // Updating indexes after the removed one
    std::for_each(m_index.begin(), m_index.end(), [&removedIndex](int &index) {
        if (index > removedIndex) --index;
    });
}

void DataMap::clear()
{
    m_data.clear();
    m_index.clear();
}

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

void DataMap::forEach(const std::function<void (Pair &)> &callback, int depth)
{
    std::for_each(m_data.begin(), m_data.end(), callback);
}

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

void DataMap::computeIndexes()
{
    m_index = generateIndex(m_data);
}

DataMap DataMap::fromVariant(const QVariant &var)
{
    return var.value<DataMap>();
}

const QVector<DataMap::Pair> &DataMap::intenalData() const
{
    return m_data;
}

QVector<DataMap::Pair> &DataMap::intenalData()
{
    return m_data;
}

const QVector<DataMap::Pair> *DataMap::constList() const
{
    return &m_data;
}

QVector<DataMap::Pair> *DataMap::mutableList()
{
    return &m_data;
}

QHash<DataMap::Key, int> DataMap::generateIndex(const QVector<Pair> &data)
{
    QHash<DataMap::Key, int> index;
    for (int i(0); i < data.size(); ++i)
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
