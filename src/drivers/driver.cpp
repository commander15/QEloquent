#include "driver.h"
#include "driver_p.h"

#include "impl/sqlitedriver_p.h"

#include <QSqlDriver>
#include <QSqlIndex>
#include <QSqlRecord>
#include <QSqlField>

namespace QEloquent {

Driver::Driver(QSqlDriver *qtDriver)
    : d_ptr(new DriverPrivate(qtDriver))
{}

Driver::~Driver() = default;

QString Driver::currentTimestamp() const
{
    return QStringLiteral("CURRENT_TIMESTAMP");
}

QString Driver::escapeTableName(const QString &table) const
{
    return d_ptr->sql->escapeIdentifier(table, QSqlDriver::TableName);
}

QString Driver::escapeFieldName(const QString &field) const
{
    return d_ptr->sql->escapeIdentifier(field, QSqlDriver::FieldName);
}

QString Driver::escapeIdentifier(const QString &identifier) const
{
    if (identifier.contains('.')) {
        QStringList items = identifier.split('.', Qt::SkipEmptyParts);
        switch (items.size()) {
        case 0:
            return QString();

        case 1:
            return escapeFieldName(identifier);

        default:
            items[0] = escapeTableName(items.at(0));
            for (int i(1); i < items.length(); ++i)
                items[i] = escapeFieldName(items.at(i));
            return items.join('.');
        }
    } else {
        return escapeFieldName(identifier);
    }
}

QString Driver::formatValue(const QVariant &value) const
{
    QSqlField field(QString(), value.metaType());
    field.setValue(value);
    return d_ptr->sql->formatValue(field, false);
}

QStringList Driver::tableNames() const
{
    return d_ptr->sql->tables(QSql::Tables);
}

QSqlRecord Driver::record(const QString &table) const
{
    return d_ptr->sql->record(table);
}

QSqlDriver *Driver::qtDriver() const
{
    return d_ptr->sql;
}

Driver *Driver::create(const QString &qtDriverName, QSqlDriver *qtDriver)
{
    if (qtDriverName == QStringLiteral("QSQLITE"))
        return new SQLiteDriver(qtDriver);

    return new DefaultDriver(qtDriver);
}

} // namespace QEloquent
