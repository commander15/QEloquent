#ifndef QELOQUENT_MODEL_H
#define QELOQUENT_MODEL_H

#include <QEloquent/global.h>
#include <QEloquent/entity.h>
#include <QEloquent/result.h>
#include <QEloquent/relation.h>

#include <QObject>
#include <QSharedDataPointer>

#include <source_location>

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

    QVariant field(const QString &name) const;
    void setField(const QString &name, const QVariant &value);

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
    Relation<T> hasOne(const QString &foreignKey = QString(), const QString &localKey = QString(),
                       const std::source_location &location = std::source_location::current()) const;

    template<typename T>
    Relation<T> hasMany(const QString &foreignKey = QString(), const QString &localKey = QString(),
                        const std::source_location &location = std::source_location::current()) const;

    template<typename T, typename Through>
    Relation<T> hasManyThrough(const QString &foreignKey = QString(), const QString &localKey = QString(),
                               const QString &throughForeignKey = QString(), const QString &throughLocalKey = QString(),
                               const std::source_location &location = std::source_location::current()) const;

    template<typename T>
    Relation<T> belongsTo(const QString &foreignKey = QString(), const QString &ownerKey = QString(),
                          const std::source_location &location = std::source_location::current()) const;

    template<typename T>
    Relation<T> belongsToMany(const QString &table = QString(), const QString &foreignPivotKey = QString(),
                              const QString &relatedPivotKey = QString(), const QString &parentKey = QString(),
                              const QString &relatedKey = QString(),
                              const std::source_location &location = std::source_location::current()) const;

    template<typename T, typename Through>
    Relation<T> belongsToManyThrough(const QString &table = QString(), const QString &foreignPivotKey = QString(),
                                     const QString &relatedPivotKey = QString(), const QString &parentKey = QString(),
                                     const QString &relatedKey = QString(),
                                     const std::source_location &location = std::source_location::current()) const;

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

template<typename T, std::enable_if<std::is_base_of<Model, T>::value>::type*>
inline Model::Model(T *) : Model(T::staticMetaObject) {}

/*!
 * \brief Defines a one-to-one relationship.
 * \param foreignKey The foreign key of the related model.
 * \param localKey The local key of the parent model.
 * \return A Relation object.
 */
template<typename T>
inline Relation<T> Model::hasOne(const QString &foreignKey, const QString &localKey, const std::source_location &location) const
{
    return Relation<T>(location, const_cast<Model *>(this), [=]() {
        auto d = new HasOneRelationData<T>();
        d->foreignKey = foreignKey;
        d->localKey = localKey;
        return d;
    });
}

/*!
 * \brief Defines a one-to-many relationship.
 * \param foreignKey The foreign key of the related model.
 * \param localKey The local key of the parent model.
 * \return A Relation object.
 */
template<typename T>
inline Relation<T> Model::hasMany(const QString &foreignKey, const QString &localKey, const std::source_location &location) const
{
    return Relation<T>(location, const_cast<Model *>(this), [=]() {
        auto d = new HasManyRelationData<T>();
        d->foreignKey = foreignKey;
        d->localKey = localKey;
        return d;
    });
}

/*!
 * \brief Defines a has-many-through relationship.
 * \param foreignKey The foreign key of the intermediate model.
 * \param localKey The local key of the parent model.
 * \return A Relation object.
 */
template<typename T, typename Through>
inline Relation<T> Model::hasManyThrough(const QString &foreignKey, const QString &localKey,
                                         const QString &throughForeignKey, const QString &throughLocalKey, const std::source_location &location) const
{
    return Relation<T>(location, const_cast<Model *>(this), [=]() {
        auto d = new HasManyThroughRelationData<T, Through>();
        d->foreignKey = foreignKey;
        d->localKey = localKey;
        d->throughForeignKey = throughForeignKey;
        d->throughLocalKey = throughLocalKey;
        return d;
    });
}

/*!
 * \brief Defines an inverse one-to-one or many-to-one relationship.
 * \param foreignKey The foreign key of the current model.
 * \param ownerKey The owner key of the related model.
 * \return A Relation object.
 */
template<typename T>
inline Relation<T> Model::belongsTo(const QString &foreignKey, const QString &ownerKey, const std::source_location &location) const
{
    return Relation<T>(location, const_cast<Model *>(this), [=]() {
        auto d = new BelongsToRelationData<T>();
        d->foreignKey = foreignKey;
        d->ownerKey = ownerKey;
        return d;
    });
}

/*!
 * \brief Defines a many-to-many relationship.
 * \param table The pivot table name.
 * \param foreignPivotKey The foreign key of the current model in the pivot table.
 * \param relatedPivotKey The foreign key of the related model in the pivot table.
 * \return A Relation object.
 */
template<typename T>
inline Relation<T> Model::belongsToMany(const QString &table, const QString &foreignPivotKey,
                                        const QString &relatedPivotKey, const QString &parentKey,
                                        const QString &relatedKey, const std::source_location &location) const
{
    return Relation<T>(location, const_cast<Model *>(this), [=]() {
        auto d = new BelongsToManyRelationData<T>();
        d->table = table;
        d->foreignPivotKey = foreignPivotKey;
        d->relatedPivotKey = relatedPivotKey;
        d->parentKey = parentKey;
        d->relatedKey = relatedKey;
        return d;
    });
}

/*!
 * \brief Defines a belongs-to-many-through relationship.
 * \param table The pivot table name.
 * \param foreignPivotKey The foreign key of the intermediate model in the pivot table.
 * \param relatedPivotKey The foreign key of the related model in the pivot table.
 * \return A Relation object.
 */
template<typename T, typename Through>
inline Relation<T> Model::belongsToManyThrough(const QString &table, const QString &foreignPivotKey,
                                               const QString &relatedPivotKey, const QString &parentKey,
                                               const QString &relatedKey, const std::source_location &location) const
{
    return Relation<T>(location, const_cast<Model *>(this), [=]() {
        auto d = new BelongsToManyThroughRelationData<T, Through>();
        d->table = table;
        d->foreignPivotKey = foreignPivotKey;
        d->relatedPivotKey = relatedPivotKey;
        d->parentKey = parentKey;
        d->relatedKey = relatedKey;
        return d;
    });
}

} // namespace QEloquent

#endif // QELOQUENT_MODEL_H
