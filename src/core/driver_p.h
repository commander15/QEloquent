#ifndef QELOQUENT_DRIVER_P_H
#define QELOQUENT_DRIVER_P_H

#include "driver.h"

#include <QString>

namespace QEloquent {

class DefaultDriver final : public Driver
{
public:
    DefaultDriver(QSqlDriver *qtDriver) : Driver(qtDriver) {}

    QString primaryKeyType(bool) const override
    { return QStringLiteral("INTEGER"); }

    QString autoIncrementKeyword() const override
    { return QString(); }

    QString timestampDefault() const override
    { return QString(); }

    bool supportsForeignKeys() const override
    { return true; }

    QString foreignKeyConstraint(const QString &column, const QString &refTable, const QString &refColumn) const override
    { return QStringLiteral("FOREIGN KEY(%1) REFERENCES %2(%3)").arg(column, refTable, refColumn); }
};

class SQLiteDriver final : public Driver
{
public:
    SQLiteDriver(QSqlDriver *qtDriver) : Driver(qtDriver) {}

    QString primaryKeyType(bool) const override
    { return QStringLiteral("INTEGER"); }

    QString autoIncrementKeyword() const override
    { return QStringLiteral("AUTOINCREMENT"); }

    QString timestampDefault() const override
    { return QStringLiteral("CURRENT_TIMESTAMP"); }

    bool supportsForeignKeys() const override
    { return true; }

    QString foreignKeyConstraint(const QString &column, const QString &refTable, const QString &refColumn) const override
    { return QStringLiteral("FOREIGN KEY(%1) REFERENCES %2(%3)").arg(column, refTable, refColumn); }
};

class MySQLDriver final : public Driver
{
public:
    MySQLDriver(QSqlDriver *qtDriver) : Driver(qtDriver) {}

    QString primaryKeyType(bool) const override
    { return QStringLiteral("BIGINT"); }

    QString autoIncrementKeyword() const override
    { return QStringLiteral("AUTO_INCREMENT"); }

    QString timestampDefault() const override
    { return QStringLiteral("NOW()"); }

    bool supportsForeignKeys() const override
    { return true; }

    QString foreignKeyConstraint(const QString &column, const QString &refTable, const QString &refColumn) const override
    { return QStringLiteral("CONSTRAINT fk_%1_%2_%3 FOREIGN KEY(%1) REFERENCES %2(%3)").arg(column, refTable, refColumn); }
};

}

#endif // QELOQUENT_DRIVER_P_H
