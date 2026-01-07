#include "schema.h"

#include <QEloquent/schema.h>

#include <QVariant>
#include <QDateTime>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlField>

TEST_F(Schema, SimpleTableCreationSucceed) {
    QEloquent::Schema::create("tests", [](QEloquent::TableBlueprint &table) {
        table.id();
        table.string("name", 30);
        table.string("first_name", 15).nullable();
        table.date("birth_date");
        table.string("email", 100).unique();
        table.doubleNumber("height").min(0.0).max(3.0);
        table.character("sex", 1).check("sex = 'M' OR sex = 'F'");
        table.boolean("completed").defaultValue(true);
        table.timestamps();
    });

    const QSqlRecord record = connection.database().record("tests");
    ASSERT_EQ(record.count(), 10) << "Incorrect field number";

    QSqlField field;

    field = record.field(0);
    ASSERT_EQ(TEST_STR(field.name()), "id");
    ASSERT_EQ(TEST_STR(field.metaType().name()), TEST_STR(QMetaType::fromType<int>().name()));
    ASSERT_TRUE(field.isAutoValue());
    ASSERT_EQ(field.requiredStatus(), QSqlField::Optional);

    field = record.field(1);
    ASSERT_EQ(TEST_STR(field.name()), "name");
    ASSERT_EQ(TEST_STR(field.metaType().name()), TEST_STR(QMetaType::fromType<QString>().name()));
    if (field.length() > 0) ASSERT_EQ(field.length(), 30);
    ASSERT_EQ(field.requiredStatus(), QSqlField::Required);

    field = record.field(2);
    ASSERT_EQ(TEST_STR(field.name()), "first_name");
    ASSERT_EQ(TEST_STR(field.metaType().name()), TEST_STR(QMetaType::fromType<QString>().name()));
    if (field.length() > 0) ASSERT_EQ(field.length(), 15);
    ASSERT_EQ(field.requiredStatus(), QSqlField::Optional);

    field = record.field(3);
    ASSERT_EQ(TEST_STR(field.name()), "birth_date");
    ASSERT_EQ(TEST_STR(field.metaType().name()), TEST_STR(QMetaType::fromType<QString>().name()));
    ASSERT_EQ(field.requiredStatus(), QSqlField::Required);

    field = record.field(4);
    ASSERT_EQ(TEST_STR(field.name()), "email");
    ASSERT_EQ(TEST_STR(field.metaType().name()), TEST_STR(QMetaType::fromType<QString>().name()));
    if (field.length() > 0) ASSERT_EQ(field.length(), 100);
    ASSERT_EQ(field.requiredStatus(), QSqlField::Required);

    field = record.field(5);
    ASSERT_EQ(TEST_STR(field.name()), "height");
    ASSERT_EQ(TEST_STR(field.metaType().name()), TEST_STR(QMetaType::fromType<double>().name()));
    ASSERT_EQ(field.requiredStatus(), QSqlField::Required);

    field = record.field(6);
    ASSERT_EQ(TEST_STR(field.name()), "sex");
    ASSERT_EQ(TEST_STR(field.metaType().name()), TEST_STR(QMetaType::fromType<QString>().name()));
    if (field.length() > 0) ASSERT_EQ(field.length(), 1);
    ASSERT_EQ(field.requiredStatus(), QSqlField::Required);

    field = record.field(7);
    ASSERT_EQ(TEST_STR(field.name()), "completed");
    ASSERT_EQ(TEST_STR(field.metaType().name()), TEST_STR(QMetaType::fromType<int>().name()));
    if (field.length() > 0) ASSERT_EQ(field.length(), 1);
    ASSERT_EQ(field.requiredStatus(), QSqlField::Required);

    field = record.field(8);
    ASSERT_EQ(TEST_STR(field.name()), "created_at");
    ASSERT_EQ(TEST_STR(field.metaType().name()), TEST_STR(QMetaType::fromType<QString>().name()));
    ASSERT_EQ(field.requiredStatus(), QSqlField::Required);

    field = record.field(9);
    ASSERT_EQ(TEST_STR(field.name()), "updated_at");
    ASSERT_EQ(TEST_STR(field.metaType().name()), TEST_STR(QMetaType::fromType<QString>().name()));
    ASSERT_EQ(field.requiredStatus(), QSqlField::Optional);
}
