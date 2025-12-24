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
    static Result<bool, Error> init(const QString &connectionName);

    static Result<int, Error> migrate();
    static Result<int, Error> rollback(int steps = 1);
    static Result<int, Error> refresh();

    template<typename MigrationClass>
    static void registerMigration() { registerMigration(new MigrationClass()); }
    static void registerMigration(Migration *migration);

private:
    enum Event { Unknown, Executed, Unexecuted };
    static bool updateMigration(Migration *migration, Event event);

    static void logError(const QString &text, const class SchemaException &e);

    static QScopedPointer<MigratorStorage> storage;
};

} // namespace QEloquent

#endif // QELOQUENT_MIGRATOR_H
