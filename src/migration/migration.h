#ifndef QELOQUENT_MIGRATION_H
#define QELOQUENT_MIGRATION_H

#include <QEloquent/global.h>
#include <QEloquent/schema.h>

#include <QString>
#include <type_traits>

#define QELOQUENT_CONCAT_IMPL(a, b) a##b
#define QELOQUENT_CONCAT(a, b) QELOQUENT_CONCAT_IMPL(a, b)

#define QELOQUENT_MIGRATION(Class) \
    static_assert(std::is_base_of_v<QEloquent::Migration, Class>, \
                  "Class must inherit from QEloquent::Migration"); \
    namespace { \
        struct Registrator { \
            Registrator() { \
                QEloquent::_registerMigration(new Class()); \
            } \
        } QELOQUENT_CONCAT(registrator_instance_, __COUNTER__); \
    }

namespace QEloquent {

class QELOQUENT_EXPORT Migration
{
public:
    typedef std::function<void()> Callback;

    virtual ~Migration() = default;

    int id() const { return m_id; }
    virtual QString name() const = 0;
    bool isExecuted() const { return m_id; }

    virtual void up() = 0;
    virtual void down() = 0;

    virtual QString connectionName() const;
    Connection connection() const;

    static Migration *boot(const QString &connectionName);

    static Migration *create(const QString &name, const Callback &up, const Callback &down);
    static Migration *create(const QString &name, const Callback &up, const Callback &down, const QString &connectionName);

private:
    int m_id = 0;
    QString m_connectionName;

    friend class Migrator;
};

QELOQUENT_EXPORT void _registerMigration(Migration *migration);

template<typename MigrationClass>
struct MigrationRegistrator {
    MigrationRegistrator() {
        static_assert(std::is_base_of_v<Migration, MigrationClass>, "Class must inherit from QEloquent::Migration");
        _registerMigration(&instance);
    }

    static MigrationClass instance;
};

template<typename MigrationClass>
MigrationClass MigrationRegistrator<MigrationClass>::instance;

} // namespace QEloquent

#endif // QELOQUENT_MIGRATION_H
