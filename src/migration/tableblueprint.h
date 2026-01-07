#ifndef TABLEBLUEPRINT_H
#define TABLEBLUEPRINT_H

#include <QEloquent/global.h>
#include <QEloquent/metaobject.h>

#include <QSharedDataPointer>

namespace QEloquent {

class ColumnDefinition;
class ColumnDefinitionData;
class ForeignKeyDefinition;

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

    ColumnDefinition id(const QString &name = "id");
    ColumnDefinition uuid(const QString &name);

    ColumnDefinition boolean(const QString &name);

    ColumnDefinition integer(const QString &name, bool autoIncrement = false, bool positive = false);
    ColumnDefinition unsignedInteger(const QString &name, bool autoIncrement = false);
    ColumnDefinition bigInteger(const QString &name, bool autoIncrement = false, bool positive = false);
    ColumnDefinition unsignedBigInteger(const QString &name, bool autoIncrement = false);
    ColumnDefinition tinyInteger(const QString &name, bool autoIncrement = false, bool positive = false);
    ColumnDefinition unsignedTinyInteger(const QString &name, bool autoIncrement = false);

    ColumnDefinition doubleNumber(const QString &name);
    ColumnDefinition floatNumber(const QString &name, int precision = 53);
    ColumnDefinition decimal(const QString &name, int total = 8, int places = 2);

    ColumnDefinition character(const QString &name, int length = 255);
    ColumnDefinition string(const QString &name, int length = 255);

    ColumnDefinition date(const QString &name);
    ColumnDefinition time(const QString &name);
    ColumnDefinition datetime(const QString &name);
    ColumnDefinition timestamp(const QString &name);

    void rawColumn(const QString &name, const QString &definition);

    template<typename Model> ForeignKeyDefinition foreign(const QString &name = QString());
    ForeignKeyDefinition foreignId(const QString &name, const QString &tableName, const QString &primaryKey = "id");

    void timestamps(const QString &creation = "created_at", const QString &update = "updated_at");

    static TableBlueprint create(const QString &table, bool newTable = true);

private:
    using Column = ColumnDefinitionData;

    TableBlueprint(TableBlueprintData *data);

    ForeignKeyDefinition foreignKey(const MetaObject &meta, const QString &fieldName = QString());

    QSharedDataPointer<TableBlueprintData> data;

    friend class Schema;
    friend class QueryBuilder;
};

class ColumnDefinitionData;
class QELOQUENT_EXPORT ColumnDefinition
{
public:
    ColumnDefinition();
    virtual ~ColumnDefinition();

    ColumnDefinition &primaryKey(bool enable = true);
    ColumnDefinition &autoIncrement();
    ColumnDefinition &index();
    ColumnDefinition &index(bool enable);
    ColumnDefinition &index(const QString &name);
    ColumnDefinition &unique(bool enable = true);
    ColumnDefinition &nullable(bool enable = true);

    ColumnDefinition &defaultValue(const QVariant &value);
    ColumnDefinition &defaultExpression(const QString &expr);

    ColumnDefinition &min(const QVariant &value);
    ColumnDefinition &max(const QVariant &value);
    ColumnDefinition &range(const QVariant &min, const QVariant &max);

    ColumnDefinition &check(const QString &expr);

protected:
    ColumnDefinition(ColumnDefinitionData *data);
    ColumnDefinition(const QExplicitlySharedDataPointer<ColumnDefinitionData> &data);

    QExplicitlySharedDataPointer<ColumnDefinitionData> data;

    friend class TableBlueprint;
};

class ForeignKeyDefinition : public ColumnDefinition
{
public:
    enum ForeignKeyAction {
        NoAction,
        Restrict,
        Cascade,
        SetNull,
    };

    ForeignKeyDefinition();
    ~ForeignKeyDefinition();

    ForeignKeyDefinition &onUpdate(ForeignKeyAction action);
    ForeignKeyDefinition &onDelete(ForeignKeyAction action);

private:
    ForeignKeyDefinition(const QExplicitlySharedDataPointer<ColumnDefinitionData> &data);

    friend class TableBlueprint;
};

template<typename Model>
inline ForeignKeyDefinition TableBlueprint::foreign(const QString &name) {
    return foreignKey(MetaObject::from<Model>(), name);
}

} // namespace QEloquent

#endif // TABLEBLUEPRINT_H
