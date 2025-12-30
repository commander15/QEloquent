#ifndef TABLEBLUEPRINT_H
#define TABLEBLUEPRINT_H

#include <QEloquent/global.h>

#include <QSharedDataPointer>

namespace QEloquent {

class TableFieldBlueprint;

class TableBlueprintData;
class QELOQUENT_EXPORT TableBlueprint
{
public:
    TableBlueprint();
    TableBlueprint(const TableBlueprint &);
    TableBlueprint(TableBlueprint &&);
    TableBlueprint &operator=(const TableBlueprint &);
    TableBlueprint &operator=(TableBlueprint &&);
    ~TableBlueprint();

    TableFieldBlueprint id(const QString &name = "id");
    TableFieldBlueprint boolean(const QString &name);
    TableFieldBlueprint integer(const QString &name);
    TableFieldBlueprint decimal(const QString &name);
    TableFieldBlueprint string(const QString &name);
    TableFieldBlueprint timestamp(const QString &name);
    TableFieldBlueprint foreignId(const QString &name, const QString &tableName, const QString &primaryKey = "id");

    void timestamps(const QString &creation = "created_at", const QString &update = "updated_at");

private:
    QSharedDataPointer<TableBlueprintData> data;

    friend class QueryBuilder;
};

class TableFieldBlueprintData;
class QELOQUENT_EXPORT TableFieldBlueprint
{
public:
    TableFieldBlueprint &length(int len);

    TableFieldBlueprint &unique();
    TableFieldBlueprint &nullable();

    TableFieldBlueprint &defaultValue(const QVariant &value);
    TableFieldBlueprint &defaultExpression(const QString &expr);

    TableFieldBlueprint &min(const QVariant &value);
    TableFieldBlueprint &max(const QVariant &value);
    // ..

private:
    TableFieldBlueprint(const QExplicitlySharedDataPointer<TableFieldBlueprintData> &data);

    QExplicitlySharedDataPointer<TableFieldBlueprintData> data;

    friend class TableBlueprint;
};

} // namespace QEloquent

#endif // TABLEBLUEPRINT_H
