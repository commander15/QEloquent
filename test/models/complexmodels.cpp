#include "complexmodels.h"

Product::Product()
    : SimpleProduct(this)
{}

QString Product::fullDescription() const {
    return name + " - " + description;
}

QString Product::priced() const {
    return QString::number(price) + " XAF";
}

QEloquent::Relation<Stock> Product::stock() const {
    return hasOne<Stock>();
}

QEloquent::Relation<Category> Product::category() const {
    return belongsTo<Category>();
}

Stock::Stock() :
    SimpleStock(this)
{}

QEloquent::Relation<Product> Stock::product() const {
    return belongsTo<Product>();
}

Category::Category()
    : SimpleCategory(this)
{}

int Category::productCount() const
{
    return products().count();
}

QEloquent::Relation<Product> Category::products() const {
    return hasMany<Product>();
}

QEloquent::Relation<SaleItem> Category::saleItems() const {
    return hasManyThrough<SaleItem, Product>("category_id", "id", "product_id", "id");
}

User::User()
    : QEloquent::Model(this)
{}

QEloquent::Relation<UserGroup> User::groups() const {
    return belongsToMany<UserGroup>("UserGroupMembers", "user_id", "group_id");
}

UserGroup::UserGroup()
    : QEloquent::Model(this)
{}

QEloquent::Relation<User> UserGroup::users() const {
    return belongsToMany<User>("UserGroupMembers", "group_id", "user_id");
}

Sale::Sale()
    : QEloquent::Model(this)
{}

QEloquent::Relation<User> Sale::seller() const {
    return belongsTo<User>("seller_id");
}

QEloquent::Relation<UserGroup> Sale::groups() const {
    return belongsToManyThrough<UserGroup, User>("UserGroupMembers", "user_id", "group_id", "seller_id");
}

SaleItem::SaleItem()
    : QEloquent::Model(this)
{}

QEloquent::Relation<Sale> SaleItem::sale() const {
    return belongsTo<Sale>();
}

QEloquent::Relation<Product> SaleItem::product() const {
    return belongsTo<Product>();
}
