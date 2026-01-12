#ifndef QELOQUENT_DRIVER_H
#define QELOQUENT_DRIVER_H

#include <QEloquent/global.h>

#include <QScopedPointer>

class QSqlRecord;
class QSqlDriver;

namespace QEloquent {

class DriverPrivate;
class QELOQUENT_EXPORT Driver
{
public:
    Driver(QSqlDriver *qtDriver);
    virtual ~Driver();

    virtual QString currentTimestamp() const;

    QString escapeTableName(const QString &table) const;
    QString escapeFieldName(const QString &field) const;
    QString escapeIdentifier(const QString &identifier) const;
    QString formatValue(const QVariant &value) const;

    QStringList tableNames() const;
    QSqlRecord record(const QString &table) const;

#ifdef QELOQUENT_MIGRATIONS_SUPPORT
    virtual class SchemaGrammar *schemaGrammar() const = 0;
#endif

    QSqlDriver *qtDriver() const;

    static Driver *create(const QString &qtDriverName, QSqlDriver *qtDriver);

protected:
    QScopedPointer<DriverPrivate> d_ptr;
};

} // namespace QEloquent

#endif // QELOQUENT_DRIVER_H
