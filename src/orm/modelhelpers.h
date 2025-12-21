#ifndef QELOQUENT_MODELHELPERS_H
#define QELOQUENT_MODELHELPERS_H

#include <QEloquent/model.h>
#include <QEloquent/metaobject.h>
#include <QEloquent/query.h>
#include <QEloquent/error.h>
#include <QEloquent/connection.h>
#include <QEloquent/querybuilder.h>
#include <QEloquent/queryrunner.h>

#include <QSqlQuery>
#include <QSqlRecord>
#include <QJsonObject>

namespace QEloquent {

/**
 * @brief Internal helper to create model instances and access metadata.
 * @tparam Model The model type.
 */
template<typename Model>
class ModelMaker {
public:
    /** @brief Creates a default model instance */
    static Model make() { return Model(); }
    /** @brief Returns the MetaObject for the model type */
    static MetaObject metaObject() { return MetaObject::fromQtMetaObject(Model::staticMetaObject); }
};

/**
 * @brief Provides static CRUD and query helpers for Model types.
 * 
 * This class is intended to be used via multiple inheritance in your models:
 * @code
 * struct User : public Model, public ModelHelpers<User> { ... };
 * @endcode
 * 
 * @tparam Model The model type.
 * @tparam Maker Creation strategy (defaults to ModelMaker<Model>).
 */
template<typename Model, typename Maker = ModelMaker<Model>>
class ModelHelpers
{
public:
    /** @brief Creates a model instance with optional JSON data */
    static Model make(const QJsonObject &object = QJsonObject());
    /** @brief Creates a list of model instances from a list of JSON objects */
    static QList<Model> make(const QList<QJsonObject> &objects);

    /** @brief Finds a model by its primary key */
    static Result<Model, Error> find(const QVariant &primary);
    /** @brief Finds models matching the given query */
    static Result<QList<Model>, Error> find(Query query);

    /** @brief Finds models matching the given query and limit output using pagination */
    static Result<QList<Model>, Error> paginate(int page = 1, int itemsPerPage = 20, Query query = Query());

    /** @brief Finds all models, optionaly matching the given query */
    static Result<QList<Model>, Error> all(Query query = Query());

    /** @brief Returns the number of records matching the query */
    static Result<int, Error> count(Query query = Query());

    /** @brief Creates and persists a new model from JSON data */
    static Result<Model, Error> create(const QJsonObject &object);
    /** @brief Creates and persists multiple models from JSON data */
    static Result<QList<Model>, Error> create(const QList<QJsonObject> &objects);

    /** @brief Deletes records matching the given query */
    static Result<int, Error> remove(Query query);

    /** @brief Returns a new Query object initialized for this model's table */
    static Query query();
    /** @brief Configures a Query object for this model's table and connection */
    static Query &fixQuery(Query &query);
    /** @brief Configures a Query object using a specific MetaObject */
    static Query &fixQuery(Query &query, const MetaObject &metaObject);
};

template<typename Model, typename Maker>
inline Model ModelHelpers<Model, Maker>::make(const QJsonObject &object)
{
    Model m = Maker::make();
    m.fill(object);
    return m;
}

template<typename Model, typename Maker>
inline QList<Model> ModelHelpers<Model, Maker>::make(const QList<QJsonObject> &objects)
{
    QList<Model> models;
    for (const QJsonObject &object : objects) {
        Model m = Maker::make();
        m.fill(object);
        models.append(m);
    }
    return models;
}

template<typename Model, typename Maker>
inline Result<Model, Error> ModelHelpers<Model, Maker>::find(const QVariant &primary)
{
    Query query;
    query.where(Maker::metaObject().primaryProperty().fieldName(), primary);
    query.limit(1);

    Result<QList<Model>, Error> result = find(query);
    if (result) {
        const QList<Model> models = result.value();
        return (models.isEmpty() ? Maker::make() : models.first());
    } else {
        return unexpected(result.error());
    }
}

template<typename Model, typename Maker>
inline Result<QList<Model>, Error> ModelHelpers<Model, Maker>::find(Query query)
{
    const MetaObject metaObject = Maker::metaObject();
    const QString statement = QueryBuilder::selectStatement(fixQuery(query, metaObject));

    auto result = QueryRunner::exec(statement, query.connectionName());
    if (result) {
        QList<Model> models;

        QStringList relations = metaObject.relations() + query.relations();
        relations.removeDuplicates();

        while (result->next()) {
            Model m = Maker::make();
            m.fill(result->record());
            if (m.load(relations))
                models.append(m);
            else
                return unexpected(m.lastError());
        }

        return models;
    } else {
        return unexpected(Error::fromSqlError(result.error()));
    }
}

template<typename Model, typename Maker>
inline Result<QList<Model>, Error> ModelHelpers<Model, Maker>::paginate(int page, int itemsPerPage, Query query)
{
    return find(query.page(page, itemsPerPage));
}

template<typename Model, typename Maker>
inline Result<QList<Model>, Error> ModelHelpers<Model, Maker>::all(Query query)
{
    return find(query);
}

template<typename Model, typename Maker>
inline Result<int, Error> ModelHelpers<Model, Maker>::count(Query query)
{
    const QString statement = QueryBuilder::selectStatement("COUNT(1)", fixQuery(query));

    auto result = QueryRunner::exec(statement, query.connectionName());
    if (result)
        return (result->next() ? result->value(0).toInt() : 0);
    else
        return unexpected(Error::fromSqlError(result.error()));
}

template<typename Model, typename Maker>
inline Result<Model, Error> ModelHelpers<Model, Maker>::create(const QJsonObject &object)
{
    Model model = Maker::make();
    model.fill(object);
    if (model.save())
        return model;
    else
        return unexpected(model.lastError());
}

template<typename Model, typename Maker>
inline Result<QList<Model>, Error> ModelHelpers<Model, Maker>::create(const QList<QJsonObject> &objects)
{
    QList<Model> models;

    for (const QJsonObject &object : objects) {
        auto result = create(object);
        if (result)
            models.append(result.value());
        else
            return unexpected(result.error());
    }

    return models;
}

template<typename Model, typename Maker>
inline Result<int, Error> ModelHelpers<Model, Maker>::remove(Query query)
{
    const QString statement = QueryBuilder::deleteStatement(fixQuery(query));

    auto result = QueryRunner::exec(statement, query.connectionName());
    if (result)
        return result->numRowsAffected();
    else
        return unexpected(Error(Error::DatabaseError, QString(), result.error()));
}

template<typename Model, typename Maker>
inline Query ModelHelpers<Model, Maker>::query()
{
    Query query;
    return fixQuery(query, Maker::metaObject());
}

template<typename Model, typename Maker>
inline Query &ModelHelpers<Model, Maker>::fixQuery(Query &query)
{
    return fixQuery(query, Maker::metaObject());
}

template<typename Model, typename Maker>
inline Query &ModelHelpers<Model, Maker>::fixQuery(Query &query, const MetaObject &metaObject)
{
    query.table(metaObject.tableName()).connection(metaObject.connectionName());
    return query;
}

} // namespace QEloquent

#endif // QELOQUENT_MODELHELPERS_H
