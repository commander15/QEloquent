#ifndef QELOQUENT_MODELHELPERS_H
#define QELOQUENT_MODELHELPERS_H

#include <QEloquent/model_a.h>
#include <QEloquent/modelmaker.h>
#include <QEloquent/queryhelper.h>
#include <QEloquent/connection.h>

#include <QSqlQuery>
#include <QSqlRecord>
#include <QJsonObject>

#define QELOQUENT_HELPERS(Class) \
public: \
    using QEloquent::ModelHelpers<Class>::make; \
    using QEloquent::ModelHelpers<Class>::create; \
    using QEloquent::ModelHelpers<Class>::find; \
    using QEloquent::ModelHelpers<Class>::count; \
    using QEloquent::ModelHelpers<Class>::all; \
    using QEloquent::ModelHelpers<Class>::where; \
    using QEloquent::ModelHelpers<Class>::with; \
    using QEloquent::ModelHelpers<Class>::query; \
private:

namespace QEloquent {

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

    /** @brief Creates and persists a new model from JSON data */
    static Result<Model, Error> create(const QJsonObject &object);
    /** @brief Creates and persists multiple models from JSON data */
    static Result<QList<Model>, Error> create(const QList<QJsonObject> &objects);

    /** @brief Finds a model by its primary key */
    static Result<Model, Error> find(const QVariant &primary);

    /** @brief Counts the total number of records matching the model's table criteria. */
    static Result<int, Error> count() { return query().count(); }

    /** @brief Finds all models */
    static Result<QList<Model>, Error> all() { return query().get(); }

    /** \brief Adds a basic where clause to the query with an implicit equality operator. */
    static QueryHelper<Model, Maker> where(const QString &field, const QVariant &value) { return query().where(field, value); }
    /** \brief Adds a conditional where clause to the query using a custom operator. */
    static QueryHelper<Model, Maker> where(const QString &field, const QString &op, const QVariant &value) { return query().where(field, op, value); }
    /** \brief Adds a raw SQL expression directly to the query filter. */
    static QueryHelper<Model, Maker> whereRaw(const QString &expression) { return query().whereRaw(expression); }

    /** @brief Eager loads a single relationship for the query results. */
    static QueryHelper<Model, Maker> with(const QString &relation) { return query().with(QStringList() << relation); }
    /** @brief Eager loads multiple relationships for the query results. */
    static QueryHelper<Model, Maker> with(const QStringList &relations) { return query().with(relations); }

    /** @brief Returns a new Query object initialized for this model's table */
    static QueryHelper<Model, Maker> query() { return QueryHelper<Model, Maker>(); }
};

template<typename Model, typename Maker>
inline Model ModelHelpers<Model, Maker>::make(const QJsonObject &object)
{
    Model m = Maker::make();
    QueryHelper<Model, Maker>::safeModel(m).fill(object);
    return m;
}

template<typename Model, typename Maker>
inline QList<Model> ModelHelpers<Model, Maker>::make(const QList<QJsonObject> &objects)
{
    QList<Model> models;
    for (const QJsonObject &object : objects) {
        Model m = Maker::make();
        QueryHelper<Model, Maker>::safeModel(m).fill(object);
        models.append(m);
    }
    return models;
}

template<typename Model, typename Maker>
inline Result<Model, Error> ModelHelpers<Model, Maker>::find(const QVariant &primary)
{
    return query()
        .where(Maker::metaObject().primaryProperty().fieldName(), primary)
        .limit(1)
        .first();
}

template<typename Model, typename Maker>
inline Result<Model, Error> ModelHelpers<Model, Maker>::create(const QJsonObject &object)
{
    Model model = Maker::make();
    QueryHelper<Model, Maker>::safeModel(model).fill(object);
    if (QueryHelper<Model, Maker>::safeModel(model).save())
        return model;
    else
        return failWith(safeModel(model).lastError());
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
            return failWith(result.error());
    }

    return models;
}

} // namespace QEloquent

#endif // QELOQUENT_MODELHELPERS_H
