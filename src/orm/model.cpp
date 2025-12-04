#include "model.h"
#include "model_p.h"

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
    : data(new MetaModelData(&metaObject))
{}

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
    MODEL_DATA(const Model);
    const QString primaryKey = data.info.primaryKey();
    return data.readValue(primaryKey, this);
}

void Model::setPrimary(const QVariant &value)
{
    MODEL_DATA(Model);
    const QString primaryKey = data.info.primaryKey();
    data.writeValue(primaryKey, value, this);
}

QDateTime Model::createdAt() const
{
    MODEL_DATA(const Model);
    const QString field = data.info.createdAt();
    return data.readValue(field, this).toDateTime();
}

QDateTime Model::updatedAt() const
{
    MODEL_DATA(const Model);
    const QString field = data.info.updatedAt();
    return data.readValue(field, this).toDateTime();
}

QDateTime Model::deletedAt() const
{
    MODEL_DATA(const Model);
    const QString field = data.info.deletedAt();
    return data.readValue(field, this).toDateTime();
}

QVariant Model::value(const QString &field) const
{
    MODEL_DATA(const Model);
    return data.readValue(field, this);
}

void Model::setValue(const QString &field, const QVariant &value)
{
    MODEL_DATA(Model);
    data.writeValue(field, value, this);
}

QVariant Model::property(const QString &name) const
{
    MODEL_DATA(const Model);
    const QString field = data.fieldName(name);
    return data.readValue(field, this);
}

void Model::setProperty(const QString &name, const QVariant &value)
{
    MODEL_DATA(Model);
    const QString field = data.fieldName(name);
    data.writeValue(field, value, this);
}

void Model::fill(const QJsonObject &object)
{
    MODEL_DATA(Model);
    const QStringList fields = object.keys();
    for (const QString &field : fields)
        data.writeValue(field, object.value(field).toVariant(), this);
}

void Model::fill(const QSqlRecord &record)
{
    MODEL_DATA(Model);
    for (int i(0); i < record.count(); ++i)
        data.writeValue(record.fieldName(i), record.value(i), this);
}

bool Model::exists() const
{
    MODEL_DATA(const Model);
    const QString primaryKey = data.info.primaryKey();
    return data.hasValue(primaryKey, this);
}

bool Model::get()
{
    MODEL_DATA(Model);

    if (!data.hasValue(data.info.primaryKey(), this)) {
        data.lastError = ModelError(ModelError::NotFoundError, "Primary key not provided");
        return false;
    }

    ModelQuery query;
    query.where(data.info.primaryKey(), primary());
    data.lastQuery = query;

    const QString statement = QueryBuilder::selectStatement(query, data.info);
    auto result = QueryRunner::exec(statement, data.info.connection());

    if (result) {
        if (result->next()) {
            fill(result->record());
            return load(data.info.with());
        } else {
            data.lastError = ModelError(ModelError::NotFoundError, "Model not found");
        }
    } else {
        data.lastError = ModelError(ModelError::DatabaseError, QString(), result.error());
    }

    return false;
}

bool Model::insert()
{
    MODEL_DATA(Model);

    const QVariantMap values = data.fieldsData(data.info.fields(), this);
    const QString statement = QueryBuilder::insertStatement(values, data.info);
    auto result = QueryRunner::exec(statement, data.info.connection());

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

    ModelQuery query;
    query.where(data.info.primaryKey(), primary());

    const QVariantMap values = data.fieldsData(data.info.fillables(), this);
    const QString statement = QueryBuilder::updateStatement(query, values, data.info);

    auto result = QueryRunner::exec(statement, data.info.connection());
    return (result ? result->numRowsAffected() > 0 : false);
}

bool Model::deleteData()
{
    MODEL_DATA(Model);

    ModelQuery query;
    query.where(data.info.primaryKey(), primary());

    const QString statement = QueryBuilder::deleteStatement(query, data.info);

    auto result = QueryRunner::exec(statement, data.info.connection());
    return (result ? result->numRowsAffected() > 0 : false);
}

bool Model::load(const QString &relation)
{
    return load(QStringList() << relation);
}

bool Model::load(const QStringList &relations)
{
    for (const QString &relation : relations) {
        //
    }

    return true;
}

ModelQuery Model::lastQuery() const
{
    MODEL_DATA(const Model);
    return data.lastQuery;
}

ModelError Model::lastError() const
{
    MODEL_DATA(const Model);
    return data.lastError;
}

ModelInfo Model::modelInfo() const
{
    MODEL_DATA(const Model);
    return data.info;
}

Connection Model::connection() const
{
    MODEL_DATA(const Model);
    return data.info.connection();
}

} // namespace QEloquent

template<>
QEloquent::ModelData *QSharedDataPointer<QEloquent::ModelData>::clone()
{
    return d->clone();
}
