#include <core/mytest.h>
#include <models/product.h>

#include <QEloquent/queryrunner.h>

#include <QVariant>
#include <QJsonObject>
#include <QSqlQuery>
#include <QSqlRecord>

using namespace QEloquent;

// We need a connection and migration and seeding facilities
class SimpleCRUDTest : public MyTest {};

TEST_F(SimpleCRUDTest, metadataRetrieval)
{
    const MetaObject product = MetaObject::from<Product>();

    const std::list<std::string> fillables = {
        "name", "description", "price", "barcode", "category_id"
    };

    const std::list<std::string> hidden = {
        "category_id"
    };

    const std::list<std::string> fields = {
        "id", "name", "description", "price", "barcode",
        "category_id", "created_at", "updated_at",
        "full_description", "priced", "stock", "category"
    };

    auto fieldNames = [](const QList<MetaProperty> &properties) {
        QStringList names;
        std::transform(properties.begin(), properties.end(), std::back_inserter(names), [](const MetaProperty &property) {
            return property.fieldName();
        });
        return names;
    };

    ASSERT_EQ(TEST_STR(product.className()), "Product");
    ASSERT_EQ(TEST_STR(product.tableName()), "Products");
    ASSERT_EQ(TEST_STR(product.primaryProperty().fieldName()), "id");
    ASSERT_EQ(TEST_STR(product.foreignProperty().fieldName()), "product_id");
    ASSERT_EQ(TEST_STR(product.creationTimestamp().fieldName()), "created_at");
    ASSERT_EQ(TEST_STR(product.updateTimestamp().fieldName()), "updated_at");
    ASSERT_EQ(TEST_STR_LIST(fieldNames(product.properties(MetaProperty::FillableProperty))), fillables);
    ASSERT_EQ(TEST_STR_LIST(fieldNames(product.properties(MetaProperty::HiddenProperty))), hidden);
    ASSERT_EQ(TEST_STR_LIST(fieldNames(product.properties())), fields);
    ASSERT_EQ(TEST_STR(product.connectionName()), "DB");
}

TEST_F(SimpleCRUDTest, retrievingSingleModel)
{
    auto migrationResult = migrate();
    auto seedingResult = seed();
    ASSERT_TRUE(migrationResult && seedingResult) << "Migration/Seeding failed";

    auto result = Product::find(1);
    ASSERT_TRUE(result) << (result ? "" : TEST_STR(result.error().text()));

    const Product product = result.value();
    ASSERT_EQ(TEST_STR(product.property("name").toString()), "Apple");
    ASSERT_EQ(TEST_STR(product.property("description").toString()), "Fresh red apple");
    ASSERT_EQ(product.property("price").toDouble(), 0.50);
    ASSERT_EQ(TEST_STR(product.property("barcode").toString()), "1234567890123");
    ASSERT_EQ(product.property("categoryId").toInt(), 1);

    auto stockRelation = product.stock();
    ASSERT_TRUE(stockRelation.get());
    const Stock stock = stockRelation;
    ASSERT_EQ(stock.id, 1);
}

TEST_F(SimpleCRUDTest, creatingSingleModel)
{
    auto migrationResult = migrate();
    ASSERT_TRUE(migrationResult) << "Migration failed";

    // Filling data
    QJsonObject object;
    object.insert("name", "Orange");
    object.insert("description", "Fresh oranges");
    object.insert("price", 50.0);
    object.insert("category_id", 1);

    // Storing data
    auto result = Product::create(object);
    ASSERT_TRUE(result) << (result ? "" : TEST_STR(result.error().text()));

    // Cheking if data had been stored though SQL
    QString statement = "SELECT * FROM Products WHERE id = 1";
    auto response = QueryRunner::exec(statement, connection);
    ASSERT_TRUE(response) << (response ? "" : TEST_STR(response.error().text()));
    ASSERT_TRUE(response->next());

    // Field by field check
    ASSERT_EQ(response->value("id").toInt(), 1);
    ASSERT_EQ(TEST_STR(response->value("name").toString()), TEST_STR(object.value("name").toString()));
    ASSERT_EQ(TEST_STR(response->value("description").toString()), TEST_STR(object.value("description").toString()));
    ASSERT_EQ(response->value("price").toDouble(), object.value("price").toDouble());
    ASSERT_EQ(response->value("category_id").toInt(), object.value("category_id").toInt());
}

TEST_F(SimpleCRUDTest, editSingleModel)
{
    auto migrationResult = migrate();
    auto seedingResult = seed();
    ASSERT_TRUE(migrationResult && seedingResult) << "Migration/Seeding failed";

    // Retrieving first
    auto result = Product::find(1);
    ASSERT_TRUE(result) << (result ? "" : TEST_STR(result.error().text()));

    // Filling data
    QJsonObject object;
    object.insert("id", 1);
    object.insert("name", "Orange");
    object.insert("description", "Fresh oranges");

    // Storing
    Product product = result.value();
    product.fill(object);
    ASSERT_TRUE(product.save()) << TEST_STR(product.lastError().text());

    // Cheking if data had been stored though SQL
    QString statement = "SELECT * FROM Products WHERE id = 1";
    auto response = QueryRunner::exec(statement, connection);
    ASSERT_TRUE(response) << (response ? "" : TEST_STR(response.error().text()));
    ASSERT_TRUE(response->next());

    // Field by field check
    ASSERT_EQ(response->value("id").toInt(), 1);
    ASSERT_EQ(TEST_STR(response->value("name").toString()), TEST_STR(object.value("name").toString()));
    ASSERT_EQ(TEST_STR(response->value("description").toString()), TEST_STR(object.value("description").toString()));
}


TEST_F(SimpleCRUDTest, deleteSingleModel)
{
    auto migrationResult = migrate();
    auto seedingResult = seed();
    ASSERT_TRUE(migrationResult && seedingResult) << "Migration/Seeding failed";

    Query query;
    query.where("id", 1);

    auto result = Product::remove(query);
    ASSERT_TRUE(result) << (result ? "" : TEST_STR(result.error().text()));

    ASSERT_EQ(result.value(), 1);

    // Checking though SQL
    QString statement = "SELECT * FROM Products WHERE id = 1";
    auto response = QueryRunner::exec(statement, connection);
    ASSERT_TRUE(response) << (response ? "" : TEST_STR(response.error().text()));
    ASSERT_FALSE(response->next());
}

TEST_F(SimpleCRUDTest, countTest)
{
    auto migrationResult = migrate();
    auto seedingResult = seed();
    ASSERT_TRUE(migrationResult && seedingResult) << "Migration/Seeding failed";

    auto result = Product::count();
    ASSERT_TRUE(result) << (result ? "" : TEST_STR(result.error().text()));
    ASSERT_EQ(result.value(), 3);

    // Checking though SQL
    QString statement = "SELECT COUNT(id) FROM Products";
    auto response = QueryRunner::exec(statement, connection);
    ASSERT_TRUE(response) << (response ? "" : TEST_STR(response.error().text()));
    ASSERT_TRUE(response->next());
    ASSERT_EQ(response->value(0).toInt(), 3);
}
