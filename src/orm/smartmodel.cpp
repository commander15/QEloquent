#include "smartmodel.h"

#include "model_p.h"

#define MODEL_DATA(Class) Class##Data &data = *static_cast<Class##Data *>(Model::data.get());

namespace QEloquent {

/**
 * \class QEloquent::SmartModel
 * @brief Base model class providing automated ID management and timestamp auditing for QEloquent models.
 *
 * The `SmartModel` class extends the core `Model` architecture by introducing built-in
 * metadata properties for standard entity tracking, including a flexible typed primary key (`id`)
 * supporting integers, strings, or UUIDs, alongside automated timestamp audit fields (`createdAt`,
 * `updatedAt`, and `deletedAt`). It serves as the primary base class for domain models requiring
 * convention-over-configuration features and soft-deletion or creation life-cycle hooks.
 */

class SmartModelData : public ModelData
{
public:
    ID id;
    QDateTime createdAt;
    QDateTime updatedAt;
    QDateTime deletedAt;
};

SmartModel::SmartModel(const QMetaObject &metaObject, IDType idType)
    : Model(metaObject, new SmartModelData())
{
    MODEL_DATA(SmartModel);
    data.id = ID(QVariant(), static_cast<ID::IDType>(idType));
}

/*!
 * \brief Copy constructor.
 *
 * Constructs a new SimpleModel by copying another instance.
 * This is the default implementation (member-wise copy).
 */
SmartModel::SmartModel(const SmartModel &other)
    : Model(other)
{}

/*!
 * \brief Move constructor.
 *
 * Constructs a new SimpleModel by moving from another instance.
 * This is the default implementation (member-wise move).
 */
SmartModel::SmartModel(SmartModel &&other)
    : Model(other)
{}

SmartModel &SmartModel::operator=(const SmartModel &other)
{
    Model::operator=(other);
    return *this;
}

SmartModel &SmartModel::operator=(SmartModel &&other)
{
    Model::operator=(std::move(other));
    return *this;
}

/*!
 * \brief Destructor.
 *
 * Cleans up any resources held by the SimpleModel instance.
 * This is the default implementation.
 */
SmartModel::~SmartModel()
{}

/*!
 * \brief Returns the model id.
 *
 * \return The model's ID.
 */
ID SmartModel::id() const
{
    MODEL_DATA(const SmartModel);
    return data.id;
}

/*!
 * \brief Sets the model id.
 *
 * \param id The new ID value to set.
 */
bool SmartModel::setId(const ID &id)
{
    MODEL_DATA(SmartModel);

    if (id.type() != data.id.type())
        return false;

    data.id = id;
    return true;
}

/*!
 * \brief Returns the creation timestamp.
 *
 * This timestamp is typically set automatically by QEloquent on insert.
 *
 * \return The creation time as QDateTime (UTC).
 */
QDateTime SmartModel::createdAt() const
{
    MODEL_DATA(const SmartModel);
    return data.createdAt;
}

/*!
 * \brief Returns the last update timestamp.
 *
 * This timestamp is typically updated automatically by QEloquent on update.
 *
 * \return The last update time as QDateTime (UTC).
 */
QDateTime SmartModel::updatedAt() const
{
    MODEL_DATA(const SmartModel);
    return data.updatedAt;
}

/*!
 * \brief Returns the deletion timestamp.
 *
 * This timestamp is typically updated automatically by QEloquent on soft delete.
 *
 * \return The deletion time as QDateTime (UTC).
 */

QDateTime SmartModel::deletedAt() const
{
    MODEL_DATA(const SmartModel);
    return data.deletedAt;
}

bool SmartModel::doInsert()
{
    MODEL_DATA(SmartModel);

    if (data.id.isUuid() && data.id.isNull()) {
        data.id.generateValue(); // generate UUID
    }

    return Model::doInsert();
}

void SmartModel::setCreatedAt(const QDateTime &time)
{
    MODEL_DATA(SmartModel);
    data.createdAt = time;
}

void SmartModel::setUpdatedAt(const QDateTime &time)
{
    MODEL_DATA(SmartModel);
    data.updatedAt = time;
}

void SmartModel::setDeletedAt(const QDateTime &time)
{
    MODEL_DATA(SmartModel);
    data.deletedAt = time;
}

} // namespace QEloquent
