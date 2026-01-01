#include "metadata.h"

#include <models/invalidmodels.h>
#include <models/simplemodels.h>
#include <models/complexmodels.h>

#include <QEloquent/metaobject.h>

using namespace QEloquent;

template<typename Model>
void expectInvalidMetaObject() {
    const MetaObject meta = MetaObject::from<Model>();
    ASSERT_FALSE(meta.isValid()) << "valid meta object produced for invalid model " + TEST_STR(meta.className());
}

TEST_F(MetaData, InvalidModelsProducesInvalidMetaObjects) {
    expectInvalidMetaObject<NoGadgetModel>();
    expectInvalidMetaObject<NoPropertiesModel>();
    expectInvalidMetaObject<NoIdModel>();
}

TEST_F(MetaData, ValidSimpleModelProducesValidMetaObject) {
    // Basic validity and naming
    const MetaObject meta = MetaObject::from<SimpleProduct>();
    ASSERT_TRUE(meta.isValid()) << "valid model should produce valid MetaObject";
    ASSERT_EQ(TEST_STR(meta.className()), "SimpleProduct") << "incorrect class name";
    ASSERT_EQ(TEST_STR(meta.tableName()), "Products") << "incorrect table name";

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
    ASSERT_TRUE(meta.hasLabelProperty()) << "label not found";
    ASSERT_EQ(TEST_STR(label.propertyName()), "name");
    ASSERT_EQ(TEST_STR(label.fieldName()), "name");
    ASSERT_EQ(TEST_STR(label.metaType().name()), "QString");
    ASSERT_TRUE(label.hasAttribute(MetaProperty::LabelProperty));
    ASSERT_TRUE(label.hasAttribute(MetaProperty::FillableProperty));
    ASSERT_EQ(label.propertyType(), MetaProperty::StandardProperty);

    auto properties = [&meta](MetaProperty::PropertyAttributes attributes, MetaObject::PropertyFilters filters = MetaObject::AllProperties) {
        const QList<MetaProperty> props = (attributes == MetaProperty::NoAttibutes ? meta.properties(filters) : meta.properties(attributes, filters));
        std::string str;
        for (int i(0); i < props.size(); ++i) {
            str.append(props.at(i).propertyName().toStdString());
            if (i < props.size() - 1) str.append(", ");
        }
        return str;
    };

    // Append
    ASSERT_EQ(properties(MetaProperty::NoAttibutes, MetaObject::AppendedProperties), "since");

    // Fillable
    ASSERT_EQ(properties(MetaProperty::FillableProperty), "name, description, price, barcode, categoryId") << "fillables retrieve failed";

    // Hidden
    ASSERT_EQ(properties(MetaProperty::HiddenProperty), "categoryId") << "hidden resolving failed";

    // Creation timestamp
    ASSERT_TRUE(meta.hasCreationTimestamp()) << "creation timestamp not found";

    const MetaProperty createdAt = meta.creationTimestamp();
    // ...

    // Update timestamp
    ASSERT_TRUE(meta.hasUpdateTimestamp()) << "update timestamp not found";

    const MetaProperty updatedAt = meta.updateTimestamp();
    // ...

    ASSERT_FALSE(meta.hasDeletionTimestamp()) << "found an unknown deletion timestamp";
}

TEST_F(MetaData, ValidComplexModelProducesValidMetaObject) {
    // Basic validity and naming
    const MetaObject meta = MetaObject::from<Product>();
    ASSERT_TRUE(meta.isValid()) << "valid model should produce valid MetaObject";
    ASSERT_EQ(TEST_STR(meta.className()), "Product") << "incorrect class name";
    ASSERT_EQ(TEST_STR(meta.tableName()), "Products") << "incorrect table name";

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
    ASSERT_TRUE(meta.hasLabelProperty()) << "label not found";
    ASSERT_EQ(TEST_STR(label.propertyName()), "name");
    ASSERT_EQ(TEST_STR(label.fieldName()), "name");
    ASSERT_EQ(TEST_STR(label.metaType().name()), "QString");
    ASSERT_TRUE(label.hasAttribute(MetaProperty::LabelProperty));
    ASSERT_TRUE(label.hasAttribute(MetaProperty::FillableProperty));
    ASSERT_EQ(label.propertyType(), MetaProperty::StandardProperty);

    auto properties = [&meta](MetaProperty::PropertyAttributes attributes, MetaObject::PropertyFilters filters = MetaObject::AllProperties) {
        const QList<MetaProperty> props = (attributes == MetaProperty::NoAttibutes ? meta.properties(filters) : meta.properties(attributes, filters));
        std::string str;
        for (int i(0); i < props.size(); ++i) {
            str.append(props.at(i).propertyName().toStdString());
            if (i < props.size() - 1) str.append(", ");
        }
        return str;
    };

    // Append
    ASSERT_EQ(properties(MetaProperty::NoAttibutes, MetaObject::AppendedProperties), "since");

    // Fillable
    ASSERT_EQ(properties(MetaProperty::FillableProperty), "name, description, price, barcode, categoryId") << "fillables retrieve failed";

    // Hidden
    ASSERT_EQ(properties(MetaProperty::HiddenProperty), "categoryId") << "hidden resolving failed";

    // Relation
    ASSERT_EQ(properties(MetaProperty::NoAttibutes, MetaObject::RelationProperties), "stock, category");

    // Creation timestamp
    ASSERT_TRUE(meta.hasCreationTimestamp()) << "creation timestamp not found";

    const MetaProperty createdAt = meta.creationTimestamp();
    // ...

    // Update timestamp
    ASSERT_TRUE(meta.hasUpdateTimestamp()) << "update timestamp not found";

    const MetaProperty updatedAt = meta.updateTimestamp();
    // ...

    ASSERT_FALSE(meta.hasDeletionTimestamp()) << "found an unknown deletion timestamp";
}
