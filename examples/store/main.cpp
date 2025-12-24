#include <QApplication>
#include <QEloquent/connection.h>
#include <QEloquent/queryrunner.h>
#include "ui/mainwindow.h"
#include "ui/logindialog.h"
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

    auto table = [&conn](const QString &name, QStringList fields, bool withTimestamps = true) {
        fields.prepend("id INTEGER PRIMARY KEY");
        if (withTimestamps) {
            fields.append("created_at TIMESTAMP");
            fields.append("updated_at TIMESTAMP");
        }

        const QString statement = "CREATE TABLE " + name + " (" + fields.join(", ") + ")";

        auto result = QueryRunner::exec(statement, conn);
        if (!result) {
            qDebug() << statement;
            qDebug() << result.error();
        }
    };

    auto save = [](Model &model) {
        if (!model.save()) {
            qDebug() << model.lastQuery().toString();
            qDebug() << model.lastError().text() << Qt::endl;
        }
    };

    table("UserRoles", { "name TEXT" });
    table("Users", { "name TEXT", "email TEXT", "password TEXT", "role_id INTEGER" });
    table("Categories", { "name TEXT", "description TEXT" });
    table("Products", { "name TEXT", "description TEXT", "price REAL", "barcode TEXT", "category_id INTEGER" });
    table("Stocks", { "quantity INTEGER", "product_id INTEGER" });
    table("Sales", { "number INTEGER", "amount REAL", "seller_id INTEGER" });
    table("SaleItems", { "unit_price REAL", "quantity REAL", "sale_id INTEGER", "product_id INTEGER" });

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

    LoginDialog login;
    if (login.exec() != QDialog::Accepted) {
        return 0;
    }

    MainWindow w;
    w.show();

    return a.exec();
}
