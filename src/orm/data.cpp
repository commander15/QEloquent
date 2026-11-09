#include "data.h"

#include <QUuid>

namespace QEloquent {

void AbstractData::unsetValue()
{
    setValue(QVariant());
}

QVariant AbstractData::generateValue()
{
    QVariant value;

    switch (format()) {
    case DataFormat::UuidData:
        value = QUuid::createUuidV7();
        break;

    default:
        break;
    }

    if (setValue(value))
        return value;
    else
        return QVariant();
}

bool AbstractData::isDirty() const
{
    return true;
}

AbstractData::DataFormat AbstractData::formatFromType(const QMetaType &type)
{
    if (type == QMetaType::fromType<bool>())
        return DataFormat::BooleanData;

    if (type == QMetaType::fromType<int>() ||
        type == QMetaType::fromType<unsigned int>() ||
        type == QMetaType::fromType<long>() ||
        type == QMetaType::fromType<unsigned long>() ||
        type == QMetaType::fromType<long long>() ||
        type == QMetaType::fromType<unsigned long long>() ||
        type == QMetaType::fromType<short>() ||
        type == QMetaType::fromType<unsigned short>() ||
        type == QMetaType::fromType<signed char>() ||
        type == QMetaType::fromType<unsigned char>())
    {
        return DataFormat::IntegerData;
    }

    if (type == QMetaType::fromType<float>() ||
        type == QMetaType::fromType<double>())
    {
        return DataFormat::DecimalData;
    }

    if (type == QMetaType::fromType<QString>() ||
        type == QMetaType::fromType<std::string>())
    {
        return DataFormat::StringData;
    }

    if (type == QMetaType::fromType<QUuid>())
    {
        return DataFormat::UuidData;
    }

    return DataFormat::UnknownData;
}

} // namespace QEloquent
