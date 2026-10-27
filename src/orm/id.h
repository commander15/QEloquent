#ifndef QELOQUENT_ID_H
#define QELOQUENT_ID_H

#include <QEloquent/global.h>
#include <QEloquent/datatype.h>

#include <QObject>
#include <QUuid>
#include <QVariant>

namespace QEloquent {

class QELOQUENT_EXPORT ID : public DataType
{
    Q_GADGET

public:
    enum IDType {
        IntegerID,
        StringID,
        UuidID
    };

    ID();
    ID(const QVariant &value, IDType type);
    ID(const ID &other);

    ID &operator=(const ID &other);
    ID &operator=(ID &&other);
    ID &operator=(qint64 v) { setValue(v); return *this; }
    ID &operator=(const QString &v) { setValue(v); return *this; }
    ID &operator=(const QUuid &v) { setValue(v); return *this; }

    bool operator==(const ID &other) const;
    inline bool operator!=(const ID &other) const { return !operator==(other); }

    bool isInteger() const;
    qint64 integer() const;

    bool isString() const;
    QString string() const;

    bool isUuid() const;
    QUuid uuid() const;

    bool isNull() const;
    IDType type() const;

    bool setValue(qint64 id);
    bool setValue(const QString &id);
    bool setValue(const QUuid &id);
    bool setValue(const QVariant &id) override;

    DataFormat format() const override;

    operator qint64() const { return integer(); }
    operator QString() const { return string(); }
    operator QUuid() const { return uuid(); }

    static ID make(qint64 id) { return ID(id, IntegerID); }
    static ID make(const QString &id) { return ID(id, (QUuid::fromString(id).isNull() ? StringID : UuidID)); }
    static ID make(const QString &id, bool uuid) { return ID(id, (uuid ? UuidID : StringID)); }

private:
    IDType m_type;
};

} // namespace QEloquent

Q_DECLARE_METATYPE(QEloquent::ID)

bool operator==(qint64 n, const QEloquent::ID &o)
{ return (o.isInteger() ? n == o.integer() : false); }

bool operator!=(qint64 n, const QEloquent::ID &o)
{ return !operator==(n, o); }

bool operator==(const QString &str, const QEloquent::ID &o)
{ return (o.isString() ? str == o.string() : false); }

bool operator!=(const QString &str, const QEloquent::ID &o)
{ return !operator==(str, o); }

bool operator==(const QUuid &uuid, const QEloquent::ID &o)
{ return (o.isUuid() ? uuid == o.uuid() : false); }

bool operator!=(const QUuid &uuid, const QEloquent::ID &o)
{ return !operator==(uuid, o); }

#endif // QELOQUENT_ID_H