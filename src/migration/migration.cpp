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

Migration *Migration::create(const QString &name, const Callback &up, const Callback &down)
{
    Migration *m = new GenericMigration(name, up, down, Connection::defaultConnectionName());
    if (s_autoRegistrationOn) Migrator::registerMigration(m);
    return m;
}

Migration *Migration::create(const QString &name, const Callback &up, const Callback &down, const QString &connectionName)
{
    Migration *m = new GenericMigration(name, up, down, connectionName);
    if (s_autoRegistrationOn) Migrator::registerMigration(m);
    return m;
}

Migration *Migration::createTable(const QString &tableName, const Schema::BlueprintCallback &callback)
{
    return createTable(tableName, callback, Connection::defaultConnectionName());
}

Migration *Migration::createTable(const QString &tableName, const Schema::BlueprintCallback &callback, const QString &connectionName)
{
    auto up = [tableName, callback] {
        Schema::create(tableName, callback);
    };

    auto down = [tableName] {
        Schema::dropIfExists(tableName);
    };

    return create("create_" + tableName + "_table", up, down, connectionName);
}

Migration *Migration::fromScriptFilePattern(const QString &name, const QString &filePrefix)
{
    return fromScriptFiles(name, filePrefix + "_up.sql", filePrefix + "_down.sql", Connection::defaultConnectionName());
}

Migration *Migration::fromScriptFilePattern(const QString &name, const QString &filePrefix, const QString &connectionName)
{
    return fromScriptFiles(name, filePrefix + "_up.sql", filePrefix + "_down.sql", connectionName);
}

Migration *Migration::fromScriptFiles(const QString &name, const QString &upFileName, const QString &downFileName)
{
    return fromScriptFiles(name, upFileName, downFileName, Connection::defaultConnectionName());
}

Migration *Migration::fromScriptFiles(const QString &name, const QString &upFileName, const QString &downFileName, const QString &connectionName)
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

    return fromScriptContents(name, up, down, connectionName);
}

Migration *Migration::fromScriptContents(const QString &name, const QByteArray &upScript, const QByteArray &downScript)
{
    return fromScriptContents(name, upScript, downScript, Connection::defaultConnectionName());
}

Migration *Migration::fromScriptContents(const QString &name, const QByteArray &upScript, const QByteArray &downScript, const QString &connectionName)
{
    const Callback up = [upScript] {
        QStringList statements = QueryBuilder::statementsFromScriptContent(upScript);
        Schema::exec(statements);
    };

    const Callback down = [downScript] {
        QStringList statements = QueryBuilder::statementsFromScriptContent(downScript);
        Schema::exec(statements);
    };

    return create(name, up, down, connectionName);
}

void Migration::enableAutoRegistration()
{
    s_autoRegistrationOn = true;
}

void Migration::disableAutoRegistration()
{
    s_autoRegistrationOn = false;
}

Result<bool, Error> Migration::getStatus(bool cached)
{
    if (cached && m_id > 0)
        return true;

    const QStringList fields = { "id", "executed_at" };
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

void Migration::updateData(int id, const QDateTime &execTime)
{
    m_id = id;
    m_executedAt = execTime;
}

Result<bool, Error> Migration::markAsExecuted()
{
    QDateTime now;
    {
        auto result = connection().now();
        if (result)
            now = result.value();
        else
            now = QDateTime::currentDateTimeUtc();
    }

    DataMap data = {
        { "executed_at", now }
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
    Query query = Query().table(MIGRATIONS_TABLE);

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

bool Migration::s_autoRegistrationOn = false;

}
