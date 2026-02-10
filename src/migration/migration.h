#ifndef QELOQUENT_MIGRATION_H
#define QELOQUENT_MIGRATION_H

#include <QEloquent/global.h>
#include <QEloquent/schema.h>
#include <QEloquent/result.h>

#include <QString>
#include <QDateTime>
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

class QFile;

namespace QEloquent {

class Error;

class QELOQUENT_EXPORT Migration
{
public:
    typedef std::function<void()> Callback;

    virtual ~Migration();

    int id() const;
    virtual QString name() const = 0;
    QDateTime executedAt() const;
    bool isExecuted() const;

    virtual void up() = 0;
    virtual void down() = 0;

    virtual QString connectionName() const;
    Connection connection() const;

    static Migration *create(const QString &name, const Callback &up, const Callback &down);
    static Migration *create(const QString &name, const Callback &up, const Callback &down,  const QString &connectionName);

    static Migration *createTable(const QString &tableName, const Schema::BlueprintCallback &callback);
    static Migration *createTable(const QString &tableName, const Schema::BlueprintCallback &callback, const QString &connectionName);

    static Migration *fromScriptFilePattern(const QString &name, const QString &filePrefix);
    static Migration *fromScriptFilePattern(const QString &name, const QString &filePrefix, const QString &connectionName);

    static Migration *fromScriptFiles(const QString &name, const QString &upFileName, const QString &downFileName);
    static Migration *fromScriptFiles(const QString &name, const QString &upFileName, const QString &downFileName, const QString &connectionName);

    static Migration *fromScriptContents(const QString &name, const QByteArray &upScript, const QByteArray &downScript);
    static Migration *fromScriptContents(const QString &name, const QByteArray &upScript, const QByteArray &downScript, const QString &connectionName);

    static void enableAutoRegistration();
    static void disableAutoRegistration();

private:
    int m_id = 0;
    QDateTime m_executedAt;

    void updateData(int id, const QDateTime &execTime);
    void markAsExecuted();
    void markAsUnexecuted();

    static bool s_autoRegistrationOn;

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
