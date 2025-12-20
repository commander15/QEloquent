#ifndef QELOQUENT_RELATION_IMPL_H
#define QELOQUENT_RELATION_IMPL_H

#include "relation.h"
#include <QEloquent/namingconvention.h>
#include <QEloquent/query.h>

namespace QEloquent {

template<typename RelatedModel>
class BelongsToRelationData : public RelationBaseData<RelatedModel>
{
public:
    BelongsToRelationData *clone() const override { return new BelongsToRelationData(*this); }

    QString foreignKey;
    QString ownerKey;

    bool get() override
    {
        NamingConvention *convention = this->primaryObject.namingConvention();
        QString fk = foreignKey;
        if (fk.isEmpty()) {
            fk = convention->foreignPropertyName(this->relatedObject.primaryProperty().propertyName(), this->relatedObject.className());
        }

        QVariant keyVal = this->parent->property(fk);
        if (!keyVal.isValid() || keyVal.isNull()) {
            MetaProperty prop = this->primaryObject.property(fk, MetaObject::ResolveByFieldName);
            if (prop.isValid()) keyVal = prop.read(this->parent);
        }

        if (!keyVal.isValid() || keyVal.isNull()) return false;

        auto result = RelatedModel::find(keyVal);
        if (result) {
            this->related = { result.value() };
            return true;
        }
        return false;
    }
};

template<typename RelatedModel>
class HasOneRelationData : public RelationBaseData<RelatedModel>
{
public:
    HasOneRelationData *clone() const override { return new HasOneRelationData(*this); }

    QString foreignKey;
    QString localKey;

    bool get() override
    {
        NamingConvention *convention = this->primaryObject.namingConvention();
        QString fk = foreignKey;
        if (fk.isEmpty()) {
            fk = convention->foreignFieldName(this->primaryObject.primaryProperty().fieldName(), this->primaryObject.tableName());
        }

        QString lk = localKey;
        if (lk.isEmpty()) {
            lk = this->primaryObject.primaryProperty().propertyName();
        }
        
        QVariant localKeyVal = this->parent->property(lk);
        if (!localKeyVal.isValid() || localKeyVal.isNull()) {
            MetaProperty prop = this->primaryObject.property(lk, MetaObject::ResolveByFieldName);
            if (prop.isValid()) localKeyVal = prop.read(this->parent);
        }

        Query q;
        q.where(fk, localKeyVal);
        q.limit(1);
        
        auto result = RelatedModel::find(q);
        if (result && !result.value().isEmpty()) {
            this->related = { result.value().first() };
            return true;
        }
        return false;
    }
};

template<typename RelatedModel>
class HasManyRelationData : public RelationBaseData<RelatedModel>
{
public:
    HasManyRelationData *clone() const override { return new HasManyRelationData(*this); }

    QString foreignKey;
    QString localKey;

    bool get() override
    {
        NamingConvention *convention = this->primaryObject.namingConvention();
        QString fk = foreignKey;
        if (fk.isEmpty()) {
            fk = convention->foreignFieldName(this->primaryObject.primaryProperty().fieldName(), this->primaryObject.tableName());
        }

        QString lk = localKey;
        if (lk.isEmpty()) {
            lk = this->primaryObject.primaryProperty().propertyName();
        }
        
        QVariant localKeyVal = this->parent->property(lk);
        if (!localKeyVal.isValid() || localKeyVal.isNull()) {
            MetaProperty prop = this->primaryObject.property(lk, MetaObject::ResolveByFieldName);
            if (prop.isValid()) localKeyVal = prop.read(this->parent);
        }

        Query q;
        q.where(fk, localKeyVal);
        
        auto result = RelatedModel::find(q);
        if (result) {
            this->related = result.value();
            return true;
        }
        return false;
    }
};

} // namespace QEloquent

#endif // QELOQUENT_RELATION_IMPL_H
