#include "common.h"

#include <QEloquent/migration.h>
#include <QEloquent/migrator.h>
#include <QEloquent/queryrunner.h>

#include <QSqlQuery>

using namespace QEloquent;

QEloquent::Result<int> migrate()
{
    Migration::enableAutoRegistration();

    Migration::createTable("user_roles", [](TableBlueprint &table) {
        table.id();
        table.string("name", 30).unique();
        table.timestamps();
    }, QDateTime(QDate(2026, 1, 15), QTime(11, 56)));

    Migration::createTable("users", [](TableBlueprint &table) {
        table.id();
        table.string("name", 30);
        table.string("email", 60).unique();
        table.string("password", 64);
        table.foreignId("role_id").references("id").on("user_roles");
        table.timestamps();
    }, QDateTime(QDate(2026, 1, 15), QTime(11, 56)));

    Migration::createTable("categories", [](TableBlueprint &table) {
        table.id();
        table.string("name", 30);
        table.string("description").nullable();
        table.timestamps();
    }, QDateTime(QDate(2026, 1, 15), QTime(11, 56)));

    Migration::createTable("products", [](TableBlueprint &table) {
        table.id();
        table.string("name", 30);
        table.string("description").nullable();
        table.decimal("price").min(0.0);
        table.string("barcode", 20).nullable();
        table.foreignId("category_id").on("categories").references("id");
        table.timestamps();
    }, QDateTime(QDate(2026, 1, 15), QTime(11, 56)));

    Migration::createTable("stocks", [](TableBlueprint &table) {
        table.id();
        table.string("label", 30).nullable();
        table.unsignedInteger("quantity").min(0);
        table.foreignId("product_id").on("products").references("id");
        table.timestamps("created_at", "updated_at");
    }, QDateTime(QDate(2026, 1, 15), QTime(11, 56)));

    Migration::createTable("sales", [](TableBlueprint &table) {
        table.id();
        table.unsignedBigInteger("number").min(0);
        table.decimal("amount").min(0.0);
        table.foreignId("seller_id").references("id").on("sales");
        table.timestamps();
    }, QDateTime(QDate(2026, 1, 15), QTime(11, 56)));

    Migration::createTable("sale_items", [](TableBlueprint &table) {
        table.id();
        table.decimal("unit_price").min(0.0);
        table.unsignedInteger("quantity").min(0);
        table.foreignId("sale_id").references("id").on("sales");
        table.foreignId("product_id").references("id").on("products");
    }, QDateTime(QDate(2026, 1, 15), QTime(11, 56)));

    Migration::createTable("versions", [](TableBlueprint &table) {
        table.id();
        table.string("name", 20).unique();
        table.timestamp("installed_at");
    }, QDateTime(QDate(2026, 1, 15), QTime(11, 56)));

    Migration::disableAutoRegistration();

    qDebug().nospace().noquote() << "Running migrations...";
    return Migrator::migrate([](const Migration *migration, bool) {
        qDebug().noquote().nospace() << "running " << migration->name() << "...";
    });
}

void rollbackMigrations()
{
    qDebug().nospace().noquote() << "Rolling back...";
    Migrator::rollback(10, [](const Migration *migration, bool) {
        qDebug().noquote().nospace() << "reverting " << migration->name() << "...";
    });
}
