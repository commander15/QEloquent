#ifndef QELOQUENT_DATATYPE_H
#define QELOQUENT_DATATYPE_H

#include <QEloquent/global.h>

#include <QObject>
#include <QVariant>

namespace QEloquent {

class QELOQUENT_EXPORT AbstractData
{
    Q_GADGET
    Q_PROPERTY(QVariant value READ value WRITE setValue RESET unsetValue FINAL)
    Q_PROPERTY(AbstractData::DataFormat format READ format FINAL)

public:
    enum DataFormat: int {
        UnknownData = -1,
        BooleanData = 0,
        IntegerData = 1,
        DecimalData = 2,
        StringData = 3,
        UuidData = 4
    };
    Q_ENUM(DataFormat)

    virtual ~AbstractData() = default;

    virtual QVariant value() const = 0;
    virtual bool setValue(const QVariant &value) = 0;
    virtual void unsetValue();

    virtual QVariant generateValue();

    virtual bool isDirty() const;
    virtual AbstractData::DataFormat format() const = 0;

    template<typename T>
    T to() const { return value().value<T>(); }

    template<typename T>
    operator T() const { return value().value<T>(); }

    static DataFormat formatFromType(const QMetaType &type);
};

template<typename T>
class Data : public AbstractData
{
public:
    Data(DataFormat format = UnknownData)
        : data(new Internal())
    {
        initFormat(format);
    }

    Data(const T &val, DataFormat format = UnknownData)
        : data(new Internal())
    {
        initFormat(format);
        setValue(val);
    }

    Data(const Data &other) = default;
    Data(Data &&other) noexcept = default;

    Data& operator=(const Data &other) = default;
    Data& operator=(Data &&other) noexcept = default;

    Data& operator=(const T &val) {
        setValue(val);
        return *this;
    }

    bool operator==(const Data &other) const {
        if (data->null != other.data->null)
            return false;
        if (data->null)
            return true;
        return data->value == other.data->value;
    }

    bool operator==(const T &val) const {
        if (data->null)
            return false;
        return data->value == val;
    }

    bool operator!=(const Data &other) const {
        return !(*this == other);
    }

    bool operator!=(const T &val) const {
        return !(*this == val);
    }

    bool operator<(const Data &other) const {
        if (data->null || other.isNull())
            return data->null && !other.isNull();
        return data->value < other.data->value;
    }

    bool isNull() const { return data->null; }

    void clear() { unsetValue(); }

    QVariant value() const override {
        if (data->null)
            return QVariant();
        return QVariant::fromValue(data->value);
    }

    T get() const {
        return data->null ? T() : data->value;
    }

    bool setValue(const QVariant &val) override {
        if (val.isNull()) {
            unsetValue();
            return true;
        }
        if (!val.canConvert<T>())
            return false;

        setValue(val.value<T>());
        return true;
    }

    void setValue(const T &val) {
        data->value = val;
        data->null = false;
    }

    void unsetValue() override {
        data->value = T();
        data->null = true;
    }

    bool isDirty() const override {
        return data->dirty;
    }

    AbstractData::DataFormat format() const override {
        return data->format;
    }

    void setFormat(AbstractData::DataFormat f) {
        data->format = f;
    }

    // Dereference and conversion operators
    const T& operator*() const { return data->value; }
    const T* operator->() const { return &data->value; }

    operator T() const { return get(); }

private:
    void initFormat(AbstractData::DataFormat f = UnknownData) {
        if (f == UnknownData)
            data->format = formatFromType(QMetaType::fromType<T>());
        else
            data->format = f;
    }

    class Internal : public QSharedData
    {
    public:
        T value = T();
        bool null = true;
        bool dirty = true;
        DataFormat format = AbstractData::DataFormat::UnknownData;
    };

    QSharedDataPointer<Internal> data;
};

} // namespace QEloquent

#endif // QELOQUENT_DATATYPE_H
