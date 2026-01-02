#include "complexmodel.h"

#include <models/complexmodels.h>

#include <QJsonArray>

TEST_F(ComplexModel, RetrieveWithHasOneRelation) {
    // Migration and seeding
    ASSERT_TRUE(migrateAndSeed()) << lastErrorText;

    auto result = Product::find(1);
    ASSERT_TRUE(result) << (result ? "" : TEST_STR(result.error().text()));

    Product product = result.value();
    auto stockRelation = product.stock();

    ASSERT_TRUE(stockRelation.get());
    ASSERT_EQ(stockRelation.count(), 1);
    ASSERT_EQ(stockRelation->id, 1);
    ASSERT_EQ(stockRelation->property("productId").toInt(), 1);
}

TEST_F(ComplexModel, RetrieveWithBelongsTo) {
    // Migration and seeding
    ASSERT_TRUE(migrateAndSeed()) << lastErrorText;

    auto result = Stock::find(1);
    ASSERT_TRUE(result) << (result ? "" : TEST_STR(result.error().text()));

    Stock stock = result.value();
    auto productRelation = stock.product();

    ASSERT_TRUE(productRelation.get());
    ASSERT_EQ(productRelation.count(), 1);
    ASSERT_EQ(productRelation->id, 1);
    ASSERT_EQ(TEST_STR(productRelation->name), "Apple");
}

TEST_F(ComplexModel, RetrieveWithHasMany) {
    // Migration and seeding
    ASSERT_TRUE(migrateAndSeed()) << lastErrorText;

    auto result = Category::find(1); // Fruits
    ASSERT_TRUE(result) << (result ? "" : TEST_STR(result.error().text()));

    Category category = result.value();
    auto productsRelation = category.products();

    ASSERT_TRUE(productsRelation.get());
    ASSERT_EQ(productsRelation.count(), 2); // Apple and Banana

    QStringList productNames;
    for (const auto &product : productsRelation) {
        productNames << product.name;
    }
    ASSERT_TRUE(productNames.contains("Apple"));
    ASSERT_TRUE(productNames.contains("Banana"));
}

TEST_F(ComplexModel, RetrieveWithReverseBelongsTo) {
    // Migration and seeding
    ASSERT_TRUE(migrateAndSeed()) << lastErrorText;

    auto result = Product::find(1); // Apple
    ASSERT_TRUE(result) << (result ? "" : TEST_STR(result.error().text()));

    Product product = result.value();
    auto categoryRelation = product.category();

    ASSERT_TRUE(categoryRelation.get());
    ASSERT_EQ(categoryRelation.count(), 1);
    ASSERT_EQ(TEST_STR(categoryRelation->name), "Fruits");
}

TEST_F(ComplexModel, RetrieveWithBelongsToMany) {
    // Migration and seeding
    ASSERT_TRUE(migrateAndSeed()) << lastErrorText;

    // Test User -> UserGroup
    auto userResult = User::find(1); // Admin
    ASSERT_TRUE(userResult);
    User admin = userResult.value();

    auto groupsRelation = admin.groups();
    ASSERT_TRUE(groupsRelation.get());
    ASSERT_GE(groupsRelation.count(), 1);

    bool foundManagers = false;
    for (const auto &group : groupsRelation) {
        if (TEST_STR(group.name) == "Stock Managers") foundManagers = true;
    }
    ASSERT_TRUE(foundManagers);

    // Test UserGroup -> User
    auto groupResult = UserGroup::find(1); // Stock Managers
    ASSERT_TRUE(groupResult);
    UserGroup group = groupResult.value();

    auto usersRelation = group.users();
    ASSERT_TRUE(usersRelation.get());
    ASSERT_GE(usersRelation.count(), 1);

    bool foundUser = false;
    for (const auto &user : usersRelation) {
        if (TEST_STR(user.name) == "Amadou Benjamain") foundUser = true;
    }
    ASSERT_TRUE(foundUser);
}

TEST_F(ComplexModel, RetrieveWithBelongsToManyThrough) {
    // Migration and seeding
    ASSERT_TRUE(migrateAndSeed()) << lastErrorText;

    // Sale (1) -> seller_id (1) -> User (1) -> Groups (1, 2)
    auto result = Sale::find(1);
    ASSERT_TRUE(result);
    Sale sale = result.value();

    auto groupsRelation = sale.groups();
    ASSERT_TRUE(groupsRelation.get());
    ASSERT_EQ(groupsRelation.count(), 2);

    bool foundManagers = false;
    for (const auto &group : groupsRelation) {
        if (TEST_STR(group.name) == "Stock Managers") foundManagers = true;
    }
    ASSERT_TRUE(foundManagers);
}

TEST_F(ComplexModel, RetrieveWithHasManyThrough) {
    // Migration and seeding
    ASSERT_TRUE(migrateAndSeed()) << lastErrorText;

    // Category -> SaleItem (through Product)
    auto categoryResult = Category::find(1); // Fruits
    ASSERT_TRUE(categoryResult);
    Category fruits = categoryResult.value();

    auto saleItemsRelation = fruits.saleItems();
    ASSERT_TRUE(saleItemsRelation.get());

    // Apple (id 1) belongs to Fruits (id 1)
    // Seeding should have a sale with Apple
    ASSERT_GE(saleItemsRelation.count(), 1);

    for (const auto &item : saleItemsRelation) {
        auto productResult = item.product().get();
        ASSERT_TRUE(productResult);
        ASSERT_EQ(item.product()->property("categoryId").toInt(), fruits.id);
    }
}

TEST_F(ComplexModel, FriendlyApiWorks) {
    // Migration and seeding
    ASSERT_TRUE(migrateAndSeed()) << lastErrorText;

    auto pResult = Product::find(1); // Apple
    Product apple = pResult.value();

    // 1. QList-like methods
    auto stockRelation = apple.stock();
    ASSERT_EQ(stockRelation.count(), 1);
    ASSERT_EQ(stockRelation->id, 1);
    ASSERT_FALSE(stockRelation.isEmpty());

    // 2. operator-> and operator*
    ASSERT_EQ(stockRelation->id, 1);
    ASSERT_EQ((*stockRelation).id, 1);

    // 3. Implicit conversion
    Stock stock = stockRelation;
    ASSERT_EQ(stock.id, 1);

    // 4. std-container compliance (iteration)
    auto cResult = Category::find(1); // Fruits
    Category fruits = cResult.value();
    auto productsRelation = fruits.products();

    int count = 0;
    for (const Product &p : productsRelation) {
        count++;
        ASSERT_FALSE(p.name.isEmpty());
    }
    ASSERT_EQ(count, 2);

    //return;
    auto result = Category::find(QEloquent::Query().with("products").with("saleItems"));
    if (result) {
        QJsonArray array;
        for (const auto &c : std::as_const(result).value()) {
            array.append(c.toJsonObject());
            static bool debugged = false;

            if (!debugged) {
                writeFile(c.metaObject().tableName() + ".csv", c.toCsv(QEloquent::SerializationFormat::Pretty));
                debugged = true;
            }
        }
        writeFile("categories.json", QJsonDocument(array).toJson());
    }
}
