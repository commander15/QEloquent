#include "id.h"

namespace QEloquent {

ID::ID()
    : m_type(IntegerID)
{
}

ID::ID(const QVariant &value, IDType type)
    : m_value(value)
    , m_type(IntegerID)
{
}

ID::ID(const ID &other) = default;

bool ID::operator==(const ID &other) const
{
    if (m_type != other.m_type)
        return false;

    return QVariant::compare(m_value, other.m_value) == QPartialOrdering::Equivalent;
}

ID &ID::operator=(const ID &other)
{
    if (this == &other)
        return *this;

    m_value = other.m_value;
    m_type = other.m_type;
    return *this;
}

ID &ID::operator=(ID &&other)
{
    if (this == &other)
        return *this;

    m_value.swap(other.m_value);
    qSwap(m_type, other.m_type);
    return *this;
}

bool ID::isInteger() const
{
    return m_type == IntegerID;
}

qint64 ID::integer() const
{
    if (m_type != IntegerID || m_value.isNull())
        return 0;
    else
        return m_value.value<qint64>();
}

bool ID::isString() const
{
    return m_type == StringID || m_type == UuidID;
}

QString ID::string() const
{
    return m_value.toString();
}

bool ID::isUuid() const
{
    return m_type == UuidID;
}

QUuid ID::uuid() const
{
    return QUuid::fromString(m_value.toString());
}

bool ID::isNull() const
{
    return m_value.isNull();
}

QVariant ID::value() const
{
    return m_value;
}

ID::IDType ID::type() const
{
    return m_type;
}

bool ID::setValue(qint64 id)
{
    if (m_type != IntegerID) return false;

    m_value = id;
    return true;
}

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

AbstractDataType::DataFormat ID::format() const
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

    return AbstractDataType::format();
}

} // namespace QEloquent