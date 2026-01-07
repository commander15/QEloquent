#include "driver.h"
#include "driver_p.h"

#include <QSqlIndex>
#include <QSqlRecord>
#include <QSqlField>

namespace QEloquent {

Driver::Driver(QSqlDriver *qtDriver)
    : m_driver(qtDriver)
{}

QString Driver::columnType(FieldType baseType, int length, int precision) const
{
    switch (baseType) {
    case FieldType::PrimaryKey:
        return "BIGINT";

    case FieldType::TinyInteger:
        return "TINYINT";

    case FieldType::SmallInteger:
        return "SMALLINT";

    case FieldType::Integer:
        return "INTEGER";

    case FieldType::BigInteger:
        return "BIGINT";

    case FieldType::Float:
        return "FLOAT";

    case FieldType::Double:
        return "DOUBLE";

    case FieldType::Decimal:
        return QStringLiteral("DECIMAL(%1, %2)").arg(length).arg(precision);

    case FieldType::Boolean:
        return "BOOLEAN";

    case FieldType::Char:
        return QStringLiteral("CHAR(%1)").arg(length);

    case FieldType::String:
        return QStringLiteral("VARCHAR(%1)").arg(length);

    case FieldType::Text:
        return "TEXT";

    case FieldType::MediumText:
        return "MEDIUMTEXT";

    case FieldType::LongText:
        return "LONGTEXT";

    case FieldType::Date:
        return "DATE";

    case FieldType::Time:
        return "TIME";

    case FieldType::DateTime:
        return "DATETIME";

    case FieldType::Timestamp:
        return "TIMESTAMP";

    case FieldType::Binary:
        return "BLOB";

    case FieldType::Json:
        return "JSON";

    case Raw:
        QString();
    }

    return QString();
}

bool Driver::isOpen() const
{
    return m_driver->isOpen();
}

bool Driver::beginTransaction()
{
    return m_driver->beginTransaction();
}

bool Driver::commitTransaction()
{
    return m_driver->commitTransaction();
}

bool Driver::rollbackTransaction()
{
    return m_driver->rollbackTransaction();
}

QStringList Driver::tables(QSql::TableType tableType) const
{
    return m_driver->tables(tableType);
}

QSqlIndex Driver::primaryIndex(const QString &tableName) const
{
    return m_driver->primaryIndex(tableName);
}

QSqlRecord Driver::record(const QString &tableName) const
{
    return m_driver->record(tableName);
}

QString Driver::formatValue(const QSqlField &field, bool trimStrings) const
{
    return m_driver->formatValue(field, trimStrings);
}

QString Driver::escapeIdentifier(const QString &identifier, IdentifierType type) const
{
    return m_driver->escapeIdentifier(identifier, type);
}

QString Driver::sqlStatement(StatementType type,
                             const QString &tableName,
                             const QSqlRecord &rec,
                             bool preparedStatement) const
{
    return m_driver->sqlStatement(type, tableName, rec, preparedStatement);
}

QVariant Driver::handle() const
{
    return m_driver->handle();
}

bool Driver::hasFeature(DriverFeature f) const
{
    return m_driver->hasFeature(f);
}

void Driver::close()
{
    m_driver->close();
}

QSqlResult *Driver::createResult() const
{
    return m_driver->createResult();
}

bool Driver::open(const QString &db,
                  const QString &user,
                  const QString &password,
                  const QString &host,
                  int port,
                  const QString &connOpts)
{
    return m_driver->open(db, user, password, host, port, connOpts);
}

bool Driver::subscribeToNotification(const QString &name)
{
    return m_driver->subscribeToNotification(name);
}

bool Driver::unsubscribeFromNotification(const QString &name)
{
    return m_driver->unsubscribeFromNotification(name);
}

QStringList Driver::subscribedToNotifications() const
{
    return m_driver->subscribedToNotifications();
}

bool Driver::isIdentifierEscaped(const QString &identifier, IdentifierType type) const
{
    return m_driver->isIdentifierEscaped(identifier, type);
}

QString Driver::stripDelimiters(const QString &identifier, IdentifierType type) const
{
    return m_driver->stripDelimiters(identifier, type);
}

int Driver::maximumIdentifierLength(IdentifierType type) const
{
    return m_driver->maximumIdentifierLength(type);
}

Driver *Driver::create(const QString &qtDriverName, QSqlDriver *qtDriver)
{
    if (qtDriverName == QStringLiteral("QSQLITE"))
        return new SQLiteDriver(qtDriver);

    if (qtDriverName == QStringLiteral("QMYSQL"))
        return new MySQLDriver(qtDriver);

    return new DefaultDriver(qtDriver);
}

bool Driver::cancelQuery()
{
    return m_driver->cancelQuery();
}

} // namespace QEloquent
