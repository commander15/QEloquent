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

/*!
 * \brief Constructs a Model instance from a QMetaObject.
 *
 * Initializes the model with metadata derived from the given Qt meta-object.
 * This constructor is typically used when creating model instances from Qt's
 * introspection system (e.g., during meta-object registration or dynamic creation).
 *
 * \param metaObject The QMetaObject describing the model's properties and metadata.
 */
Model::Model(const QMetaObject &metaObject)
    : data(new ModelData())
{
    data->metaObject = MetaObject::fromQtMetaObject(metaObject);
}

/*!
 * \internal
 * \brief Constructs a Model instance from an existing ModelData pointer.
 *
 * Takes ownership of the provided ModelData pointer. This constructor is
 * intended for internal use or when reusing pre-initialized model data.
 *
 * \param data Pointer to an already initialized ModelData object.
 *             The Model takes ownership of this pointer.
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
 * \return Returns the label field value
 */
QVariant Model::label() const
{
    const MetaProperty property = data->metaObject.labelProperty();
    return (property.isValid() ? property.read(this) : QVariant());
}

/*!
 * \brief Returns a property value by name.
 */
QVariant Model::property(const QString &name) const
{
    MODEL_DATA(const Model);
    const MetaProperty property = data.metaObject.property(name, MetaObject::ResolveByPropertyName);
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
 * \brief Returns a field value by name.
 */
QVariant Model::field(const QString &name) const
{
    MODEL_DATA(const Model);

    // First we check if it's known
    const MetaProperty property = data.metaObject.property(name, MetaObject::ResolveByFieldName);
    if (property.isValid()) return property.read(this);

    // Fallback to dynamic properties
    const QString prop = data.metaObject.namingConvention()->propertyName(name, data.metaObject.tableName());
    return data.dynamicProperties.value(prop);
}

/*!
 * \brief Set field value by name.
 */
void Model::setField(const QString &name, const QVariant &value)
{
    MODEL_DATA(Model);
    const MetaProperty property = data.metaObject.property(name, MetaObject::ResolveByFieldName);
    if (property.isValid()) {
        property.write(this, value);
        return;
    }

    // Fallback to dynamic properties
    const QString prop = data.metaObject.namingConvention()->propertyName(name, data.metaObject.tableName());
    data.dynamicProperties.insert(prop, value);
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
            load(data.metaObject.relations());
            return true;
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

    const MetaProperty timestamp = data.metaObject.creationTimestamp();
    if (timestamp.isValid()) timestamp.write(this, now());

    const DataMap values = data.metaObject.read(this,
        (timestamp.hasAttribute(MetaProperty::DatabaseField) ? MetaProperty::CreationTimestamp : MetaProperty::NoAttibutes) | MetaProperty::FillableProperty,
        MetaObject::StandardProperties | MetaObject::DynamicProperties,
        MetaObject::ResolveByFieldName);

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

    const MetaProperty timestamp = data.metaObject.updateTimestamp();
    if (timestamp.isValid()) timestamp.write(this, now());

    const DataMap values = data.metaObject.read(this,
        (timestamp.hasAttribute(MetaProperty::DatabaseField) ? MetaProperty::CreationTimestamp : MetaProperty::NoAttibutes) | MetaProperty::FillableProperty,
        MetaObject::StandardProperties | MetaObject::DynamicProperties,
        MetaObject::ResolveByFieldName);

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

    // If soft delete is enabled (deletion timestamp available), we just update it value
    const MetaProperty timestamp = data.metaObject.deletionTimestamp();
    if (timestamp.isValid() && timestamp.hasAttribute(MetaProperty::DatabaseField)) {
        const QDateTime now = this->now();
        timestamp.write(this, now);

        const DataMap values = { { timestamp.fieldName(), now } };
        auto r = exec([&values](const Query &query) {
            return QueryBuilder::updateStatement(values, query);
        }, true);
        return (r ? r->numRowsAffected() == 1 : false);
    }

    // Regular deletion
    return deletePermanently();
}

/*!
 * \brief Deletes the model record from DB.
 * \note This method delete the record and by-pass soft delete.
 */
bool Model::deletePermanently()
{
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

        // We must make sure that the relation is initialized
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

QString Model::serializationContext() const
{
    const QString className = data->metaObject.className();

    const QString id = primary().toString();
    const QString label = this->label().toString();
    if (id.isEmpty() && label.isEmpty())
        return className;

    QStringList context;
    if (!id.isEmpty()) context.append('#' + id);
    if (!label.isEmpty()) context.append(label);

    return className + '(' + context.join(", ") + ')';
}

bool Model::isListSerializable() const
{
    return false;
}

QList<DataMap> Model::serialize() const
{
    QList<MetaProperty> properties = data->metaObject.properties(
        MetaProperty::PrimaryProperty | MetaProperty::LabelProperty | MetaProperty::FillableProperty |
        MetaProperty::CreationTimestamp | MetaProperty::UpdateTimestamp | MetaProperty::DeletionTimestamp,
        MetaObject::AllProperties);

    // We remove hidden properties
    properties.removeIf([](const MetaProperty &property) {
        return property.hasAttribute(MetaProperty::HiddenProperty);
    });

    return { data->metaObject.read(this, properties, MetaObject::ResolveByFieldName) };
}

void Model::deserialize(const QList<DataMap> &data, bool all)
{
    if (data.isEmpty()) return;

    static const MetaProperty::PropertyAttributes allAttributes =
        MetaProperty::PrimaryProperty | MetaProperty::FillableProperty |
        MetaProperty::CreationTimestamp | MetaProperty::UpdateTimestamp | MetaProperty::DeletionTimestamp;

    static const MetaProperty::PropertyAttributes fillableAttributes = MetaProperty::FillableProperty;

    const MetaProperty::PropertyAttributes allowedAttributes = (all ? allAttributes : fillableAttributes);
    const QList<MetaProperty> properties = this->data->metaObject.properties(allowedAttributes, MetaObject::StandardProperties | MetaObject::DynamicProperties);
    const DataMap map = data.constFirst();

    for (const MetaProperty &property : properties) {
        const QString fieldName = property.fieldName();
        if (map.contains(fieldName))
            property.write(this, map.value(fieldName));
    }
}

/*!
 * \brief Returns the full data map containing all fields (including hidden ones).
 */
DataMap Model::fullDataMap() const
{
    DataMap map = data->metaObject.read(this,
        MetaProperty::PrimaryProperty | MetaProperty::FillableProperty | MetaProperty::HiddenProperty |
        MetaProperty::CreationTimestamp | MetaProperty::UpdateTimestamp | MetaProperty::DeletionTimestamp,
        MetaObject::StandardProperties | MetaObject::AppendedProperties | MetaObject::RelationProperties,
        MetaObject::ResolveByFieldName);

    map.insert(data->dynamicProperties);
    return map;
}

/*!
 * \brief Returns the current timestamp in UTC.
 * \note This method always return a vallid value, if DB fetch fail, it returns local time converted in UTC.
 */
QDateTime Model::now() const
{
    auto result = connection().now();
    return (result ? result.value() : QDateTime::currentDateTimeUtc());
}

/*!
 * \brief Build a Query set up for this model.
 * \param statementGenerator The callback used to generated the raw query.
 * \param filter Wheter or not, filtering for 'this' model linked record must be added to the query.
 */
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

/*!
 * \brief Execute the statement provided by statementGenerator, filter parameter enable filtering or not to current record.
 */
Result<QSqlQuery, QSqlError> Model::exec(const std::function<QString (const Query &)> &statementGenerator, bool filter)
{
    data->lastQuery = newQuery(statementGenerator, filter);
    auto result = QueryRunner::exec(data->lastQuery.rawSql(), data->lastQuery.connection());
    if (!result)
        data->lastError = Error::fromSqlError(result.error());
    return result;
}

/*!
 * \brief Copy constructor.
 *
 * Constructs a new SimpleModel by copying another instance.
 * This is the default implementation (member-wise copy).
 */
SimpleModel::SimpleModel(const SimpleModel &) = default;

/*!
 * \brief Move constructor.
 *
 * Constructs a new SimpleModel by moving from another instance.
 * This is the default implementation (member-wise move).
 */
SimpleModel::SimpleModel(SimpleModel &&) = default;

/*!
 * \brief Copy assignment operator.
 *
 * Assigns the contents of another SimpleModel to this instance.
 * This is the default implementation (member-wise copy assignment).
 *
 * \return A reference to this object.
 */
SimpleModel &SimpleModel::operator=(const SimpleModel &) = default;

/*!
 * \brief Move assignment operator.
 *
 * Moves the contents of another SimpleModel into this instance.
 * This is the default implementation (member-wise move assignment).
 *
 * \return A reference to this object.
 */
SimpleModel &SimpleModel::operator=(SimpleModel &&) = default;

/*!
 * \brief Destructor.
 *
 * Cleans up any resources held by the SimpleModel instance.
 * This is the default implementation.
 */
SimpleModel::~SimpleModel() = default;

/*!
 * \brief Returns the primary key identifier.
 *
 * \return The model's ID value (qint64).
 */
qint64 SimpleModel::id() const
{
    return m_id;
}

/*!
 * \brief Sets the primary key identifier.
 *
 * If the provided \a id is negative, it is clamped to 0.
 *
 * \param id The new ID value to set.
 */
void SimpleModel::setId(qint64 id)
{
    m_id = (id < 0 ? 0 : id);
}

/*!
 * \brief Returns the primary key identifier.
 *
 * \return The model's ID value (qint64).
 */
qint64 StandardModel::id() const
{
    return m_id;
}

/*!
 * \brief Sets the primary key identifier.
 *
 * If the provided \a id is negative, it is clamped to 0.
 *
 * \param id The new ID value to set.
 */
void StandardModel::setId(qint64 id)
{
    m_id = (id < 0 ? 0 : id);
}

/*!
 * \brief Returns the creation timestamp.
 *
 * This timestamp is typically set automatically by the database on insert.
 *
 * \return The creation time as QDateTime (UTC).
 */
QDateTime StandardModel::createdAt() const
{
    return m_createdAt;
}

/*!
 * \brief Returns the last update timestamp.
 *
 * This timestamp is typically updated automatically by the database on update.
 *
 * \return The last update time as QDateTime (UTC).
 */
QDateTime StandardModel::updatedAt() const
{
    return m_updatedAt;
}

/*!
 * \brief Copy constructor.
 *
 * Constructs a new StandardModel by copying another instance.
 * This is the default implementation (member-wise copy).
 */
StandardModel::StandardModel(const StandardModel &) = default;

/*!
 * \brief Move constructor.
 *
 * Constructs a new StandardModel by moving from another instance.
 * This is the default implementation (member-wise move).
 */
StandardModel::StandardModel(StandardModel &&) = default;

/*!
 * \brief Copy assignment operator.
 *
 * Assigns the contents of another StandardModel to this instance.
 * This is the default implementation (member-wise copy assignment).
 *
 * \return A reference to this object.
 */
StandardModel &StandardModel::operator=(const StandardModel &) = default;

/*!
 * \brief Move assignment operator.
 *
 * Moves the contents of another StandardModel into this instance.
 * This is the default implementation (member-wise move assignment).
 *
 * \return A reference to this object.
 */
StandardModel &StandardModel::operator=(StandardModel &&) = default;

/*!
 * \brief Destructor.
 *
 * Destroys this StandardModel instance.
 * This is the default implementation.
 */
StandardModel::~StandardModel() = default;

} // namespace QEloquent
