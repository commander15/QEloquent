#ifndef QELOQUENT_DATATYPE_H
#define QELOQUENT_DATATYPE_H

#include <QEloquent/global.h>

#include <QObject>
#include <QVariant>

namespace QEloquent {

class QELOQUENT_EXPORT AbstractDataType
{
    Q_GADGET
    Q_PROPERTY(QVariant value READ value WRITE setValue FINAL)
    Q_PROPERTY(AbstractDataType::DataFormat format READ format FINAL)

public:
    enum class DataFormat: int {
        UnknownData = -1,
        BooleanData = 0,
        IntegerData = 1,
        DecimalData = 2,
        StringData = 3,
        UuidData = 4
    };
    Q_ENUM(DataFormat)

    virtual ~AbstractDataType() = default;

    virtual QVariant value() const = 0;
    virtual bool setValue(const QVariant &value) = 0;
    virtual void unsetValue();

    virtual AbstractDataType::DataFormat format() const = 0;
};

} // namespace QEloquent

#endif // QELOQUENT_DATATYPE_H
