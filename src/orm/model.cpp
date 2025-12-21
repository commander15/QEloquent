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

/*!
 * \class QEloquent::Model
 * \brief The Model class is the base class for all ORM models.
 *
 * It provides the core functionality for attribute management, persistence,
 * and relationship definition. Models are typically defined using Q_GADGET
 * and Q_PROPERTY macros to enable reflection and database mapping.
 */

Model::Model(const QMetaObject &metaObject)
    : data(new ModelData())
{
    data->metaObject = MetaObject::fromQtMetaObject(metaObject);
}

/*!
 * \brief Constructor from internal data.
 */
Model::Model(ModelData *data)
    : data(data)
{}

/*!
 * \brief Copy constructor.
 */
Model::Model(const Model &rhs)
    : data{rhs.data}
{}

/*!
 * \brief Move constructor.
 */
Model::Model(Model &&rhs)
    : data{std::move(rhs.data)}
{}

/*!
 * \brief Copy assignment operator.
 */
Model &Model::operator=(const Model &rhs)
{
    if (this != &rhs)
        data = rhs.data;
    return *this;
}

/*!
 * \brief Move assignment operator.
 */
Model &Model::operator=(Model &&rhs)
{
    if (this != &rhs)
        data = std::move(rhs.data);
    return *this;
}

/*!
 * \brief Virtual destructor.
 */
Model::~Model()
{}

/*!
 * \brief Returns the value of the primary key.
 */
QVariant Model::primary() const
{
    return data->metaObject.primaryProperty().read(this);
}

/*!
 * \brief Sets the primary key value.
 */
void Model::setPrimary(const QVariant &value)
{
    data->metaObject.primaryProperty().write(this, value);
}

/*!
 * \brief Returns a property value by name.
 */
QVariant Model::property(const QString &name) const
{
    MODEL_DATA(const Model);
    const MetaProperty property = data.metaObject.property(name);
    return (property.isValid() ? property.read(this) : data.dynamicProperties.value(name));
}

/*!
 * \brief Sets a property value by name.
 */
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

/*!
 * \brief Fills the model with data from a map (field names as keys).
 */
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

/*!
 * \brief Fills the model with data from a JSON object.
 */
void Model::fill(const QJsonObject &object)
{
    fill(object.toVariantMap());
}

/*!
 * \brief Fills the model with data from a SQL record.
 */
void Model::fill(const QSqlRecord &record)
{
    MODEL_DATA(Model);
    for (int i(0); i < record.count(); ++i) {
        const MetaProperty property = data.metaObject.property(record.fieldName(i), MetaObject::ResolveByFieldName);
        if (property.isValid())
            property.write(this, record.value(i));
    }
}

/*!
 * \brief Returns true if the model exists in the database.
 */
bool Model::exists()
{
    const Query query = newQuery(true);
    auto result = exec(QueryBuilder::selectStatement("COUNT(1)", query), query);
    if (result && result->next())
        return result->value(0).toInt() > 0;
    else
        return false;
}

/*!
 * \brief Refreshes the model data from the database.
 */
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

/*!
 * \brief Inserts the model into the database.
 */
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

/*!
 * \brief Updates the model in the database.
 */
bool Model::update()
{
    MODEL_DATA(Model);

    const QVariantMap values = data.metaObject.readFillableFields(this);

    const Query query = newQuery(true);
    auto result = exec(QueryBuilder::updateStatement(values, query), query);
    return (result ? result->numRowsAffected() > 0 : false);
}

/*!
 * \brief Deletes the model from the database.
 */
bool Model::deleteData()
{
    MODEL_DATA(Model);

    const Query query = newQuery(true);
    auto result = exec(QueryBuilder::deleteStatement(query), query);
    return (result ? result->numRowsAffected() > 0 : false);
}

/*!
 * \brief Eagerly loads a relationship.
 */
bool Model::load(const QString &relation)
{
    return load(QStringList() << relation);
}

/*!
 * \brief Eagerly loads multiple relationships.
 */
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

/*!
 * \brief Returns the last query executed by this model.
 */
Query Model::lastQuery() const
{
    MODEL_DATA(const Model);
    return data.lastQuery;
}

/*!
 * \brief Returns the last error encountered by this model.
 */
Error Model::lastError() const
{
    MODEL_DATA(const Model);
    return data.lastError;
}

/*!
 * \brief Returns the metadata object for this model.
 */
MetaObject Model::metaObject() const
{
    return data->metaObject;
}

/*!
 * \brief Returns the database connection used by this model.
 */
Connection Model::connection() const
{
    MODEL_DATA(const Model);
    return data.metaObject.connection();
}

/*!
 * \brief Converts the model to a JSON object.
 */
QJsonObject Model::toJsonObject() const
{
    return QJsonObject::fromVariantMap(data->dynamicProperties);
}

/*!
 * \brief Converts the model to a SQL record matching the table structure.
 */
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
