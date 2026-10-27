#include "id.h"

namespace QEloquent {

ID::ID()
    : m_type(IntegerID)
{
}

ID::ID(const QVariant &value, IDType type)
    : DataType(value)
    , m_type(IntegerID)
{
}

ID::ID(const ID &other) = default;

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
    qSwap(m_value, other.m_value);
    return *this;
}

bool ID::isInteger() const
{
    return m_value.canConvert<qint64>();
}

qint64 ID::integer() const
{
    return m_type == IntegerID;
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

ID::IDType ID::type() const
{
    return m_type;
}

bool ID::setValue(qint64 id)
{
    if (m_type != IntegerID) return false;

    DataType::setValue(id);
    return true;
}

bool ID::setValue(const QString &id)
{
    if (m_type == IntegerID) return false;

    if (m_type == UuidID)
        return setValue(QUuid::fromString(id));

    DataType::setValue(id);
    return true;
}

bool ID::setValue(const QUuid &id)
{
    if (m_type != UuidID) return false;

    DataType::setValue(id.toString(QUuid::WithoutBraces));
    return true;
}

bool ID::setValue(const QVariant &id)
{
    switch (m_type) {
    case IntegerID:
        return setValue(id.value<qint64>());

    case StringID:
    case UuidID:
        return setValue(id.toString());
    }

    return false;
}

DataType::DataFormat ID::format() const
{
    switch (m_type) {
    case IntegerID:
        return DataFormat::IntegerData;

    case StringID:
        return DataFormat::StringData;

    case UuidID:
        return DataFormat::UuidData;
    }

    return DataType::format();
}

} // namespace QEloquent