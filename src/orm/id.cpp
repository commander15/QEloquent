#include "id.h"

namespace QEloquent {

/**
 * \class QEloquent::ID
 * @brief Represents a strongly-typed primary key identifier for QEloquent models.
 *
 * The `ID` class encapsulates model identifiers supporting multiple underlying data formats
 * (such as 64-bit integers, standard strings, and UUIDs). It provides type-safe accessors,
 * assignment routines, and validation logic to ensure keys map cleanly across different
 * database drivers and storage strategies.
 */

/**
 * @brief Constructs a default ID instance initialized as an IntegerID.
 */
ID::ID()
    : m_type(IntegerID)
{
}

/**
 * @brief Constructs an ID instance with a specified variant value and type.
 * @param value The underlying variant value.
 * @param type The ID type classification.
 */
ID::ID(const QVariant &value, IDType type)
    : m_value(value)
    , m_type(IntegerID)
{
}

/**
 * @brief Constructs a copy of an existing ID instance.
 * @param other The ID instance to copy.
 */
ID::ID(const ID &other) = default;

/**
 * @brief Compares two ID instances for equality.
 * @param other The other ID instance to compare against.
 * @return true if both IDs share the same type and equivalent variant values, false otherwise.
 */
bool ID::operator==(const ID &other) const
{
    if (m_type != other.m_type)
        return false;

    return QVariant::compare(m_value, other.m_value) == QPartialOrdering::Equivalent;
}

/**
 * @brief Copy assignment operator.
 * @param other The ID instance to copy from.
 * @return ID& A reference to this updated instance.
 */
ID &ID::operator=(const ID &other)
{
    if (this == &other)
        return *this;

    m_value = other.m_value;
    m_type = other.m_type;
    return *this;
}

/**
 * @brief Move assignment operator.
 * @param other The ID instance to move from.
 * @return ID& A reference to this updated instance.
 */
ID &ID::operator=(ID &&other)
{
    if (this == &other)
        return *this;

    m_value.swap(other.m_value);
    qSwap(m_type, other.m_type);
    return *this;
}

/**
 * @brief Checks whether the ID is of integer type.
 * @return true if the ID type is IntegerID, false otherwise.
 */
bool ID::isInteger() const
{
    return m_type == IntegerID;
}

/**
 * @brief Retrieves the ID value as a 64-bit integer.
 * @return qint64 The integer ID value, or 0 if it is not an integer type or is null.
 */
qint64 ID::integer() const
{
    if (m_type != IntegerID || m_value.isNull())
        return 0;
    else
        return m_value.value<qint64>();
}

/**
 * @brief Checks whether the ID is represented as a string or a UUID.
 * @return true if the type is StringID or UuidID, false otherwise.
 */
bool ID::isString() const
{
    return m_type == StringID || m_type == UuidID;
}

/**
 * @brief Retrieves the ID value as a string.
 * @return QString The string representation of the ID value.
 */
QString ID::string() const
{
    return m_value.toString();
}

/**
 * @brief Checks whether the ID is of UUID type.
 * @return true if the ID type is UuidID, false otherwise.
 */
bool ID::isUuid() const
{
    return m_type == UuidID;
}

/**
 * @brief Retrieves the ID value as a parsed QUuid object.
 * @return QUuid The UUID representation of the ID.
 */
QUuid ID::uuid() const
{
    return QUuid::fromString(m_value.toString());
}

/**
 * @brief Checks whether the underlying ID value is null.
 * @return true if the variant value is null, false otherwise.
 */
bool ID::isNull() const
{
    return m_value.isNull();
}

/**
 * @brief Retrieves the raw underlying QVariant value.
 * @return QVariant The stored variant.
 */
QVariant ID::value() const
{
    return m_value;
}

/**
 * @brief Retrieves the specific ID type classification.
 * @return IDType The type enum value.
 */
ID::IDType ID::type() const
{
    return m_type;
}

/**
 * @brief Sets the ID value from a 64-bit integer.
 * @param id The 64-bit integer value.
 * @return true if successfully updated, false if the ID type is not IntegerID.
 */
bool ID::setValue(qint64 id)
{
    if (m_type != IntegerID) return false;

    m_value = id;
    return true;
}

/**
 * @brief Sets the ID value from a string.
 * @param id The string value.
 * @return true if successfully set and valid (validating UUID format if applicable), false otherwise.
 */
bool ID::setValue(const QString &id)
{
    if (m_type == IntegerID) return false;

    if (m_type == UuidID) {
        const QUuid uuid = QUuid::fromString(id);
        if (uuid.isNull()) return false;
    }

    m_value = id;
    return true;
}

/**
 * @brief Sets the ID value from a QUuid object.
 * @param id The QUuid object.
 * @return true if successfully set, false if type is not UuidID or if the UUID is null.
 */
bool ID::setValue(const QUuid &id)
{
    if (m_type != UuidID) return false;

    if (id.isNull()) {
        m_value = QVariant();
        return false;
    }

    m_value = id.toString(QUuid::WithoutBraces);
    return true;
}

/**
 * @brief Sets the ID value from a generic QVariant.
 * @param id The QVariant value to assign.
 * @return true if successfully parsed and assigned according to the ID type, false otherwise.
 */
bool ID::setValue(const QVariant &id)
{
    if (id.isNull()) {
        m_value = id;
        return true;
    }

    switch (m_type) {
    case IntegerID:
        return setValue(id.value<qint64>());

    case StringID:
    case UuidID:
        return setValue(id.toString());
    }

    return false;
}

/**
 * @brief Retrieves the abstract data format mapping corresponding to the ID type.
 * @return AbstractDataType::DataFormat The matching data format enum.
 */
AbstractData::DataFormat ID::format() const
{
    switch (m_type) {
    case IntegerID:
        return DataFormat::IntegerData;

    case StringID:
        return DataFormat::StringData;

    case UuidID:
        return DataFormat::UuidData;

    default:
        return DataFormat::UnknownData;
    }

    return AbstractData::format();
}

} // namespace QEloquent