#include "relationtest.h"
#include "models/product.h"

using namespace QEloquent;

TEST_F(RelationTest, hasOneRelation)
{
    auto migrationResult = migrate();
    auto seedingResult = seed();
    ASSERT_TRUE(migrationResult && seedingResult) << "Migration/Seeding failed";

    auto result = Product::find(1);
    ASSERT_TRUE(result) << (result ? "" : TEST_STR(result.error().text()));

    Product product = result.value();
    auto stockRelation = product.stock();
    
    ASSERT_TRUE(stockRelation.get());
    ASSERT_EQ(stockRelation.related().count(), 1);
    ASSERT_EQ(stockRelation.related().first().id, 1);
    ASSERT_EQ(stockRelation.related().first().productId, 1);
}

TEST_F(RelationTest, belongsToRelation)
{
    auto migrationResult = migrate();
    auto seedingResult = seed();
    ASSERT_TRUE(migrationResult && seedingResult) << "Migration/Seeding failed";

    auto result = Stock::find(1);
    ASSERT_TRUE(result) << (result ? "" : TEST_STR(result.error().text()));

    Stock stock = result.value();
    auto productRelation = stock.product();

    ASSERT_TRUE(productRelation.get());
    ASSERT_EQ(productRelation.related().count(), 1);
    ASSERT_EQ(productRelation.related().first().id, 1);
    ASSERT_EQ(TEST_STR(productRelation.related().first().name), "Apple");
}

TEST_F(RelationTest, hasManyRelation)
{
    auto migrationResult = migrate();
    auto seedingResult = seed();
    ASSERT_TRUE(migrationResult && seedingResult) << "Migration/Seeding failed";

    auto result = Category::find(1); // Fruits
    ASSERT_TRUE(result) << (result ? "" : TEST_STR(result.error().text()));

    Category category = result.value();
    auto productsRelation = category.products();

    ASSERT_TRUE(productsRelation.get());
    ASSERT_EQ(productsRelation.related().count(), 2); // Apple and Banana
    
    QStringList productNames;
    for (const auto &product : productsRelation.related()) {
        productNames << product.name;
    }
    ASSERT_TRUE(productNames.contains("Apple"));
    ASSERT_TRUE(productNames.contains("Banana"));
}

TEST_F(RelationTest, belongsToReverseRelation)
{
    auto migrationResult = migrate();
    auto seedingResult = seed();
    ASSERT_TRUE(migrationResult && seedingResult) << "Migration/Seeding failed";

    auto result = Product::find(1); // Apple
    ASSERT_TRUE(result) << (result ? "" : TEST_STR(result.error().text()));

    Product product = result.value();
    auto categoryRelation = product.category();

    ASSERT_TRUE(categoryRelation.get());
    ASSERT_EQ(categoryRelation.related().count(), 1);
    ASSERT_EQ(TEST_STR(categoryRelation.related().first().name), "Fruits");
}
TEST_F(RelationTest, apiEnhancements)
{
    auto migrationResult = migrate();
    auto seedingResult = seed();
    ASSERT_TRUE(migrationResult && seedingResult) << "Migration/Seeding failed";

    auto pResult = Product::find(1); // Apple
    Product apple = pResult.value();

    // 1. QList-like methods
    auto stockRelation = apple.stock();
    ASSERT_EQ(stockRelation.count(), 1);
    ASSERT_EQ(stockRelation.first().id, 1);
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
}
