#ifndef QELOQUENT_DATATYPE_H
#define QELOQUENT_DATATYPE_H

#include <QEloquent/global.h>

#include <QObject>
#include <QVariant>
#include <QDateTime>

#define QELOQUENT_DATA(Type) \
public: \
    using Data<Type>::Data; \
    using Data<Type>::operator=; \
    using Data<Type>::operator<=>; \
    using Data<Type>::operator==;

namespace QEloquent {

class Connection;

class QELOQUENT_EXPORT AbstractData
{
    Q_GADGET
    Q_PROPERTY(QVariant value READ value WRITE setValue RESET clear FINAL)

public:
    virtual ~AbstractData() = default;

    virtual QVariant value() const = 0;
    virtual bool setValue(const QVariant &value) = 0;
    virtual void clear();

    bool generate(const QString &connection);
    virtual QVariant generateValue(const QString &connection) const;

    virtual bool isNull() const;
    virtual bool isDirty() const;

    virtual QMetaType metaType() const = 0;

    template<typename T>
    T get() const { return value().value<T>(); }

    template<typename T>
    const T &to() const { return static_cast<const T &>(*this); }

    template<typename T>
    T &to() { return static_cast<T &>(*this); }

    static bool inside(const QVariant &var);
    static const AbstractData *fromVariant(const QVariant &var);
    static AbstractData *fromVariant(QVariant &var);
};

template<typename T>
class Data : public AbstractData
{
public:
    Data() : data(new Internal()) {}
    Data(const T &val) : data(new Internal()) { data->value = val; }

    Data(const Data &other) = default;
    Data(Data &&other) noexcept = default;

    Data& operator=(const Data &other) = default;
    Data& operator=(Data &&other) noexcept = default;
    Data& operator=(const T &val) { setValue(val); return *this; }

    using OrderingType = std::strong_ordering;

    OrderingType operator<=>(const Data &other) const {
        // 1. Compare null states first (nulls are considered "less than" non-nulls)
        if (this->data->null != other.data->null) {
            return this->data->null ? OrderingType::less : OrderingType::greater;
        }
        if (this->data->null) {
            return OrderingType::equivalent; // Both are null
        }

        // 2. Compare underlying values
        auto result = operator<=>(other.data->value);
        if (result != 0)
            return result;

        // 3. Fallback to dirty state comparison if values are equal
        return this->data->dirty <=> other.data->dirty;
    }

    OrderingType operator<=>(const T &val) const {
        if (this->data->null) {
            return OrderingType::less; // Null is less than any raw value
        }

        if (data->value < val) return OrderingType::less;
        if (val < data->value) return OrderingType::greater;
        else return OrderingType::equivalent;
    }

    bool operator==(const Data &other) const {
        return operator<=>(other) == OrderingType::equivalent;
    }

    bool operator==(const T &val) const {
        return operator<=>(val) == OrderingType::equivalent;
    }

    QVariant value() const override
    { return (data->null ? QVariant() : QVariant::fromValue<T>(data->value)); }

    bool setValue(const QVariant &val) override {
        if (val.isNull()) {
            clear();
            return true;
        }

        if (!val.canConvert<T>()) {
            clear();
            return false;
        }

        data->value = val.value<T>();
        data->null = false;
        data->dirty = true;
        return true;
    }

    void clear() override {
        data->value = T();
        data->null = true;
        data->dirty = true;
    }

    bool isNull() const override { return data->null; }
    bool isDirty() const override { return data->dirty; }

    QMetaType metaType() const override { return QMetaType::fromType<T>(); }

    const T &get() const { return data->value; }
    T &get() { return data->value; }
    void set(const T &val) { data->value = val; }
    void reset() { clear(); }

    // Dereference and conversion operators
    const T& operator*() const { return data->value; }
    const T* operator->() const { return &data->value; }

    operator const T &() const { return data->value; }
    operator T &() { return data->value; }

protected:
    class Internal : public QSharedData
    {
    public:
        T value = T();
        bool null = true;
        bool dirty = false;
    };

    QSharedDataPointer<Internal> data;
};

class QELOQUENT_EXPORT Uuid : public Data<QString>
{
    QELOQUENT_DATA(QString)

public:
    bool setValue(const QVariant &value) override;
    QVariant generateValue(const QString &) const override;

    static QString generateUuid(int version = -1);
};

class QELOQUENT_EXPORT Timestamp : public Data<QDateTime>
{
    QELOQUENT_DATA(QDateTime)

public:
    QVariant generateValue(const QString &connection) const override;
};

} // namespace QEloquent

#endif // QELOQUENT_DATATYPE_H
