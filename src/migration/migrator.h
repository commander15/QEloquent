#ifndef QELOQUENT_MIGRATOR_H
#define QELOQUENT_MIGRATOR_H

#include <QEloquent/global.h>
#include <QEloquent/result.h>

#include <QScopedPointer>

namespace QEloquent {

class Migration;
class Error;

class MigratorStorage;
class QELOQUENT_EXPORT Migrator
{
public:
    using MigrationCallback = std::function<void()>;
    using MigrationMonitor = std::function<void(const Migration *migration)>;

    static Result<bool, Error> init(const QString &connectionName);

    static Result<int, Error> migrate(const MigrationMonitor &monitor = nullptr);
    static Result<int, Error> rollback(int steps = 1);
    static Result<int, Error> rollback(const MigrationMonitor &monitor, int steps = 1);
    static Result<int, Error> refresh(const MigrationMonitor &monitor = nullptr);

    template<typename MigrationClass>
    static void registerMigration() { registerMigration(new MigrationClass()); }
    static void registerMigration(Migration *migration);
    static void registerMigration(const QString &name, const MigrationCallback &up, const MigrationCallback &down);
    static void registerMigration(const QString &name, const MigrationCallback &up, const MigrationCallback &down, const QString &connectionName);

    static void clear();

private:
    static Result<bool, Error> prepareMigrations();
    static Result<bool, Error> createMigrationsTable();

    static void logError(const QString &text, const class SchemaException &e);

    static QScopedPointer<MigratorStorage> storage;
};

} // namespace QEloquent

#endif // QELOQUENT_MIGRATOR_H
