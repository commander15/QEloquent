#ifndef TABLEBLUEPRINT_H
#define TABLEBLUEPRINT_H

#include <QEloquent/global.h>
#include <QEloquent/metaobject.h>

#include <QSharedDataPointer>
#include <QVariant>

namespace QEloquent {

class TableData;
class ColumnDefinition;
class ColumnData;
class ForeignKeyDefinition;
class Driver;

class TableBlueprintPrivate;
class QELOQUENT_EXPORT TableBlueprint
{
public:
    typedef std::function<void(const TableData &table)> TableExplorer;

    TableBlueprint();
    TableBlueprint(const TableBlueprint &);
    TableBlueprint(TableBlueprint &&);
    TableBlueprint &operator=(const TableBlueprint &);
    TableBlueprint &operator=(TableBlueprint &&);
    ~TableBlueprint();

    ColumnDefinition id(const QString &name = "id");
    ColumnDefinition uuid(const QString &name);

    ColumnDefinition boolean(const QString &name);

    ColumnDefinition integer(const QString &name, bool positive = false, bool autoIncrement = false);
    ColumnDefinition unsignedInteger(const QString &name, bool autoIncrement = false);
    ColumnDefinition bigInteger(const QString &name, bool positive = false, bool autoIncrement = false);
    ColumnDefinition unsignedBigInteger(const QString &name, bool autoIncrement = false);
    ColumnDefinition tinyInteger(const QString &name, bool positive = false, bool autoIncrement = false);
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
    void timestamps(const QString &creation = "created_at", const QString &update = "updated_at");

    ForeignKeyDefinition foreignId(const QString &name);
    template<typename Model> ForeignKeyDefinition foreignIdFor();
    template<typename Model> ForeignKeyDefinition foreignIdFor(const QString &name);
    ForeignKeyDefinition foreign(const QString &name);

    void rawColumn(const QString &name, const QString &definition);

    TableData table() const;

    static TableBlueprint create(const QString &table, bool create, const QString &connectionName);

private:
    using Column = ColumnData;

    TableBlueprint(TableBlueprintPrivate *data);

    ColumnDefinition idTyped(const QString &name);

    QExplicitlySharedDataPointer<TableBlueprintPrivate> data;

    friend class Schema;
    friend class SchemaGrammar;
};

class ColumnDefinitionPrivate;
class QELOQUENT_EXPORT ColumnDefinition
{
public:
    ColumnDefinition();
    ColumnDefinition(const ColumnDefinition &);
    ColumnDefinition(ColumnDefinition &&);
    ColumnDefinition &operator=(const ColumnDefinition &);
    ColumnDefinition &operator=(ColumnDefinition &&);
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
    ColumnDefinition &in(const QVariantList &values);

    ColumnDefinition &check(const QString &expr);

protected:
    ColumnDefinition(ColumnDefinitionPrivate *data);
    ColumnDefinition(const QExplicitlySharedDataPointer<ColumnDefinitionPrivate> &data);

    QExplicitlySharedDataPointer<ColumnDefinitionPrivate> data;

    friend class TableBlueprint;
};

class QELOQUENT_EXPORT ForeignKeyDefinition : public ColumnDefinition
{
public:
    enum ForeignKeyAction {
        NoAction,
        Restrict,
        Cascade,
        SetNull,
        SetDefault,
    };

    ForeignKeyDefinition();
    ForeignKeyDefinition(const ForeignKeyDefinition &);
    ForeignKeyDefinition(ForeignKeyDefinition &&);
    ForeignKeyDefinition &operator=(const ForeignKeyDefinition &);
    ForeignKeyDefinition &operator=(ForeignKeyDefinition &&);
    ~ForeignKeyDefinition();

    ForeignKeyDefinition &on(const QString &table);
    ForeignKeyDefinition &references(const QString &column);
    ForeignKeyDefinition &references(const QStringList &columns);

    ForeignKeyDefinition &onUpdate(ForeignKeyAction action);
    ForeignKeyDefinition &onDelete(ForeignKeyAction action);

private:
    ForeignKeyDefinition(const QExplicitlySharedDataPointer<ColumnDefinitionPrivate> &data);

    friend class TableBlueprint;
};

template<typename Model>
inline ForeignKeyDefinition TableBlueprint::foreignIdFor() {
    return foreignIdFor<Model>(QString());
}

template<typename Model>
inline ForeignKeyDefinition TableBlueprint::foreignIdFor(const QString &name) {
    const MetaObject meta = MetaObject::from<Model>();
    return foreignId(name.isEmpty() ? meta.foreignProperty().fieldName() : name)
        .on(meta.tableName())
        .references(meta.primaryProperty().fieldName());
}

} // namespace QEloquent

#endif // TABLEBLUEPRINT_H
