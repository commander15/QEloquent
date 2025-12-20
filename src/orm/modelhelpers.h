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

template<typename Model>
class ModelMaker {
public:
    static Model make() { return Model(); }
    static MetaObject metaObject() { return MetaObject::fromQtMetaObject(Model::staticMetaObject); }
};

template<typename Model, typename Maker = ModelMaker<Model>>
class ModelHelpers
{
public:
    static Model make(const QJsonObject &object = QJsonObject());
    static QList<Model> make(const QList<QJsonObject> &objects);

    static Result<Model, Error> find(const QVariant &primary);
    static Result<QList<Model>, Error> find(Query query);

    static Result<int, Error> count(Query query = Query());

    static Result<Model, Error> create(const QJsonObject &object);
    static Result<QList<Model>, Error> create(const QList<QJsonObject> &objects);

    static Result<int, Error> remove(Query query);

    static Query query();
    static Query &fixQuery(Query &query);
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
