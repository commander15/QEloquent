#ifndef QELOQUENT_MIGRATOR_P_H
#define QELOQUENT_MIGRATOR_P_H

#include "migrator.h"

#include <QEloquent/migration.h>

#include <QList>

namespace QEloquent {

class MigratorStorage {
public:
    ~MigratorStorage()
    { for (Migration *m : std::as_const(migrations)) delete m; }

    void add(Migration *migration)
    { if (!migrations.contains(migration)) migrations.append(migration); }

    QList<Migration *> migrations;
};

}

#endif // QELOQUENT_MIGRATOR_P_H
