#include "seeder.h"
#include "seeder_p.h"

#include <QStringList>

namespace QEloquent {

Seeder *Seeder::create(const QString &name, const RunCallback &run)
{
    return create(name, nullptr, run);
}

Seeder *Seeder::create(const QString &name, const CheckCallback &check, const RunCallback &run)
{
    GenericSeeder *seeder = new GenericSeeder(name, check, run);
    if (internal->autoRegister)
        internal->seeders.append(seeder);
    return seeder;
}

void Seeder::registerSeeder(Seeder *seeder)
{
    if (internal->seeders.contains(seeder))
        return;

    internal->seeders.append(seeder);
}

void Seeder::enableAutoRegistration()
{
    internal->autoRegister = true;
}

void Seeder::disableAutoRegistration()
{
    internal->autoRegister = false;
}

bool Seeder::run(const QString &name, const SeedMonitor &monitor)
{
    return runOnly(QStringList() << name, monitor) > 0;
}

int Seeder::runOnly(const QStringList &names, const SeedMonitor &monitor)
{
    int count = 0;

    for (Seeder *seeder : std::as_const(internal->seeders)) {
        if (names.contains(seeder->name())) {
            if (seeder->shouldSeed()) {
                if (monitor) monitor(seeder);
                seeder->seed();
                ++count;
            }
        }
    }

    return count;
}

int Seeder::runAll(const SeedMonitor &monitor)
{
    int count = 0;

    for (Seeder *seeder : std::as_const(internal->seeders)) {
        if (seeder->shouldSeed()) {
            if (monitor) monitor(seeder);
            seeder->seed();
            ++ count;
        }
    }

    return count;
}

QScopedPointer<SeederPrivate> Seeder::internal(new SeederPrivate());

} // namespace QEloquent
