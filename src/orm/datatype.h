#ifndef QELOQUENT_DATATYPE_H
#define QELOQUENT_DATATYPE_H

#include <QEloquent/global.h>

#include <QObject>
#include <QVariant>

namespace QEloquent {

class QELOQUENT_EXPORT DataType
{
    Q_GADGET
    Q_PROPERTY(QVariant value READ value WRITE setValue FINAL)
    Q_PROPERTY(DataType::DataFormat format READ format FINAL)

public:
    enum class DataFormat: int {
        BooleanData = 0,
        IntegerData = 1,
        DecimalData = 2,
        StringData = 3,
        UuidData = 4
    };
    Q_ENUM(DataFormat)

    DataType();
    DataType(const QVariant &value);
    virtual ~DataType() = default;

    virtual QVariant value() const;
    virtual bool setValue(const QVariant &value);
    virtual void unsetValue();

    virtual DataType::DataFormat format() const;

protected:
    QVariant m_value;
};

} // namespace QEloquent

Q_DECLARE_METATYPE(QEloquent::DataType)

#endif // QELOQUENT_DATATYPE_H
