#ifndef QELOQUENT_DESERIALIZABLE_H
#define QELOQUENT_DESERIALIZABLE_H

#include <QEloquent/global.h>
#include <QEloquent/serialization.h>

class QJsonObject;
class QJsonArray;
class QJsonValue;
class QSqlRecord;
class QDataStream;

namespace QEloquent {

class DataMap;

class QELOQUENT_EXPORT Deserializable
{
public:
    enum DataType {
        Json,
        Csv
    };

    virtual ~Deserializable() = default;

    void fill(const DataMap &values);
    void fill(const QList<DataMap> &data);

    void fill(const QJsonObject &object);
    void fill(const QJsonArray &array);
    void fill(const QJsonValue &value);

    void fill(const QSqlRecord &record);
    void fill(const QList<QSqlRecord> &records);

    void fill(const QByteArray &data, DataType type, SerializationFormat format);

    virtual void deserialize(const QList<DataMap> &maps, bool all) = 0;

private:
    void fillJson(const QByteArray &data);
    void fillCsv(const QByteArray &data, SerializationFormat format, char separator = ';');
};

} // namespace QEloquent

QELOQUENT_EXPORT QDataStream &operator>>(QDataStream &in, QEloquent::Deserializable &deserializable);

#endif // QELOQUENT_DESERIALIZABLE_H
