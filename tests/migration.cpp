#include "migration.h"

#include <QEloquent/migrator.h>
#include <QEloquent/migration.h>
#include <QEloquent/schema.h>

using Migrator = QEloquent::Migrator;
using Schema = QEloquent::Schema;
using Blueprint = QEloquent::TableBlueprint;

void Migration::SetUp() {
    MyTest::SetUp();
    Migrator::clear();
}

TEST_F(Migration, InvalidMigrationMakesMigrateToFail) {
    auto userUp = [] {
        Schema::drop("users");
    };

    auto userDown = [] {
        Schema::create("users", [](Blueprint &table) {
            table.id();
            table.string("name", 30);
        });
    };

    auto init = Migrator::init(connection.name());
    ASSERT_TRUE(init);

    QEloquent::Migration *user = QEloquent::Migration::create("create_users_table", userUp, userDown);
    Migrator::registerMigration(user);

    auto result = Migrator::migrate();
    ASSERT_FALSE(result) << (result ? "" : TEST_STR(result.error().text()));
}

TEST_F(Migration, ValidMigrationMakesMigrateToSucceed) {
    auto userUp = [] {
        Schema::create("users", [](Blueprint &table) {
            table.id();
            table.string("name", 30);
        });
    };

    auto userDown = [] {
        Schema::drop("users");
    };

    auto init = Migrator::init(connection.name());
    ASSERT_TRUE(init);

    QEloquent::Migration *user = QEloquent::Migration::create("create_users_table", userUp, userDown);
    Migrator::registerMigration(user);

    auto result = Migrator::migrate();
    ASSERT_TRUE(result) << (result ? "" : TEST_STR(result.error().text()));
}
