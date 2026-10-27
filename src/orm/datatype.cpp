#include "datatype.h"

namespace QEloquent {

QVariant DataType::value() const
{
    return m_value;
}

bool DataType::setValue(const QVariant &value)
{
    m_value = value;
    return true;
}

void DataType::unsetValue()
{
    m_value.clear();
}

DataType::DataFormat DataType::format() const
{
    return DataFormat::StringData;
}

} // namespace QEloquent
