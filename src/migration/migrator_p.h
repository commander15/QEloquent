#ifndef QELOQUENT_MIGRATOR_P_H
#define QELOQUENT_MIGRATOR_P_H

#include "migrator.h"

#include <QEloquent/migration.h>

#include <QList>

#define MIGRATIONS_TABLE "migrations"

namespace QEloquent {

class MigratorStorage {
public:
    ~MigratorStorage()
    { clearMigrations(); }

    void add(Migration *migration)
    { if (!migrations.contains(migration)) migrations.append(migration); }

    void removeMigration(Migration *migration)
    { migrations.removeOne(migration); }

    void clearMigrations()
    { for (Migration *m : std::as_const(migrations)) delete m; migrations.clear(); }

    QList<Migration *> migrations;
};

}

#endif // QELOQUENT_MIGRATOR_P_H
