#ifndef QELOQUENT_JSONSERIALIZER_P_H
#define QELOQUENT_JSONSERIALIZER_P_H

#include <QEloquent/serialization.h>
#include <QEloquent/datamap.h>

#include <QJsonObject>
#include <QJsonArray>

namespace QEloquent::Private {

class JsonSerializer
{
public:
    static QJsonObject serializeMap(const DataMap &map) {
        return QJsonObject::fromVariantMap(map.toVariantMap());
    }

    static QJsonArray serializeMaps(const QList<DataMap> &maps) {
        QJsonArray array;
        for (const DataMap &map : maps)
            array.append(QJsonObject::fromVariantMap(map.toVariantMap()));
        return array;
    }

    static DataMap deserializeMap(const QJsonObject &object) {
        return DataMap::fromVariantMap(object.toVariantMap());
    }

    static QList<DataMap> deserializeMaps(const QJsonArray &array) {
        QList<DataMap> maps;
        std::transform(array.begin(), array.end(), std::back_inserter(maps), [](const QJsonValue &value) {
            return DataMap::fromVariantMap(value.toObject().toVariantMap());
        });
        return maps;
    }
};

}

#endif // QELOQUENT_JSONSERIALIZER_P_H
