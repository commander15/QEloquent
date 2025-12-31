#ifndef QELOQUENT_YAMLSERIALIZER_P_H
#define QELOQUENT_YAMLSERIALIZER_P_H

#include <QEloquent/serialization.h>
#include <QEloquent/datamap.h>

#include <QRegularExpression>

namespace QEloquent::Private {

class YamlSerializer
{
public:
    static QByteArray serializeMap(const DataMap &map) {

        using Pair = QEloquent::DataMap::Pair;
        using DataMap = QEloquent::DataMap;

        QString output = printYaml(map, 0);
        if (output.isEmpty()) {
            output = "{}";
        }

        return output.toUtf8();
    }

    static QByteArray serializeMaps(const QList<DataMap> &maps) {
        QByteArrayList output;

        std::transform(maps.begin(), maps.end(), std::back_inserter(output), [](const DataMap &map) {
            QByteArrayList yaml = serializeMap(map).split('\n');
            for (int i(0); i < yaml.size(); ++i) {
                QByteArray &line = yaml[i];
                if (i == 0) line.prepend("- ");
                else line.prepend("  ");
            }
            return yaml.join('\n');
        });

        return output.join("\n\n");
    }

    static DataMap deserializeMap(const QByteArray &data) {
        // ToDo: implement deserialization here
        return DataMap();
    }

    static QList<DataMap> deserializeMaps(const QByteArray &data) {
        // ToDo: implement deserialization here
        return {};
    }

private:
    static QString printYaml(const DataMap& m, int depth) {

        static const QRegularExpression alphaNumExpr("[^a-zA-Z0-9_-]");

        if (m.isEmpty()) {
            return "{}";
        }

        QString indent(depth * 2, ' ');
        QStringList lines;

        for (const DataMap::Pair& pair : m) {
            const QVariant& value = pair.second;
            QString key = pair.first;

            // Quote key if it contains special characters
            bool keyNeedsQuote = key.contains(alphaNumExpr) ||
                                 key.isEmpty() ||
                                 key[0].isDigit();

            QString keyLine = indent + (keyNeedsQuote ? ("\"" + key + "\"") : key) + ":";

            if (value.canConvert<DataMap>()) {
                DataMap nested = value.value<DataMap>();
                QString nestedStr = printYaml(nested, depth + 1);
                if (nestedStr == "{}") {
                    lines << keyLine + " {}";
                } else {
                    lines << keyLine;
                    lines << nestedStr;
                }
            }
            else if (value.canConvert<QList<DataMap>>()) {
                const QList<DataMap> list = value.value<QList<DataMap>>();
                if (list.isEmpty()) {
                    lines << keyLine + " []";
                } else {
                    lines << keyLine;
                    for (const DataMap& item : list) {
                        QString itemStr = printYaml(item, depth + 1);
                        if (itemStr == "{}") {
                            lines << indent + "  - {}";
                        } else {
                            QStringList itemLines = itemStr.split('\n');
                            // First line gets "-"
                            lines << indent + "  -" + itemLines.takeFirst().removeFirst();
                            // Subsequent lines get extra indentation
                            for (const QString& line : std::as_const(itemLines)) {
                                if (!line.isEmpty()) {
                                    lines << indent + "  " + line;
                                }
                            }
                        }
                    }
                }
            }
            else {
                QString strVal = value.toString();

                // Quote if contains special YAML chars or multiline
                bool needsQuote = strVal.contains('\n') ||
                                  strVal.contains(':') ||
                                  strVal.contains('#') ||
                                  strVal.startsWith('{') ||
                                  strVal.startsWith('[') ||
                                  strVal.contains('"');

                if (needsQuote) {
                    strVal = strVal.replace("\"", "\\\"");
                    strVal = "\"" + strVal + "\"";
                }

                lines << keyLine + " " + strVal;
            }
        }

        return lines.join("\n");
    }
};

}

#endif // QELOQUENT_YAMLSERIALIZER_P_H
