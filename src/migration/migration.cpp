#include "migration.h"
#include "migration_p.h"

#include <QEloquent/connection.h>
#include <QEloquent/migrator.h>
#include <QEloquent/querybuilder.h>
#include <QEloquent/queryrunner.h>
#include <QEloquent/query.h>
#include <QEloquent/error.h>
#include <QEloquent/datamap.h>
#include <QEloquent/private/migrator_p.h>

#include <QFile>
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

Migration *Migration::create(const QString &name, const Callback &up, const Callback &down, const QDateTime &createdAt)
{
    Migration *m = new GenericMigration(name, up, down, createdAt, Connection::defaultConnectionName());
    if (s_autoRegistrationOn) Migrator::registerMigration(m);
    return m;
}

Migration *Migration::create(const QString &name, const Callback &up, const Callback &down, const QDateTime &createdAt, const QString &connectionName)
{
    Migration *m = new GenericMigration(name, up, down, createdAt, connectionName);
    if (s_autoRegistrationOn) Migrator::registerMigration(m);
    return m;
}

Migration *Migration::createTable(const QString &tableName, const Schema::BlueprintCallback &callback, const QDateTime &createdAt)
{
    return createTable(tableName, callback, createdAt, Connection::defaultConnectionName());
}

Migration *Migration::createTable(const QString &tableName, const Schema::BlueprintCallback &callback, const QDateTime &createdAt, const QString &connectionName)
{
    auto up = [tableName, callback] {
        Schema::create(tableName, callback);
    };

    auto down = [tableName] {
        Schema::dropIfExists(tableName);
    };

    return create("create_" + tableName + "_table", up, down, createdAt, connectionName);
}

Migration *Migration::fromScriptFilePattern(const QString &name, const QString &filePrefix, const QDateTime &createdAt)
{
    return fromScriptFiles(name, filePrefix + "_up.sql", filePrefix + "_down.sql", createdAt, Connection::defaultConnectionName());
}

Migration *Migration::fromScriptFilePattern(const QString &name, const QString &filePrefix, const QDateTime &createdAt, const QString &connectionName)
{
    return fromScriptFiles(name, filePrefix + "_up.sql", filePrefix + "_down.sql", createdAt, connectionName);
}

Migration *Migration::fromScriptFiles(const QString &name, const QString &upFileName, const QString &downFileName, const QDateTime &createdAt)
{
    return fromScriptFiles(name, upFileName, downFileName, createdAt, Connection::defaultConnectionName());
}

Migration *Migration::fromScriptFiles(const QString &name, const QString &upFileName, const QString &downFileName, const QDateTime &createdAt, const QString &connectionName)
{
    QByteArray up;
    QByteArray down;
    QFile file;

    file.setFileName(upFileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        up = file.readAll();
        file.close();
    }

    file.setFileName(downFileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        down = file.readAll();
        file.close();
    }

    return fromScriptContents(name, up, down, createdAt, connectionName);
}

Migration *Migration::fromScriptContents(const QString &name, const QByteArray &upScript, const QByteArray &downScript, const QDateTime &createdAt)
{
    return fromScriptContents(name, upScript, downScript, createdAt, Connection::defaultConnectionName());
}

Migration *Migration::fromScriptContents(const QString &name, const QByteArray &upScript, const QByteArray &downScript, const QDateTime &createdAt, const QString &connectionName)
{
    const Callback up = [upScript] {
        QStringList statements = QueryBuilder::statementsFromScriptContent(upScript);
        Schema::exec(statements);
    };

    const Callback down = [downScript] {
        QStringList statements = QueryBuilder::statementsFromScriptContent(downScript);
        Schema::exec(statements);
    };

    return create(name, up, down, createdAt, connectionName);
}

void Migration::enableAutoRegistration()
{
    s_autoRegistrationOn = true;
}

void Migration::disableAutoRegistration()
{
    s_autoRegistrationOn = false;
}

void Migration::updateData(int id, const QDateTime &execTime)
{
    m_id = id;
    m_executedAt = execTime;
}

void Migration::markAsExecuted()
{
    Migrator::saveMigrationRecord(this, true);
}

void Migration::markAsUnexecuted()
{
    Migrator::saveMigrationRecord(this, true);
}

void _registerMigration(Migration *migration)
{
    Migrator::registerMigration(migration);
}

bool Migration::s_autoRegistrationOn = false;

}
