#ifndef QELOQUENT_MIGRATOR_H
#define QELOQUENT_MIGRATOR_H

#include <QEloquent/global.h>
#include <QEloquent/result.h>

#include <QScopedPointer>

class QDateTime;

namespace QEloquent {

class Migration;
class Error;

class MigratorStorage;
class QELOQUENT_EXPORT Migrator
{
public:
    using MigrationCallback = std::function<void()>;
    using MigrationMonitor = std::function<void(const Migration *migration, bool up)>;

    static Result<int, Error> init(const QString &connectionName);

    static Result<int, Error> migrate(const MigrationMonitor &monitor = nullptr);
    static Result<int, Error> rollback(int steps = 1, const MigrationMonitor &monitor = nullptr);
    static Result<int, Error> refresh(const MigrationMonitor &monitor = nullptr);

    template<typename MigrationClass>
    static void registerMigration() { registerMigration(new MigrationClass()); }
    static void registerMigration(const QString &name, const MigrationCallback &up, const MigrationCallback &down);
    static void registerMigration(const QString &name, const MigrationCallback &up, const MigrationCallback &down, const QString &connectionName);
    static void registerMigration(Migration *migration);

    static void clear();

private:
    static Result<int, Error> prepareMigrations();
    static Result<bool, Error> createMigrationsTable();
    static Result<int, Error> retrieveMigrationsData();
    static Result<int, Error> saveMigrationRecord(Migration *migration, bool executed);

    static void logError(const QString &text, const class SchemaException &e);

    static QScopedPointer<MigratorStorage> storage;

    friend class Migration;
};

} // namespace QEloquent

#endif // QELOQUENT_MIGRATOR_H
