#ifndef QELOQUENT_MODEL_H
#define QELOQUENT_MODEL_H

#include <QEloquent/global.h>
#include <QEloquent/entity.h>
#include <QEloquent/result.h>
#include <QEloquent/relation.h>

#include <QObject>
#include <QSharedDataPointer>

class QSqlRecord;
class QSqlQuery;
class QSqlError;

namespace QEloquent {

class MetaObject;
class Query;
class Error;
class Connection;

class ModelData;
class QELOQUENT_EXPORT Model : public Entity
{
    Q_GADGET

public:
    Model(const Model &);
    Model(Model &&);
    Model &operator=(const Model &);
    Model &operator=(Model &&);
    virtual ~Model();

    QVariant primary() const;
    void setPrimary(const QVariant &value);

    QVariant property(const QString &name) const;
    void setProperty(const QString &name, const QVariant &value);

    void fill(const QVariantMap &values);
    void fill(const QJsonObject &object);
    void fill(const QSqlRecord &record);

    bool exists() override;
    bool get() override;
    bool insert() override;
    bool update() override;
    bool deleteData() override;

    bool load(const QString &relation);
    bool load(const QStringList &relations);

    Query lastQuery() const;
    Error lastError() const;

    MetaObject metaObject() const;
    Connection connection() const;

    QJsonObject toJsonObject() const;
    QSqlRecord toSqlRecord() const;

protected:
    template<typename T, std::enable_if<std::is_base_of<Model, T>::value>::type* = nullptr> Model(T *self);
    Model(const QMetaObject &metaObject);
    Model(ModelData *data);

    template<typename T>
    Relation<T> hasOne(const QString &foreignKey = QString(), const QString &localKey = QString()) const;

    template<typename T>
    Relation<T> belongsTo(const QString &foreignKey = QString(), const QString &ownerKey = QString()) const;

    template<typename T>
    Relation<T> hasMany(const QString &foreignKey = QString(), const QString &localKey = QString()) const;

    QSharedDataPointer<ModelData> data;

private:
    Query newQuery(bool filter = true) const;
    Result<::QSqlQuery, ::QSqlError> exec(const QString &statement, const Query &query);

    friend class MetaProperty;
    friend class RelationData;
};

} // namespace QEloquent

#include "relation_impl.h"

namespace QEloquent {

template<typename T>
inline Relation<T> Model::hasOne(const QString &foreignKey, const QString &localKey) const
{
    return Relation<T>("hasOne", const_cast<Model *>(this), [=]() {
        auto d = new HasOneRelationData<T>();
        d->name = "hasOne";
        d->foreignKey = foreignKey;
        d->localKey = localKey;
        return d;
    });
}

template<typename T>
inline Relation<T> Model::hasMany(const QString &foreignKey, const QString &localKey) const
{
    return Relation<T>("hasMany", const_cast<Model *>(this), [=]() {
        auto d = new HasManyRelationData<T>();
        d->name = "hasMany";
        d->foreignKey = foreignKey;
        d->localKey = localKey;
        return d;
    });
}

template<typename T>
inline Relation<T> Model::belongsTo(const QString &foreignKey, const QString &ownerKey) const
{
    return Relation<T>("belongsTo", const_cast<Model *>(this), [=]() {
        auto d = new BelongsToRelationData<T>;
        d->name = "belongsTo";
        d->foreignKey = foreignKey;
        d->ownerKey = ownerKey;
        return d;
    });
}

template<typename T, std::enable_if<std::is_base_of<Model, T>::value>::type*>
inline Model::Model(T *) : Model(T::staticMetaObject) {}

} // namespace QELoquent

#endif // QELOQUENT_MODEL_H
