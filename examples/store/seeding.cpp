#include "common.h"

#include <QEloquent/seeder.h>

#include "models/product.h"
#include "models/user.h"
#include "models/sale.h"

using namespace Store;
using namespace QEloquent;

Result<int> seed()
{
    int count = 0;
    auto save = [&count](Model &model) {
        if (!model.save()) {
            qDebug() << model.lastQuery().toString();
            qDebug() << model.lastError().text() << Qt::endl;
        } else {
            ++count;
        }
    };

    Seeder::enableAutoRegistration();

    Seeder::create("initial user roles", [] {
        auto count = UserRole::count();
        return (count ? count.value() == 0 : false);
    }, [&save] {
        UserRole adminRole;
        adminRole.name = "Store Manager";
        save(adminRole);
    });

    Seeder::create("initial user", [] {
        auto count = User::count();
        return (count ? count.value() == 0 : false);
    }, [&save] {
        auto role = UserRole::find(Query().where("name", "Store Manager"));
        if (!role || role->isEmpty()) {
            // ...
            return;
        }

        User amadou;
        amadou.name = "Amadou";
        amadou.email = "amadou@store.com";
        amadou.password = "1234";
        amadou.roleId = role->constFirst().id;
        save(amadou);

        User guest;
        guest.name = "Celestin";
        guest.email = "guest@store.com";
        guest.password = "1234";
        guest.roleId = role->constFirst().id;
        save(guest);
    });

    Seeder::create("initial products", [] {
        auto count = Product::count();
        return (count ? count.value() == 0 : false);
    }, [&save] {
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
    });

    Seeder::create("sample sale", [] {
        auto count = Sale::count();
        return (count ? count.value() == 0 : false);
    }, [&save] {
        auto users = User::find(Query().where("name", "Amadou"));
        if (!users || users->isEmpty())
            return;

        auto products = Product::find(Query().where("name", "Organic Apple"));
        if (!products || products->isEmpty())
            return;

        const User amadou = users->constFirst();
        const Product apple = products->constFirst();

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
    });

    Seeder::disableAutoRegistration();

    qDebug() << "Seeding...";
    return Seeder::runAll([](Seeder *seeder) {
        qDebug() << "running " << seeder->name();
    });
}
