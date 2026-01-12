#include "common.h"

#include "models/product.h"
#include "models/user.h"
#include "models/sale.h"

using namespace Store;
using namespace QEloquent;

Result<int> seed()
{
    // Checks
    auto roleCount = UserRole::count();
    if (roleCount && roleCount.value() > 0)
        return 0;

    int count = 0;

    auto save = [&count](Model &model) {
        if (!model.save()) {
            qDebug() << model.lastQuery().toString();
            qDebug() << model.lastError().text() << Qt::endl;
        } else {
            ++count;
        }
    };

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

    return count;
}
