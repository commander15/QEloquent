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
        // ToDo: implement deserialization here
        return DataMap();
    }

    static QList<DataMap> deserializeMaps(const QJsonArray &array) {
        // ToDo: implement deserialization here
        return {};
    }
};

}

#endif // QELOQUENT_JSONSERIALIZER_P_H
