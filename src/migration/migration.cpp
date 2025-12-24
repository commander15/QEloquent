#include "migration.h"
#include "migration_p.h"

#include <QEloquent/connection.h>
#include <QEloquent/migrator.h>
#include <QEloquent/private/migrator_p.h>

#include <QList>

namespace QEloquent {

QString Migration::connectionName() const
{
    return Connection::defaultConnectionName();
}

Connection Migration::connection() const
{
    return Connection::connection(connectionName());
}

Migration *Migration::boot(const QString &connectionName)
{
    return new BootMigration(connectionName);
}

Migration *Migration::create(const QString &name, const Callback &up, const Callback &down)
{
    return create(name, up, down, Connection::defaultConnectionName());
}

Migration *Migration::create(const QString &name, const Callback &up, const Callback &down, const QString &connectionName)
{
    return new GenericMigration(name, up, down, connectionName);
}

void _registerMigration(Migration *migration)
{
    Migrator::registerMigration(migration);
}

}
