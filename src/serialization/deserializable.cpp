#include "deserializable.h"

#include <QEloquent/datamap.h>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QSqlRecord>

namespace QEloquent {

void Deserializable::fill(const DataMap &values)
{
    deserialize({ values }, false);
}

void Deserializable::fill(const QList<DataMap> &data)
{
    deserialize(data, false);
}

void Deserializable::fill(const QJsonObject &object)
{
    const QJsonArray array = { object };
    fill(array);
}

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

void Deserializable::fill(const QJsonValue &value)
{
    if (value.isObject())
        fill(value.toObject());
    else
        fill(value.toArray());
}

void Deserializable::fill(const QSqlRecord &record)
{
    fill(QList<QSqlRecord>() << record);
}

void Deserializable::fill(const QList<QSqlRecord> &records)
{
    QList<DataMap> maps;

    for (const QSqlRecord &record : records) {
        DataMap map;
        for (int i(0); i < record.count(); ++i)
            map.insert(record.fieldName(i), record.value(i));
        maps.append(map);
    }

    deserialize(maps, true);
}

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
            if (i >= headers.size())
                headers.append("Column " + QString::number(i + 1));

            map.insert(headers.at(i), line.at(i));
        }
        return map;
    };

    QList<DataMap> maps;
    for (int i(startLine); i < lines.size(); ++i)
        maps.append(extractLine(i));
    deserialize(maps, false);
}

} // namespace QEloquent

QDataStream &operator>>(QDataStream &in, QEloquent::Deserializable &deserializable)
{
    QList<QEloquent::DataMap> maps;
    in >> maps;
    deserializable.deserialize(maps, true);
    return in;
}
