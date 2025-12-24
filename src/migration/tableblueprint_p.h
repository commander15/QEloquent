#ifndef QELOQUENT_TABLEBLUEPRINT_P_H
#define QELOQUENT_TABLEBLUEPRINT_P_H

#include "tableblueprint.h"

#include <QEloquent/driver.h>

#include <QList>
#include <QVariant>

namespace QEloquent {

class TableBlueprintData : public QSharedData
{
public:
    using FieldType = Driver::FieldType;

    QExplicitlySharedDataPointer<TableFieldBlueprintData> &fieldData(const QString &name, FieldType type);

    QList<QExplicitlySharedDataPointer<TableFieldBlueprintData>> fields;
};

class TableFieldBlueprintData : public QSharedData
{
public:
    using Type = Driver::FieldType;

    QString name;
    Type type;
    QVariant defaultValue;

    QString refTable;
    QString refColumn;

    bool primaryKey = false;
    bool unique = false;
    bool nullable = false;

    int min = -1;
    int max = -1;
    int length = -1;
};

}

#endif // QELOQUENT_TABLEBLUEPRINT_P_H
