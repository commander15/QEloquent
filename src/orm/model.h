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

/**
 * @brief The Model class is the base class for all ORM models.
 * 
 * It provides the core functionality for attribute management, persistence,
 * and relationship definition. Models are typically defined using Q_GADGET
 * and Q_PROPERTY macros to enable reflection and database mapping.
 */
class QELOQUENT_EXPORT Model : public Entity
{
    Q_GADGET

public:
    /** @brief Copy constructor */
    Model(const Model &);
    /** @brief Move constructor */
    Model(Model &&);
    /** @brief Copy assignment operator */
    Model &operator=(const Model &);
    /** @brief Move assignment operator */
    Model &operator=(Model &&);
    /** @brief Virtual destructor */
    virtual ~Model();

    /** @brief Returns the value of the primary key */
    QVariant primary() const;
    /** @brief Sets the primary key value */
    void setPrimary(const QVariant &value);

    /** @brief Returns a property value by name */
    QVariant property(const QString &name) const;
    /** @brief Sets a property value by name */
    void setProperty(const QString &name, const QVariant &value);

    /** @brief Fills the model with data from a map (field names as keys) */
    void fill(const QVariantMap &values);
    /** @brief Fills the model with data from a JSON object */
    void fill(const QJsonObject &object);
    /** @brief Fills the model with data from a SQL record */
    void fill(const QSqlRecord &record);

    /** @brief Returns true if the model exists in the database */
    bool exists() override;
    /** @brief Refreshes the model data from the database */
    bool get() override;
    /** @brief Inserts the model into the database */
    bool insert() override;
    /** @brief Updates the model in the database */
    bool update() override;
    /** @brief Deletes the model from the database */
    bool deleteData() override;

    /** @brief Eagerly loads a relationship */
    bool load(const QString &relation);
    /** @brief Eagerly loads multiple relationships */
    bool load(const QStringList &relations);

    /** @brief Returns the last query executed by this model */
    Query lastQuery() const;
    /** @brief Returns the last error encountered by this model */
    Error lastError() const;

    /** @brief Returns the metadata object for this model */
    MetaObject metaObject() const;
    /** @brief Returns the database connection used by this model */
    Connection connection() const;

    /** @brief Converts the model to a JSON object */
    QJsonObject toJsonObject() const;
    /** @brief Converts the model to a SQL record matching the table structure */
    QSqlRecord toSqlRecord() const;

protected:
    /** @brief Internal constructor for derived models */
    template<typename T, std::enable_if<std::is_base_of<Model, T>::value>::type* = nullptr> Model(T *self);
    /** @brief Constructor from existing MetaObject */
    Model(const QMetaObject &metaObject);
    /** @brief Constructor from internal data */
    Model(ModelData *data);

    /** @brief Defines a 1:1 relationship where the other model has the foreign key */
    template<typename T>
    Relation<T> hasOne(const QString &foreignKey = QString(), const QString &localKey = QString()) const;

    /** @brief Defines a 1:1 or N:1 relationship where this model has the foreign key */
    template<typename T>
    Relation<T> belongsTo(const QString &foreignKey = QString(), const QString &ownerKey = QString()) const;

    /** @brief Defines a 1:N relationship */
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
