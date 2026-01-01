#ifndef QELOQUENT_DATAMAP_H
#define QELOQUENT_DATAMAP_H

#include <QEloquent/global.h>
#include <QEloquent/listproxy.h>

#include <QPair>
#include <QVector>
#include <QHash>
#include <QVariant>

namespace QEloquent {

using DataMapKey = QString;
using DataMapValue = QVariant;
using DataMapPair = QPair<DataMapKey, DataMapValue>;

class QELOQUENT_EXPORT DataMap : public AbstractListProxy<DataMapPair>
{
public:
    using Key = DataMapKey;
    using Value = DataMapValue;
    using Pair = QPair<Key, Value>;

    DataMap();
    DataMap(const std::initializer_list<Pair> &data);
    DataMap(DataMap &&other) = default;
    DataMap(const DataMap &other) = default;
    DataMap& operator=(const DataMap &) = default;
    DataMap& operator=(DataMap &&) = default;

    QList<Key> keys() const;
    QList<Value> values() const;

    bool contains(const Key &key) const;
    Value value(const Key &key) const;
    void insert(const Key &key, const Value &value);
    void insert(const Key &key, const DataMap &map);
    void insert(const Key &key, const QList<DataMap> &maps);
    void insert(const DataMap &map);
    void remove(const Key &key);
    void clear();

    void forEach(const std::function<void(const Pair &, const DataMap &)> &callback, int depth = 1) const;
    void forEach(const std::function<void(Pair &)> &callback, int depth = 1);

    void removeIf(const std::function<bool(const Pair &)> &pred);

    QVariantMap toVariantMap() const;

    void computeIndexes();

    const QVector<Pair> &intenalData() const;
    QVector<Pair> &intenalData();

    static DataMap fromVariant(const QVariant &var);

protected:
    const QVector<Pair> *constList() const override;
    QVector<Pair> *mutableList() override;

private:
    static QHash<Key, int> generateIndex(const QVector<Pair> &data);

    QVector<Pair> m_data;
    QHash<Key, int> m_index;
};

} // namespace QEloquent

Q_DECLARE_METATYPE(QEloquent::DataMap)

QELOQUENT_EXPORT QDebug operator<<(QDebug debug, const QEloquent::DataMap &map);

QELOQUENT_EXPORT QDataStream &operator<<(QDataStream &out, const QEloquent::DataMap &map);
QELOQUENT_EXPORT QDataStream &operator>>(QDataStream &in, QEloquent::DataMap &map);

#endif // QELOQUENT_DATAMAP_H
