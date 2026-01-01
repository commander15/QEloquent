#include <core/mytest.h>
#include <models/simplemodels.h>

using namespace QEloquent;

// We need DB facilities: only a valid connection in fact ;)
class SimpleModelsTest : public MyTest {};

TEST_F(SimpleModelsTest, checkMetadata) {
    const MetaObject meta = MetaObject::from<SimpleProduct>();
    ASSERT_EQ(TEST_STR(meta.className()), "SimpleProduct") << "Incorrect class name";
    ASSERT_EQ(TEST_STR(meta.tableName()), "Products") << "Incorrect table name";

    // Primary
    const MetaProperty primary = meta.primaryProperty();
    ASSERT_EQ(TEST_STR(primary.propertyName()), "id");
    ASSERT_EQ(TEST_STR(primary.fieldName()), "id");
    ASSERT_EQ(TEST_STR(primary.metaType().name()), "int");
    ASSERT_TRUE(primary.hasAttribute(MetaProperty::PrimaryProperty));
    ASSERT_EQ(primary.propertyType(), MetaProperty::StandardProperty);

    // Foreign
    const MetaProperty foreign = meta.foreignProperty();
    ASSERT_EQ(TEST_STR(foreign.propertyName()), "productId");
    ASSERT_EQ(TEST_STR(foreign.fieldName()), "product_id");
    ASSERT_EQ(foreign.metaType().name(), primary.metaType().name());
    ASSERT_EQ(foreign.propertyType(), MetaProperty::StandardProperty);

    // Label
    const MetaProperty label = meta.labelProperty();
    ASSERT_TRUE(meta.hasLabelProperty()) << "Label not found";
    ASSERT_EQ(TEST_STR(label.propertyName()), "name");
    ASSERT_EQ(TEST_STR(label.fieldName()), "name");
    ASSERT_EQ(TEST_STR(label.metaType().name()), "QString");
    ASSERT_TRUE(label.hasAttribute(MetaProperty::LabelProperty));
    ASSERT_TRUE(label.hasAttribute(MetaProperty::FillableProperty));
    ASSERT_EQ(label.propertyType(), MetaProperty::StandardProperty);

    auto properties = [&meta](MetaProperty::PropertyAttributes attributes, MetaObject::PropertyFilters filters = MetaObject::AllProperties) {
        const QList<MetaProperty> props = meta.properties(attributes, filters);
        std::string str;
        for (int i(0); i < props.size(); ++i) {
            str.append(props.at(i).propertyName().toStdString());
            if (i < props.size() - 1) str.append(", ");
        }
        return str;
    };

    // Fillable
    ASSERT_EQ(properties(MetaProperty::FillableProperty), "name, description, price, barcode, categoryId") << "fillables retrieve failed";

    // Hidden
    ASSERT_EQ(properties(MetaProperty::HiddenProperty), "categoryId") << "Hidden resolving failed";

    // Creation timestamp
    ASSERT_TRUE(meta.hasCreationTimestamp()) << "Creation timestamp not found";

    const MetaProperty createdAt = meta.creationTimestamp();
    // ...

    // Update timestamp
    ASSERT_TRUE(meta.hasUpdateTimestamp()) << "Update timestamp not found";

    const MetaProperty updatedAt = meta.updateTimestamp();
    // ...

    ASSERT_FALSE(meta.hasDeletionTimestamp()) << "Found an unknown deletion timestamp";
}

TEST_F(SimpleModelsTest, retrieveModel) {
    // Migration and seeding
    ASSERT_TRUE(migrateAndSeed()) << lastErrorText;

    // Retrieve apple
    auto result = SimpleProduct::find(1);
    ASSERT_TRUE(result) << (result ? "" : TEST_STR(result.error().text()));

    const SimpleProduct product = result.value();
    ASSERT_EQ(product.id, 1);
    ASSERT_EQ(TEST_STR(product.name), "Apple");
    ASSERT_EQ(TEST_STR(product.description), "Fresh red apple");
    ASSERT_EQ(product.price, 0.5);
    ASSERT_EQ(TEST_STR(product.barcode), "1234567890123");
    ASSERT_EQ(TEST_STR(product.createdAt.date().toString("dd/MM/yyyy hh")),
              TEST_STR(QDateTime::currentDateTime().toString("dd/MM/yyyy hh")));
    ASSERT_EQ(TEST_STR(product.updatedAt.date().toString("dd/MM/yyyy hh")),
              TEST_STR(QDateTime().toString("dd/MM/yyyy hh")));
    ASSERT_EQ(product.property("categoryId").toInt(), 1);
}
