#include "seeder.h"
#include "seeder_p.h"

#include <QStringList>

namespace QEloquent {

/*!
 * \class Seeder
 * \brief Base class for database seeders in QEloquent.
 *
 * Seeder defines a single unit of data seeding logic that can be registered,
 * checked for necessity, and executed. Seeders are typically used after migrations
 * to populate initial or test data (e.g., default roles, admin user, sample products).
 *
 * Concrete seeders can be created in two ways:
 * - Subclassing Seeder and overriding \c name() and \c seed()
 * - Using factory methods \c create() with lambdas for simple cases
 *
 * Seeders support:
 * - Idempotency via \c shouldSeed() (default: always run)
 * - Auto-registration (like migrations)
 * - Selective or full execution
 * - Progress monitoring via callback
 *
 * \sa DatabaseSeeder for a potential main entry point (optional).
 * \sa Migrator for schema counterpart.
 */

/*!
 * \brief Virtual destructor.
 *
 * Ensures proper cleanup of any subclass resources.
 * Default implementation.
 */
Seeder::~Seeder() = default;

/*!
 * \fn QString Seeder::name
 * \brief Returns the seeder name.
 *
 * Useful for debuging.
 */

/*!
 * \brief Checks whether this seeder needs to run.
 *
 * Default implementation always returns \c true (run unconditionally).
 *
 * Subclasses should override to implement idempotency, e.g.:
 * \code
 * bool MySeeder::shouldSeed() const override {
 *     return Role::count() == 0;
 * }
 * \endcode
 *
 * \return \c true if the seeder should execute, \c false to skip.
 */
bool Seeder::shouldSeed() const
{
    return true;
}

/*!
 * \fn void Seeder::seed()
 * \brief Performs the actual data seeding logic for this seeder.
 *
 * This pure virtual method contains the core seeding instructions — typically
 * creating default records, sample data, lookup values, or initial configuration
 * needed for the application to function correctly.
 *
 * The method is called automatically by \c run(), \c runOnly(), or \c runAll()
 * when the seeder is selected and \c shouldSeed() returns \c true.
 *
 * **Important notes:**
 * - Seeders should be written to be **idempotent** whenever possible
 *   (safe to run multiple times without creating duplicates or errors).
 *   The recommended pattern is to check for existence before inserting:
 *   \code
 *   if (Role::where("name", "Admin").doesntExist()) {
 *       Role::create({"name", "Admin"});
 *   }
 *   \endcode
 * - Use model factories or bulk inserts for large datasets.
 * - Avoid hard-coding IDs — prefer querying existing records or using relationships.
 * - Throw exceptions only for critical failures (the migrator/seeder system will catch them).
 *
 * **Simple example (one-off lambda seeder):**
 * \code
 * Seeder::create("initial_roles", [] {
 *     if (Role::count() == 0) {
 *         Role::create({{"name", "Admin"}});
 *         Role::create({{"name", "Editor"}});
 *         Role::create({{"name", "Viewer"}});
 *     }
 * });
 * \endcode
 *
 * **Full subclass example:**
 * \code
 * class CategorySeeder : public Seeder
 * {
 * public:
 *     QString name() const override { return "categories"; }
 *
 *     bool shouldSeed() const override {
 *         return Category::count() == 0;
 *     }
 *
 *     void seed() override {
 *         Category::create({{"name", "Fruits"}, {"description", "Fresh fruits"}});
 *         Category::create({{"name", "Vegetables"}, {"description", "Fresh vegetables"}});
 *         // ... more categories
 *     }
 * };
 *
 * // Register somewhere (e.g. in main or init)
 * Seeder::registerSeeder<CategorySeeder>();
 * \endcode
 *
 * After registration, run with:
 * \code
 * Seeder::runAll();           // runs everything that needs seeding
 * Seeder::run("categories");  // runs only this seeder
 * \endcode
 *
 * \note If you need to seed conditional or environment-specific data,
 *       override \c shouldSeed() or use environment variables/flags.
 *
 * \sa shouldSeed() to control whether this method should run.
 * \sa runAll(), runOnly(), run() for execution entry points.
 */

/*!
 * \brief Factory method to create a simple seeder with only a run callback.
 *
 * Convenience overload that uses no check (always runs when selected).
 * If auto-registration is enabled, the seeder is automatically added.
 *
 * \param name Unique name for this seeder (used for selective run).
 * \param run Callback that performs the seeding logic.
 * \return Pointer to the created seeder (ownership transferred).
 */
Seeder *Seeder::create(const QString &name, const RunCallback &run)
{
    return create(name, nullptr, run);
}

/*!
 * \brief Factory method to create a seeder with optional check and run callbacks.
 *
 * If auto-registration is enabled, the seeder is automatically added.
 *
 * \param name Unique name for this seeder.
 * \param check Optional callback that determines whether to run (nullptr = always run).
 * \param run Callback that performs the seeding logic.
 * \return Pointer to the created seeder (ownership transferred).
 */
Seeder *Seeder::create(const QString &name, const CheckCallback &check, const RunCallback &run)
{
    GenericSeeder *seeder = new GenericSeeder(name, check, run);
    if (internal->autoRegister)
        internal->seeders.append(seeder);
    return seeder;
}

/*!
 * \brief Manually registers an existing seeder instance.
 *
 * Adds the seeder to the internal list if not already present.
 * Ownership is transferred to the Seeder registry.
 *
 * \param seeder Pointer to the seeder instance to register.
 */
void Seeder::registerSeeder(Seeder *seeder)
{
    if (internal->seeders.contains(seeder))
        return;
    internal->seeders.append(seeder);
}

/*!
 * \brief Enables automatic registration for factory-created seeders.
 *
 * When enabled, any seeder created via \c create() is automatically
 * added to the registry.
 */
void Seeder::enableAutoRegistration()
{
    internal->autoRegister = true;
}

/*!
 * \brief Disables automatic registration for factory-created seeders.
 *
 * Factory methods will return the seeder without registering it.
 * The caller must manually call \c registerSeeder().
 */
void Seeder::disableAutoRegistration()
{
    internal->autoRegister = false;
}

/*!
 * \brief Runs a single named seeder if it should seed.
 *
 * Finds the first seeder matching \a name, checks \c shouldSeed(),
 * and executes \c seed() if needed.
 *
 * \param name Unique name of the seeder to run.
 * \param monitor Optional callback invoked before seeding.
 * \return \c true if a matching seeder was found and executed, \c false otherwise.
 */
bool Seeder::run(const QString &name, const SeedMonitor &monitor)
{
    return runOnly(QStringList() << name, monitor) > 0;
}

/*!
 * \brief Runs only the specified seeders if they should seed.
 *
 * Executes seeders in registration order, only those whose name is in \a names.
 *
 * \param names List of seeder names to execute.
 * \param monitor Optional callback invoked before each seeding.
 * \return Number of seeders that were actually executed.
 */
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

/*!
 * \brief Runs all registered seeders that need seeding.
 *
 * Iterates over all registered seeders in registration order.
 * Only executes those where \c shouldSeed() returns \c true.
 *
 * \param monitor Optional callback invoked before each seeding.
 * \return Number of seeders that were actually executed.
 */
int Seeder::runAll(const SeedMonitor &monitor)
{
    int count = 0;
    for (Seeder *seeder : std::as_const(internal->seeders)) {
        if (seeder->shouldSeed()) {
            if (monitor) monitor(seeder);
            seeder->seed();
            ++count;
        }
    }
    return count;
}

QScopedPointer<SeederPrivate> Seeder::internal(new SeederPrivate());

} // namespace QEloquent
