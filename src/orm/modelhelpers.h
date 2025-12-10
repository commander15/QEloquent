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
    static MetaObject metaobject() { return Model().metaObject(); }
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
    query.where(Maker::metaobject().primaryKey(), primary);
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
    const MetaObject metaobject = Maker::metaobject();
    const Connection con = metaobject.connection();

    query.table(metaobject.tableName()).connection(con.name());
    const QString statement = QueryBuilder::selectStatement(query);

    auto result = QueryRunner::exec(statement, con);
    if (result) {
        QList<Model> models;

        QStringList relations = metaobject.relations() + query.relations();
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
    const MetaObject metaobject = Maker::metaobject();
    const Connection con = metaobject.connection();

    query.table(metaobject.tableName()).connection(con.name());
    const QString statement = QueryBuilder::selectStatement("COUNT(1)", query);

    auto result = QueryRunner::exec(statement, con);
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
    const MetaObject metaobject = Maker::metaobject();
    const Connection con = metaobject.connection();

    query.table(metaobject.tableName()).connection(con.name());
    const QString statement = QueryBuilder::deleteStatement(query);

    auto result = QueryRunner::exec(statement, con);
    if (result)
        return result->numRowsAffected();
    else
        return unexpected(Error(Error::DatabaseError, QString(), result.error()));
}

} // namespace QEloquent

#endif // QELOQUENT_MODELHELPERS_H
