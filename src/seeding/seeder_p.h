#ifndef QELOQUENT_SEEDER_P_H
#define QELOQUENT_SEEDER_P_H

#include "seeder.h"

#include <QList>

namespace QEloquent {

class SeederPrivate
{
public:
    ~SeederPrivate() { clear(); }

    void clear() {
        while (!seeders.isEmpty())
            delete seeders.takeLast();
    }

    bool autoRegister = false;
    QList<Seeder *> seeders;
};

class GenericSeeder final : public Seeder
{
public:
    GenericSeeder(const QString &name, const CheckCallback &check, const RunCallback &run)
        : m_name(name), m_check(check), m_run(run) {}

    QString name() const override { return m_name; }
    bool shouldSeed() const override { return (m_check ? m_check() : Seeder::shouldSeed()); }
    void seed() override { return m_run(); }

private:
    const QString m_name;
    const CheckCallback m_check;
    const RunCallback m_run;
};

}

#endif // QELOQUENT_SEEDER_P_H
