#ifndef QELOQUENT_SEEDER_H
#define QELOQUENT_SEEDER_H

#include <QEloquent/global.h>
#include <QEloquent/result.h>
#include <QEloquent/error.h>

#include <QScopedPointer>

namespace QEloquent {

class SeederPrivate;
class QELOQUENT_EXPORT Seeder
{
public:
    typedef std::function<bool()> CheckCallback;
    typedef std::function<void()> RunCallback;
    typedef std::function<void(Seeder *seeder)> SeedMonitor;

    virtual ~Seeder() = default;

    virtual QString name() const = 0;

    virtual bool shouldSeed() const { return true; }
    virtual void seed() = 0;

    static Seeder *create(const QString &name, const RunCallback &run);
    static Seeder *create(const QString &name, const CheckCallback &check, const RunCallback &run);

    template<typename SeederClass>
    static Seeder *registerSeeder() {
        Seeder *seeder = new SeederClass();
        registerSeeder(seeder);
        return seeder;
    }
    static void registerSeeder(Seeder *seeder);

    static void enableAutoRegistration();
    static void disableAutoRegistration();

    static bool run(const QString &name, const SeedMonitor &monitor = nullptr);
    static int runOnly(const QStringList &names, const SeedMonitor &monitor = nullptr);
    static int runAll(const SeedMonitor &monitor = nullptr);

private:
    static QScopedPointer<SeederPrivate> internal;
};

} // namespace QEloquent

#endif // QELOQUENT_SEEDER_H
