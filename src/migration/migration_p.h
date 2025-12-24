#ifndef QELOQUENT_MIGRATION_P_H
#define QELOQUENT_MIGRATION_P_H

#include "migration.h"

namespace QEloquent {

class GenericMigration final : public Migration
{
public:
    GenericMigration(const QString &name,
                     const Migration::Callback &up, const Migration::Callback &down,
                     const QString &connectionName)
        : m_name(name), m_up(up), m_down(down), m_connectionName(connectionName) {}

    QString name() const override { return m_name; }
    void up() override { if (m_up) m_up(); }
    void down() override { if (m_down) m_down(); }

    QString connectionName() const override { return m_connectionName; }

private:
    const QString m_name;
    const std::function<void()> m_up;
    const std::function<void()> m_down;
    const QString m_connectionName;
};

class BootMigration final : public Migration
{
public:
    BootMigration(const QString &connectionName)
        : m_connectionName(connectionName) {}

    QString name() const override
    { return QStringLiteral("create_migrations_table"); }

    void up() override
    {
        Schema::create("migrations", [](TableBlueprint &table) {
            table.id();
            table.string("name").length(60);
            table.timestamp("timestamp");
        });
    }

    void down() override
    {
        Schema::dropIfExists("migrations");
    }

    QString connectionName() const override
    { return m_connectionName; }

private:
    const QString m_connectionName;
};

}

#endif // QELOQUENT_MIGRATION_P_H
