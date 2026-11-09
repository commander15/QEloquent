#include "data.h"

#include <QEloquent/connection.h>

#include <QUuid>

namespace QEloquent {

void AbstractData::clear()
{
    setValue(QVariant());
}

bool AbstractData::generate(const QString &connection)
{
    QVariant value = generateValue(connection);
    return setValue(value);
}

QVariant AbstractData::generateValue(const QString &connection) const
{
    Q_UNUSED(connection)
    return QVariant();
}

bool AbstractData::isNull() const
{
    return value().isNull();
}

bool AbstractData::isDirty() const
{
    return true;
}

bool AbstractData::inside(const QVariant &var)
{
    const QMetaObject *meta = var.metaType().metaObject();

    if (meta == nullptr)
        return false;

    if (!meta->inherits(&AbstractData::staticMetaObject))
        return false;

    return true;
}

const AbstractData *AbstractData::fromVariant(const QVariant &var)
{
    if (!inside(var))
        return nullptr;

    return static_cast<const AbstractData *>(var.constData());
}

AbstractData *AbstractData::fromVariant(QVariant &var)
{
    if (!inside(var))
        return nullptr;

    return static_cast<AbstractData *>(var.data());
}

bool Uuid::setValue(const QVariant &value)
{
    QUuid uuid = QUuid::fromString(value.toString());
    if (uuid.isNull()) {
        clear();
        return false;
    }

    return Data::setValue(value);
}

QVariant Uuid::generateValue(const QString &) const
{
    return generateUuid(7);
}

QString Uuid::generateUuid(int version)
{
    QUuid uuid;

    switch (version) {
    case 3:
        uuid = QUuid::createUuidV3(QUuid(), QByteArray());
        break;

    case 5:
        uuid = QUuid::createUuidV5(QUuid(), QByteArray());
        break;

    case 7:
        uuid = QUuid::createUuidV7();
        break;

    default:
        uuid = QUuid::createUuid();
        break;
    }

    return uuid.toString(QUuid::WithoutBraces);
}

QVariant Timestamp::generateValue(const QString &connection) const
{
    Connection con = Connection::connection(connection);
    return con.now()
        .transform([](const QDateTime &t) { return QVariant(t); })
        .value_or(QVariant());
}

} // namespace QEloquent
