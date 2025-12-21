#ifndef QELOQUENT_RELATION_IMPL_H
#define QELOQUENT_RELATION_IMPL_H

#include <QEloquent/relation.h>
#include <QEloquent/namingconvention.h>
#include <QEloquent/query.h>

namespace QEloquent {

template<typename RelatedModel>
class HasRelationData : public RelationBaseData<RelatedModel>
{
public:
    virtual ~HasRelationData() = default;

    void init(NamingConvention *) override
    {
        if (foreignKey.isEmpty())
            foreignKey = this->primaryObject.foreignProperty().fieldName();

        if (localKey.isEmpty())
            localKey = this->primaryObject.primaryProperty().fieldName();
    }

    bool get() override
    {
        Query q;
        q.where(foreignKey, this->parentField(localKey));
        if (!this->multiple()) q.limit(1);

        auto result = RelatedModel::find(q);
        if (result) {
            this->related = result.value();
            this->conserve(q);
            return true;
        } else {
            this->conserve(q, result.error());
            return false;
        }
    }

    QString foreignKey;
    QString localKey;
};

template<typename RelatedModel>
class HasOneRelationData final : public HasRelationData<RelatedModel>
{
public:
    HasOneRelationData *clone() const override { return new HasOneRelationData(*this); }
    bool multiple() const override { return false; }
};

template<typename RelatedModel>
class HasManyRelationData final : public HasRelationData<RelatedModel>
{
public:
    bool multiple() const override { return true; }
    HasManyRelationData *clone() const override { return new HasManyRelationData(*this); }
};

template<typename RelatedModel, typename ThroughModel>
class HasManyThroughRelationData final : public RelationBaseData<RelatedModel>
{
public:
    void init(NamingConvention *) override
    {
        const MetaObject through = MetaObject::from<ThroughModel>();

        if (localKey.isEmpty())
            localKey = this->primaryObject.primaryProperty().fieldName();

        if (throughForeignKey.isEmpty()) {
            RelatedModel m;
            throughForeignKey = m.metaObject().foreignProperty().fieldName();
        }

        if (throughLocalKey.isEmpty()) {
            throughLocalKey = through.primaryProperty().fieldName();
        }

        if (foreignKey.isEmpty()) {
            foreignKey = through.foreignProperty().fieldName();
        }
    }

    bool get() override
    {
        // SELECT related.* FROM related
        // INNER JOIN through ON related.through_foreign_key = through.through_local_key
        // WHERE through.foreign_key = parent.local_key

        const MetaObject through = MetaObject::from<ThroughModel>();

        Query q;
        q.join(through.tableName(),
               this->relatedObject.tableName() + "." + throughForeignKey,
               "=",
               through.tableName() + "." + throughLocalKey);

        q.where(through.tableName() + "." + foreignKey, this->parentField(localKey));

        auto result = RelatedModel::find(q);
        if (result) {
            this->related = result.value();
            this->conserve(q);
            return true;
        } else {
            this->conserve(q, result.error());
            return false;
        }
    }

    bool multiple() const override { return true; }

    HasManyThroughRelationData *clone() const override { return new HasManyThroughRelationData(*this); }

    QString foreignKey;
    QString localKey;
    QString throughForeignKey;
    QString throughLocalKey;
};

template<typename RelatedModel>
class BelongsToRelationData final : public RelationBaseData<RelatedModel>
{
public:
    void init(NamingConvention *) override
    {
        if (foreignKey.isEmpty())
            foreignKey = this->relatedObject.foreignProperty().fieldName();

        if (ownerKey.isEmpty())
            ownerKey = this->relatedObject.primaryProperty().fieldName();
    }

    bool get() override
    {
        Query q;
        q.where(ownerKey, this->parentField(foreignKey));

        auto result = RelatedModel::find(q);
        if (result) {
            this->related = { result.value() };
            return true;
        }
        return false;
    }

    bool multiple() const override { return false; }

    BelongsToRelationData *clone() const override { return new BelongsToRelationData(*this); }

    QString foreignKey;
    QString ownerKey;
};

template<typename RelatedModel>
class BelongsToManyRelationData final : public RelationBaseData<RelatedModel>
{
public:
    void init(NamingConvention *convention) override
    {
        if (table.isEmpty()) {
            QStringList tables = { this->primaryObject.tableName(), this->relatedObject.tableName() };
            tables.sort();
            table = convention->pivotTableName(tables.first(), tables.last());
        }

        if (foreignPivotKey.isEmpty())
            foreignPivotKey = this->primaryObject.foreignProperty().fieldName();

        if (relatedPivotKey.isEmpty())
            relatedPivotKey = this->relatedObject.foreignProperty().fieldName();

        if (parentKey.isEmpty())
            parentKey = this->primaryObject.primaryProperty().fieldName();

        if (relatedKey.isEmpty())
            relatedKey = this->relatedObject.primaryProperty().fieldName();
    }

    bool get() override
    {
        // SELECT related.* FROM related
        // INNER JOIN pivot ON related.related_key = pivot.related_pivot_key
        // WHERE pivot.foreign_pivot_key = parent.parent_key
        
        Query q;
        q.join(table, 
               this->relatedObject.tableName() + "." + relatedKey,
               "=", 
               table + "." + relatedPivotKey);
        
        q.where(table + "." + foreignPivotKey, this->parentField(parentKey));

        auto result = RelatedModel::find(q);
        if (result) {
            this->related = result.value();
            this->conserve(q);
            return true;
        } else {
            this->conserve(q, result.error());
            return false;
        }
    }

    bool multiple() const override { return true; }

    BelongsToManyRelationData *clone() const override { return new BelongsToManyRelationData(*this); }

    QString table;
    QString foreignPivotKey;
    QString relatedPivotKey;
    QString parentKey;
    QString relatedKey;
};

} // namespace QEloquent

#endif // QELOQUENT_RELATION_IMPL_H
