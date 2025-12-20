#include "model.h"
#include "model_p.h"

#include <QEloquent/metaproperty.h>
#include <QEloquent/querybuilder.h>
#include <QEloquent/queryrunner.h>
#include <QEloquent/connection.h>

#include <QVariant>
#include <QDateTime>
#include <QJsonObject>
#include <QSqlRecord>
#include <QSqlQuery>

#define MODEL_DATA(Class) Class##Data &data = *static_cast<Class##Data *>(Model::data.get());

namespace QEloquent {

Model::Model(const QMetaObject &metaObject)
    : data(new ModelData())
{
    data->metaObject = MetaObject::fromQtMetaObject(metaObject);
}

Model::Model(ModelData *data)
    : data(data)
{}

Model::Model(const Model &rhs)
    : data{rhs.data}
{}

Model::Model(Model &&rhs)
    : data{std::move(rhs.data)}
{}

Model &Model::operator=(const Model &rhs)
{
    if (this != &rhs)
        data = rhs.data;
    return *this;
}

Model &Model::operator=(Model &&rhs)
{
    if (this != &rhs)
        data = std::move(rhs.data);
    return *this;
}

Model::~Model()
{}

QVariant Model::primary() const
{
    return data->metaObject.primaryProperty().read(this);
}

void Model::setPrimary(const QVariant &value)
{
    data->metaObject.primaryProperty().write(this, value);
}

QVariant Model::property(const QString &name) const
{
    MODEL_DATA(const Model);
    const MetaProperty property = data.metaObject.property(name);
    return (property.isValid() ? property.read(this) : data.dynamicProperties.value(name));
}

void Model::setProperty(const QString &name, const QVariant &value)
{
    MODEL_DATA(Model);
    const MetaProperty property = data.metaObject.property(name);
    if (property.isValid())
        property.write(this, value);
    else if (value.isNull() && data.dynamicProperties.contains(name))
        data.dynamicProperties.remove(name);
    else
        data.dynamicProperties.insert(name, value);
}

void Model::fill(const QVariantMap &values)
{
    MODEL_DATA(Model);
    const QStringList fields = values.keys();
    for (const QString &field : fields) {
        const MetaProperty property = data.metaObject.property(field, MetaObject::ResolveByFieldName);
        if (property.isValid())
            property.write(this, values.value(field));
    }
}

void Model::fill(const QJsonObject &object)
{
    fill(object.toVariantMap());
}

void Model::fill(const QSqlRecord &record)
{
    MODEL_DATA(Model);
    for (int i(0); i < record.count(); ++i) {
        const MetaProperty property = data.metaObject.property(record.fieldName(i), MetaObject::ResolveByFieldName);
        if (property.isValid())
            property.write(this, record.value(i));
    }
}

bool Model::exists()
{
    const Query query = newQuery(true);
    auto result = exec(QueryBuilder::selectStatement("COUNT(1)", query), query);
    if (result && result->next())
        return result->value(0).toInt() > 0;
    else
        return false;
}

bool Model::get()
{
    MODEL_DATA(Model);

    if (!primary().isValid()) {
        data.lastError = Error(Error::NotFoundError, "Primary key not provided");
        return false;
    }

    const Query query = newQuery(true);
    auto result = exec(QueryBuilder::selectStatement(query), query);

    if (result) {
        if (result->next()) {
            fill(result->record());
            return load(data.metaObject.relations());
        } else {
            data.lastError = Error(Error::NotFoundError, "Model not found");
        }
    } else {
        data.lastError = Error(Error::DatabaseError, QString(), result.error());
    }

    return false;
}

bool Model::insert()
{
    MODEL_DATA(Model);

    const QVariantMap values = data.metaObject.readFillableFields(this);

    const Query query = newQuery(true);
    auto result = exec(QueryBuilder::insertStatement(values, query), query);

    if (result) {
        setPrimary(result->lastInsertId());
        return true;
    } else {
        return false;
    }
}

bool Model::update()
{
    MODEL_DATA(Model);

    const QVariantMap values = data.metaObject.readFillableFields(this);

    const Query query = newQuery(true);
    auto result = exec(QueryBuilder::updateStatement(values, query), query);
    return (result ? result->numRowsAffected() > 0 : false);
}

bool Model::deleteData()
{
    MODEL_DATA(Model);

    const Query query = newQuery(true);
    auto result = exec(QueryBuilder::deleteStatement(query), query);
    return (result ? result->numRowsAffected() > 0 : false);
}

bool Model::load(const QString &relation)
{
    return load(QStringList() << relation);
}

bool Model::load(const QStringList &relations)
{
    for (const QString &relation : relations) {
        const MetaProperty property = data->metaObject.property(relation);
        if (property.isValid()) {
            // We just need to call the function to load relations
            const QVariant value = property.read(this);

            // Something wired happened, we stop here
            if (value.isNull())
                return false;
        }
    }

    return true;
}

Query Model::lastQuery() const
{
    MODEL_DATA(const Model);
    return data.lastQuery;
}

Error Model::lastError() const
{
    MODEL_DATA(const Model);
    return data.lastError;
}

MetaObject Model::metaObject() const
{
    return data->metaObject;
}

Connection Model::connection() const
{
    MODEL_DATA(const Model);
    return data.metaObject.connection();
}

QJsonObject Model::toJsonObject() const
{
    return QJsonObject::fromVariantMap(data->dynamicProperties);
}

QSqlRecord Model::toSqlRecord() const
{
    return QSqlRecord();
}

Query Model::newQuery(bool filter) const
{
    Query query;
    query.table(data->metaObject.tableName())
        .connection(data->metaObject.connectionName());

    if (filter) {
        const MetaProperty primaryProperty = data->metaObject.primaryProperty();
        const QVariant primary = primaryProperty.read(this);
        query.where(primaryProperty.fieldName(), primary);
    }

    return query;
}

Result<::QSqlQuery, ::QSqlError> Model::exec(const QString &statement, const Query &query)
{
    data->lastQuery = query;

    auto result = QueryRunner::exec(statement, query.connection());
    if (!result)
        data->lastError = Error::fromSqlError(result.error());
    return result;
}

} // namespace QEloquent
