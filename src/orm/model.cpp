#include "model.h"
#include "model_p.h"

#include <QEloquent/metaproperty.h>
#include <QEloquent/querybuilder.h>

#include <QVariant>
#include <QDateTime>
#include <QJsonObject>
#include <QSqlRecord>

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

QVariant Model::field(const QString &name) const
{
    MODEL_DATA(const Model);
    const MetaProperty property = data.metaObject.property(name, MetaObject::ResolveByFieldName);
    return (property.isValid() ? property.read(this) : QVariant());
}

void Model::setField(const QString &name, const QVariant &value)
{
    MODEL_DATA(Model);
    const MetaProperty property = data.metaObject.property(name, MetaObject::ResolveByFieldName);
    if (property.isValid())
        property.write(this, value);
}

/*!
 * \brief Fills the model with data from a map (field names as keys).
 */
void Model::fill(const QVariantMap &values)
{
    fill(QJsonObject::fromVariantMap(values));
}

/*!
 * \brief Fills the model with data from a JSON object.
 */
void Model::fill(const QJsonObject &object)
{
    MODEL_DATA(Model);
    const QStringList fields = object.keys();
    for (const QString &field : fields) {
        const MetaProperty property = data.metaObject.property(field, MetaObject::ResolveByFieldName);
        if (!property.isValid())
            continue;

        switch (property.propertyType()) {
        case MetaProperty::StandardProperty:
            property.write(this, object.value(field).toVariant());
            break;

        case MetaProperty::AppendedProperty:
            break;

        case MetaProperty::RelationProperty:
            if (data.relationData.contains(property.propertyName()))
                data.relationData.value(property.propertyName());
            break;
        }
    }
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
    auto result = exec([](const Query &query) {
        return QueryBuilder::selectStatement("COUNT(1)", query);
    }, true);

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

    auto result = exec([](const Query &query) {
        return QueryBuilder::selectStatement(query);
    }, true);

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

    if (data.metaObject.hasCreationTimestamp())
        data.metaObject.creationTimestamp().write(this, QDateTime::currentDateTime());

    const QVariantMap values = data.metaObject.readFillableFields(this);
    auto result = exec([&values](const Query &query) {
        return QueryBuilder::insertStatement(values, query);
    }, false);

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

    if (data.metaObject.hasUpdateTimestamp())
        data.metaObject.updateTimestamp().write(this, QDateTime::currentDateTime());

    const QVariantMap values = data.metaObject.readFillableFields(this);
    auto result = exec([&values](const Query &query) {
        return QueryBuilder::updateStatement(values, query);
    }, true);

    return (result ? result->numRowsAffected() > 0 : false);
}

/*!
 * \brief Deletes the model from the database.
 */
bool Model::deleteData()
{
    MODEL_DATA(Model);

    if (data.metaObject.hasDeletionTimestamp()) {
        data.metaObject.deletionTimestamp().write(this, QDateTime::currentDateTime());
        return update();
    }

    auto result = exec([](const Query &query) {
        return QueryBuilder::deleteStatement(query);
    }, true);
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
            property.read(this); // We just read to init the relation
        }

        if (!data->relationData.contains(relation))
            continue;

        auto r = data->relationData.value(relation);
        r->parent = this; // We make sure that the relation is linked to 'this' instance
        if (!r->get()) return false;
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
    QJsonObject object;

    // static properties
    auto p = data->metaObject.properties();
    for (const MetaProperty &property : std::as_const(p)) {
        const QString field = property.fieldName();

        switch (property.propertyType()) {
        case MetaProperty::StandardProperty:
        case MetaProperty::AppendedProperty:
            if (!property.hasAttribute(MetaProperty::HiddenProperty))
                object.insert(field, QJsonValue::fromVariant(property.read(this)));
            break;

        default:
            break;
        }
    }

    // Relations
    auto r = data->relationData.keys();
    for (const QString &relation : std::as_const(r))
        object.insert(relation, data->relationData.value(relation)->extract());

    // Dynamic
    auto d = data->dynamicProperties.keys();
    for (const QString &property : std::as_const(d))
        object.insert(property, QJsonValue::fromVariant(data->dynamicProperties.value(property)));

    return object;
}

/*!
 * \brief Converts the model to a SQL record matching the table structure.
 */
QSqlRecord Model::toSqlRecord() const
{
    return QSqlRecord();
}

Query Model::newQuery(const std::function<QString (const Query &)> &statementGenerator, bool filter = true) const
{
    Query query;

    if (filter) {
        const MetaProperty primaryProperty = data->metaObject.primaryProperty();
        const QVariant primary = primaryProperty.read(this);
        query.where(primaryProperty.fieldName(), primary);
    }

    return query
        .table(data->metaObject.tableName())
        .connection(data->metaObject.connectionName())
        .raw(statementGenerator(query));
}

Result<QSqlQuery, QSqlError> Model::exec(const std::function<QString (const Query &)> &statementGenerator, bool filter)
{
    data->lastQuery = newQuery(statementGenerator, filter);
    auto result = QueryRunner::exec(data->lastQuery.rawSql(), data->lastQuery.connection());
    if (!result)
        data->lastError = Error::fromSqlError(result.error());
    return result;
}

} // namespace QEloquent
