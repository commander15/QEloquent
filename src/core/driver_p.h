#ifndef QELOQUENT_DRIVER_P_H
#define QELOQUENT_DRIVER_P_H

#include "driver.h"

#include <QString>

namespace QEloquent {

class DefaultDriver final : public Driver
{
public:
    DefaultDriver(QSqlDriver *qtDriver) : Driver(qtDriver) {}

    FieldType primaryKeyType(bool *) const override
    { return Integer; }

    QString autoIncrementKeyword() const override
    { return QString(); }

    QString timestampDefault() const override
    { return QString(); }
};

class SQLiteDriver final : public Driver
{
public:
    SQLiteDriver(QSqlDriver *qtDriver) : Driver(qtDriver) {}

    FieldType primaryKeyType(bool *) const override
    { return BigInteger; }

    QString autoIncrementKeyword() const override
    { return QStringLiteral("AUTOINCREMENT"); }

    QString columnType(FieldType baseType, int, int) const override
    {
        switch (baseType) {
        case Driver::PrimaryKey:
        case Driver::Boolean:
        case Driver::Integer:
        case Driver::BigInteger:
        case Driver::SmallInteger:
        case Driver::TinyInteger:
            return QStringLiteral("INTEGER");

        case Driver::Double:
        case Driver::Float:
            return QStringLiteral("REAL");

        case Driver::Decimal:
            return QStringLiteral("NUMERIC");

        case Driver::Char:
        case Driver::String:
        case Driver::Text:
        case Driver::MediumText:
        case Driver::LongText:
        case Driver::Date:
        case Driver::Time:
        case Driver::DateTime:
        case Driver::Timestamp:
        case Driver::Json:
            return QStringLiteral("TEXT");

        case Driver::Binary:
            return QStringLiteral("BLOB");

        case Driver::Raw:
            return QString();
        }

        return QString();
    }

    QString timestampDefault() const override
    { return QStringLiteral("CURRENT_TIMESTAMP"); }
};

class MySQLDriver final : public Driver
{
public:
    MySQLDriver(QSqlDriver *qtDriver) : Driver(qtDriver) {}

    FieldType primaryKeyType(bool *postive) const override
    { *postive = false; return BigInteger; }

    QString autoIncrementKeyword() const override
    { return QStringLiteral("AUTO_INCREMENT"); }

    QString timestampDefault() const override
    { return QStringLiteral("NOW()"); }
};

}

#endif // QELOQUENT_DRIVER_P_H
