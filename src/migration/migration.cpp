#include "migration.h"
#include "migration_p.h"

#include <QEloquent/connection.h>
#include <QEloquent/migrator.h>
#include <QEloquent/queryrunner.h>
#include <QEloquent/query.h>
#include <QEloquent/error.h>
#include <QEloquent/datamap.h>
#include <QEloquent/private/migrator_p.h>

#include <QSqlQuery>

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

Migration *Migration::create(const QString &name, const Callback &up, const Callback &down)
{
    return new GenericMigration(name, up, down, Connection::defaultConnectionName());
}

Migration *Migration::create(const QString &name, const Callback &up, const Callback &down, const QString &connectionName)
{
    return new GenericMigration(name, up, down, connectionName);
}

Result<bool, Error> Migration::getStatus(bool cached)
{
    if (cached && m_id > 0)
        return true;

    const QStringList fields = { "id," "executed_at" };
    Query query = Query().table(MIGRATIONS_TABLE);

    auto result = QueryRunner::select(fields, query.where("name", name()));
    if (result) {
        if (result->next()) {
            m_id = result->value(0).toInt();
            m_executedAt = result->value(1).toDateTime();
            return true;
        } else {
            return failWith(Error(Error::NotFoundError, "Not found"));
        }
    } else {
        return failWith(Error(Error::DatabaseError, QString(), result.error()));
    }
}

Result<bool, Error> Migration::markAsExecuted()
{
    DataMap data = {
        { "executed_at", connection().now() }
    };

    Query query = Query().table(MIGRATIONS_TABLE).connection(connectionName());

    // We just update if already exists
    if (m_id > 0) {
        auto result = QueryRunner::update(data, query.where("id", m_id));
        if (result) return true;
        else return failWith(Error(Error::DatabaseError, QString(), result.error()));
    } else {
        // We insert if not already exists
        data.insert("name", name());
        auto result = QueryRunner::insert(data, query);
        if (result) return true;
        else return failWith(Error(Error::DatabaseError, QString(), result.error()));
    }
}

Result<bool, Error> Migration::markAsUnexecuted()
{
    Query query = Query().table(MIGRATIONS_TABLE).connection(connectionName());

    // We delete if already exists
    if (m_id > 0) {
        auto result = QueryRunner::deleteData(query.where("id", m_id));
        if (!result) return failWith(Error(Error::DatabaseError, QString(), result.error()));
    }

    m_id = 0;
    m_executedAt = QDateTime();
    return true;
}

void _registerMigration(Migration *migration)
{
    Migrator::registerMigration(migration);
}

}
