#include <QApplication>
#include <QEloquent/connection.h>
#include <QEloquent/queryrunner.h>
#include "ui/mainwindow.h"
#include "models/product.h"
#include "models/user.h"
#include "models/sale.h"
#include <QDebug>

using namespace QEloquent;
using namespace Store;

void initDatabase()
{
    // Configure QEloquent
    Connection::addConnection("default", "QSQLITE", ":memory:");
    auto conn = Connection::defaultConnection();
    if (!conn.open()) {
        qFatal("Could not open database connection");
    }

    auto exec = [&conn](const char *statement) {
        auto result = QueryRunner::exec(statement, conn);
        if (!result)
            qDebug() << result.error();
    };

    auto save = [](Model &model) {
        if (!model.save()) {
            qDebug() << model.lastQuery().toString();
            qDebug() << model.lastError().text() << Qt::endl;
        }
    };

    // Create tables (minimal structure for the example)
    exec("CREATE TABLE UserRoles (id INTEGER PRIMARY KEY, name TEXT)");
    exec("CREATE TABLE Users (id INTEGER PRIMARY KEY, name TEXT, email TEXT, password TEXT, role_id INTEGER)");
    exec("CREATE TABLE Categories (id INTEGER PRIMARY KEY, name TEXT, description TEXT)");
    exec("CREATE TABLE Products (id INTEGER PRIMARY KEY, name TEXT, description TEXT, price REAL, barcode TEXT, category_id INTEGER)");
    exec("CREATE TABLE Stocks (id INTEGER PRIMARY KEY, quantity INTEGER, product_id INTEGER)");
    exec("CREATE TABLE Sales (id INTEGER PRIMARY KEY, number INTEGER, amount REAL, seller_id INTEGER)");
    exec("CREATE TABLE SaleItems (id INTEGER PRIMARY KEY, unit_price REAL, quantity INTEGER, sale_id INTEGER, product_id INTEGER)");

    // Initial Seed
    UserRole adminRole;
    adminRole.name = "Store Manager";
    save(adminRole);

    User amadou;
    amadou.name = "Amadou";
    amadou.email = "amadou@store.com";
    amadou.roleId = adminRole.id;
    save(amadou);

    Category fruits;
    fruits.name = "Fruits";
    fruits.description = "Fresh farm fruits";
    save(fruits);

    Product apple;
    apple.name = "Organic Apple";
    apple.price = 1.50;
    apple.categoryId = fruits.id;
    save(apple);

    Stock appleStock;
    appleStock.quantity = 50;
    appleStock.productId = apple.id;
    save(appleStock);

    Product banana;
    banana.name = "Fairtrade Banana";
    banana.price = 0.80;
    banana.categoryId = fruits.id;
    save(banana);

    Stock bananaStock;
    bananaStock.quantity = 5; // Low stock
    bananaStock.productId = banana.id;
    save(bananaStock);

    // A sample sale
    Sale s;
    s.number = 1001;
    s.amount = apple.price * 2;
    s.sellerId = amadou.id;
    save(s);

    SaleItem item;
    item.saleId = s.id;
    item.productId = apple.id;
    item.unitPrice = apple.price;
    item.quantity = 2;
    save(item);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    initDatabase();

    MainWindow w;
    w.show();

    return a.exec();
}
