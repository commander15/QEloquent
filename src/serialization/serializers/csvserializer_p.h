#ifndef QELOQUENT_CSVSERIALIZER_P_H
#define QELOQUENT_CSVSERIALIZER_P_H

#include <QEloquent/serialization.h>
#include <QEloquent/datamap.h>

namespace QEloquent::Private {

class CsvSerializer
{
public:
    static QByteArray serializeMaps(const QList<DataMap> &maps, bool isList, SerializationFormat format) {
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
};

}

#endif // QELOQUENT_CSVSERIALIZER_P_H
