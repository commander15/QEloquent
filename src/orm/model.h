#ifndef QELOQUENT_MODEL_H
#define QELOQUENT_MODEL_H

#include <QEloquent/global.h>
#include <QEloquent/entity.h>
#include <QEloquent/serializable.h>
#include <QEloquent/deserializable.h>
#include <QEloquent/result.h>
#include <QEloquent/relation.h>

#include <QObject>
#include <QSharedDataPointer>
#include <QDateTime>

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

class QELOQUENT_EXPORT Model : public Entity,
                               public Serializable,
                               public Deserializable
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

    QVariant label() const;

    QVariant property(const QString &name) const;
    void setProperty(const QString &name, const QVariant &value);

    QVariant field(const QString &name) const;
    void setField(const QString &name, const QVariant &value);

    bool exists() override final;
    bool get() override final;
    bool insert() override final;
    bool update() override final;
    bool deleteData() override final;
    bool deletePermanently();

    bool load(const QString &relation);
    bool load(const QStringList &relations);

    Query lastQuery() const;
    Error lastError() const;

    MetaObject metaObject() const;
    Connection connection() const;

    QString serializationContext() const override final;
    bool isListSerializable() const override final;
    QList<DataMap> serialize() const override final;
    void deserialize(const QList<DataMap> &data, bool all = false) override final;

    DataMap fullDataMap() const;

protected:
    template<typename T, std::enable_if<std::is_base_of<Model, T>::value>::type* = nullptr>
    Model(T *self);

    Model(const QMetaObject &metaObject);
    Model(ModelData *data);

    QDateTime now() const;

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
    Query newQuery(const std::function<QString (const Query &)> &statementGenerator, bool filter) const;
    Result<::QSqlQuery, QSqlError> exec(const std::function<QString (const Query &)> &statementGenerator, bool filter);

    friend class MetaObject;
    friend class MetaProperty;
    friend class RelationData;
};

class QELOQUENT_EXPORT SimpleModel : public Model
{
    Q_GADGET
    Q_PROPERTY(qint64 id READ id WRITE setId FINAL)

public:
    SimpleModel(const SimpleModel &);
    SimpleModel(SimpleModel &&);
    SimpleModel &operator=(const SimpleModel &);
    SimpleModel &operator=(SimpleModel &&);
    virtual ~SimpleModel();

    qint64 id() const;
    virtual void setId(qint64 id);

protected:
    template<typename T, std::enable_if<std::is_base_of<SimpleModel, T>::value>::type* = nullptr>
    SimpleModel(T *m) : Model(m) {}

    qint64 m_id = 0;
};

class QELOQUENT_EXPORT StandardModel : public Model
{
    Q_GADGET
    Q_PROPERTY(qint64 id READ id WRITE setId FINAL)
    Q_PROPERTY(QDateTime createdAt MEMBER m_createdAt FINAL)
    Q_PROPERTY(QDateTime updatedAt MEMBER m_updatedAt FINAL)

public:
    StandardModel(const StandardModel &);
    StandardModel(StandardModel &&);
    StandardModel &operator=(const StandardModel &);
    StandardModel &operator=(StandardModel &&);
    virtual ~StandardModel();

    qint64 id() const;
    virtual void setId(qint64 id);

    QDateTime createdAt() const;
    QDateTime updatedAt() const;

protected:
    template<typename T, std::enable_if<std::is_base_of<StandardModel, T>::value>::type* = nullptr>
    StandardModel(T *m) : Model(m) {}

    qint64 m_id = 0;
    QDateTime m_createdAt;
    QDateTime m_updatedAt;
};

/*!
 * \brief Construct a Model instance and use the QMetaObject of 'self' as base for meta functions.
 * \note Typically, you must use QEloquent::Model(this) on your model classes.
 */
template<typename T, std::enable_if<std::is_base_of<Model, T>::value>::type*>
inline Model::Model(T *self) : Model(T::staticMetaObject) {}

} // namespace QEloquent

#include <QEloquent/private/relation_impl.h>

namespace QEloquent {

/*!
 * \fn QEloquent::Model::hasOne
 * \brief Defines a one-to-one relationship.
 * \param foreignKey The foreign key of the related model.
 * \param localKey The local key of the parent model.
 * \param location The source location, used to compute the function name to name relation.
 * \return A Relation object.
 */
template<typename T>
inline Relation<T> Model::hasOne(const QString &foreignKey, const QString &localKey, const std::source_location &location) const
{
    return Relation<T>(location, this, [=]() {
        auto d = new HasOneRelationData<T>();
        d->foreignKey = foreignKey;
        d->localKey = localKey;
        return d;
    });
}

/*!
 * \fn QEloquent::Model::hasMany
 * \brief Defines a one-to-many relationship.
 * \param foreignKey The foreign key of the related model.
 * \param localKey The local key of the parent model.
 * \param location The source location, used to compute the function name to name relation.
 * \return A Relation object.
 */
template<typename T>
inline Relation<T> Model::hasMany(const QString &foreignKey, const QString &localKey, const std::source_location &location) const
{
    return Relation<T>(location, this, [=]() {
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
 * \param throughForeignKey The foreignKey referenced on the through table.
 * \param throughLocalKey The localKey referenced on the through table.
 * \param location The source location, used to compute the function name to name relation.
 * \return A Relation object.
 */
template<typename T, typename Through>
inline Relation<T> Model::hasManyThrough(const QString &foreignKey, const QString &localKey,
                                         const QString &throughForeignKey, const QString &throughLocalKey,
                                         const std::source_location &location) const
{
    return Relation<T>(location, this, [=]() {
        auto d = new HasManyThroughRelationData<T, Through>();
        d->foreignKey = foreignKey;
        d->localKey = localKey;
        d->throughForeignKey = throughForeignKey;
        d->throughLocalKey = throughLocalKey;
        return d;
    });
}

/*!
 * \fn QEloquent::Model::belongsTo
 * \brief Defines an inverse one-to-one or many-to-one relationship.
 * \param foreignKey The foreign key of the current model.
 * \param ownerKey The owner key of the related model.
 * \param location The source location, used to compute the function name to name relation.
 * \return A Relation object.
 */
template<typename T>
inline Relation<T> Model::belongsTo(const QString &foreignKey, const QString &ownerKey, const std::source_location &location) const
{
    return Relation<T>(location, this, [=]() {
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
 * \param parentKey The parent's field that must match.
 * \param relatedKey The related key that must match.
 * \param location The source location, used to compute the function name to name relation.
 * \return A Relation object.
 */
template<typename T>
inline Relation<T> Model::belongsToMany(const QString &table,
                                        const QString &foreignPivotKey, const QString &relatedPivotKey,
                                        const QString &parentKey, const QString &relatedKey,
                                        const std::source_location &location) const
{
    return Relation<T>(location, this, [=]() {
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
 * \param parentKey The parent's field that must match.
 * \param relatedKey The related key that must match.
 * \param location The source location, used to compute the function name to name relation.
 * \return A Relation object.
 */
template<typename T, typename Through>
inline Relation<T> Model::belongsToManyThrough(const QString &table,
                                               const QString &foreignPivotKey, const QString &relatedPivotKey,
                                               const QString &parentKey, const QString &relatedKey,
                                               const std::source_location &location) const
{
    return Relation<T>(location, this, [=]() {
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
