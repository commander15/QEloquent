#ifndef QELOQUENT_RELATION_IMPL_H
#define QELOQUENT_RELATION_IMPL_H

#include <QEloquent/relation.h>
#include <QEloquent/namingconvention.h>
#include <QEloquent/query.h>

namespace QEloquent {

template<typename RelatedModel>
class HasOneRelationData : public RelationBaseData<RelatedModel>
{
public:
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
        if (!multiple()) q.limit(1);
        
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

    bool multiple() const override { return false; }

    HasOneRelationData *clone() const override { return new HasOneRelationData(*this); }

    QString foreignKey;
    QString localKey;
};

template<typename RelatedModel>
class HasManyRelationData : public HasOneRelationData<RelatedModel>
{
public:
    bool multiple() const override { return true; }
    HasManyRelationData *clone() const override { return new HasManyRelationData(*this); }
};

template<typename RelatedModel>
class BelongsToRelationData : public RelationBaseData<RelatedModel>
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

} // namespace QEloquent

#endif // QELOQUENT_RELATION_IMPL_H
