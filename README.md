# QEloquent 🚀

**QEloquent** is a powerful, flexible, and developer-friendly ORM (Object-Relational Mapping) library for **Qt 6**. Inspired by **Laravel's Eloquent**, it brings a fluent, expressive API to the world of **C++** and **Qt**, making database interactions as simple as possible.



Built on top of Qt's robust Meta-Object system, QEloquent allows you to define models using standard Qt properties and metadata, offering a "zero-config" experience for most use cases.

---

## ✨ Features

- **Eloquent-like API**: Fluent and expressive syntax that feels natural to Laravel and RoR developers.
- **Qt Integration**: Deeply integrated with `Q_GADGET`, `Q_PROPERTY`, and `QSqlDatabase`.
- **Zero-Config Models**: Uses Qt's meta-data (`Q_CLASSINFO`) for table naming and primary key detection.
- **Powerful Relations**: Built-in support for `hasOne`, `belongsTo`, `hasMany` relationships with transparent loading.
- **Friendly API**: Support for range-based for loops and pointer-like access to related models.
- **Fluent Query Builder**: Chainable methods for building complex SQL queries without writing raw SQL.
- **JSON Support**: Seamless conversion to and from `QJsonObject` for modern API integration.

---

## 📦 Installation

### Requirements

- **Qt 6.x** (with Sql module)
- **CMake 3.30+**
- **C++20** compliant compiler (GCC 10+, Clang 10+, or MSVC 2019+)

### Building from Source

```bash
git clone https://github.com/commander15/QEloquent.git
cd QEloquent
mkdir build && cd build
cmake -S .. -DCMAKE_PREFIX_PATH=/path/to/Qt6 -DCMAKE_INSTALL_PREFIX=/path/to/prefix
cmake --build . --target install
```

---

## 🚀 Getting Started

### 1. Database Connection

Before using models, set up a database connection.

```cpp
#include <QEloquent/connection.h>

void setup() {
    // First added connection is considered as the default one !
    auto conn = QEloquent::Connection::addConnection("DB", "QSQLITE", ":memory:");
    if (!conn.open()) {
        qCritical() << "Failed to open database:" << conn.lastError().text();
        return;
    }
}
```

### 2. Defining your Model

To define a model in QEloquent, inherit from **QEloquent::Model** and **QEloquent::ModelHelpers<YourModel>** (the latter unlocks all the powerful static methods like *find()*, *all()*, *create()*, *etc*.).


QEloquent heavily leverages Qt's meta-object system, so most configuration is done via **Q_PROPERTY** and **Q_CLASSINFO**. This gives you a zero-config experience by default, while still offering full control when needed.

```cpp
#include <QEloquent/model.h>
#include <QEloquent/modelhelpers.h>

struct User : public QEloquent::Model, public QEloquent::ModelHelpers<User>
{
    Q_GADGET

    // Core fields — mapped directly to database columns
    Q_PROPERTY(int id MEMBER id)
    Q_PROPERTY(QString name MEMBER name)
    Q_PROPERTY(QString email MEMBER email)
    Q_PROPERTY(QString password MEMBER password)

    // Optional: Explicit table name.
    // By default, QEloquent converts the class name to to an appropriate table name
    // according to the naming convention set.
    // For Laravel naming convention, we can have: "User" → "users".
    Q_CLASSINFO("table", "users")

    // Optional: Naming convention.
    // "Laravel" (default): class name → snake_case + pluralized for table name,
    // property name → snake_case for column name.
    // "OneOne" alternative: property name = column name exactly,
    // table name = class name pluralized.
    Q_CLASSINFO("naming", "Laravel")

    // Optional: Hide fields from serialization (e.g., JSON output).
    // Use property names here — the corresponding database fields will be omitted
    // from serialized output.
    // If the properties are not regular (Q_PROPERTY based), they are considered as
    // dynamic properties so don't need to worry if they are used for relation resolution
    // They don't appear during serialization but they are still there and accessible 
    // using property() method.
    Q_CLASSINFO("hidden", "password, roleId")

    // Optional: Mass-assignable (fillable) properties.
    // By default, all writable Q_PROPERTY and dynamic properties are fillable.
    // Restricting this protects against mass-assignment vulnerabilities.
    // Note: only regular writable and dynamic properties supported
    Q_CLASSINFO("fillable", "name, email, password")

    // Optional: Computed/appended attributes.
    // These are not stored in the database but are included in serialization and debug output.
    // The key name in output will follow the naming convention (e.g., snake_case) if no
    // override had been provided (see note below).
    // Requires a parameterless Q_INVOKABLE accessor with the same name.
    Q_CLASSINFO("append", "emailHeader")

    // Optional: Eager loading.
    // Relations listed here are automatically loaded whenever the model is retrieved.
    // Requires a parameterless Q_INVOKABLE relation method with the same name.
    Q_CLASSINFO("with", "role")

    // Required when inheriting from a model that already includes ModelHelpers.
    // Resolves potential static method ambiguity.
    QELOQUENT_MODEL_HELPERS(User)

public:
    // Database-mapped fields
    int id = 0;
    QString name;
    QString email;
    QString password;

    // Appended (computed) attribute
    Q_INVOKABLE QString emailHeader() const
    {
        // This value will appear in JSON/debug output thanks to "append"
        return name + " <" + email + ">";
    }

    // One-to-one / many-to-one relation
    Q_INVOKABLE QEloquent::Relation<UserRole> role() const
    {
        // Parameters are optional — QEloquent can often infer them automatically
        return belongsTo<UserRole>("role_id", "id");
    }

    // Many-to-many relation
    Q_INVOKABLE QEloquent::Relation<UserGroup> groups() const
    {
        // Pivot table and key names — also often auto-deducible
        return belongsToMany<UserGroup>("UserGroupMembers", "user_id", "group_id");
    }
};
```

### Additional note

- For any property (standard Q_PROPERTY, appended attribute, or relation), you can manually override the database field name used for serialization using Q_CLASSINFO("<propertyName>Field", "custom_field_name").
  This bypasses the naming convention entirely for that specific property.

- Both append and with require a matching parameterless **Q_INVOKABLE** method with the exact same name as the entry.
  QEloquent uses the Qt meta-system to discover and call these automatically.

This approach keeps your models *clean*, *expressive*, and *highly configurable* without boilerplate.

---

## 🛠 Usage Guide

### Basic CRUD Operations with Error Handling

#### Create

You can create models in many ways, you can for example use the create method provided by the helper so it can persist in DB before providing the model instance.



**Note**: You can also create the instance by yourself fill the data on it the way you want and call save() on it.

```cpp
QJsonObject data;
data["name"] = "John Doe";

auto result = User::create(data);
if (result) {
    User user = result.value();
    qDebug() << "Created user with ID:" << user.id;
} else {
    qWarning() << "Create failed:" << result.error().text();
}
```

#### Read (Find)

There is many way to *find* a model, you can relay on *find* helper or just construct a default one, set it property value and call *get()* on it.

```cpp
// Find by primary key (returns Result<Model, Error>)
auto result = User::find(1);
if (result) {
    User user = result.value();
    qDebug() << "Found:" << user.name;
} else {
    qWarning() << "User not found or DB error:" << result.error().text();
}

// Find using Query Builder (returns Result<QList<Model>, Error>)
auto users = User::find(User::query().where("name", "John Doe"));
if (!users) {
    qWarning() << "Query failed:" << users.error().text();
}
```

#### Update & Delete

Just as bellow or retreive as you can and call *deleteData()* on the model.

```cpp
auto result = User::find(1);
if (result) {
    User user = result.value();
    user.name = "Jane Doe";
    if (!user.save()) {
        qWarning() << "Update failed:" << user.lastError().text();
    }

    if (!user.deleteData()) {
         qWarning() << "Delete failed:" << user.lastError().text();
    }
}
```

---

### 🔗 User-Friendly Relationships

QEloquent relations support **transparent loading**—the database query is executed automatically the first time you access the relation data.

#### One to Many (Iteration)

```cpp
auto result = Category::find(1);
if (result) {
    Category category = result.value();

    // Transparently loads and iterates products
    for (const Product &product : category.products()) {
        qDebug() << "Product:" << product.name;
    }
}
```

#### Single Relation (Pointer-like Access)

For `hasOne` or `belongsTo`, use the `->` operator for direct access to the related model's properties.

```cpp
auto result = Product::find(i);
if (result) {
    Product product = result.value();

    // Access category directly via ->
    qDebug() << "Category Name:" << product.category()->name;
}
```

#### Implicit Conversion

You can assign a relation directly to a model variable. If no related model exists, you get a default-constructed model.

```cpp
Stock stock = product.stock(); // Implicitly calls .first()
if (stock.exists()) {
    qDebug() << "Stock level:" << stock.quantity;
}
```

---

### 🔍 Advanced Querying

QEloquent use the **Query** class for high and low level data filtering, that is why you can only use database field name on it (no properties there), the *query()* function from ModelHelpers<YourModel> help you create an empty query that contains enough informations to build a request targeting the model database table using QEloquent query builder, usefull for advanced data queries without going fully RAW SQL.



Below, you will see how we use it to query models based on filters:

```cpp
auto query = User::query()
    .where("active", true)
    .andWhere("age", ">", 18)
    .orderBy("created_at", Qt::DescendingOrder)
    .limit(10);

auto results = User::find(query);
if (results) {
    for (const User &user : results) {
        qDebug() << "User: " << user.name;
    }
}
```

---

## 📄 License

**LGPLv3** (GNU Lesser General Public License version 3)



**Note**

If you distribute an application that uses QEloquent under the LGPL,
you must include a visible acknowledgment such as:
"This software uses QEloquent - https://github.com/commander15/QEloquent"
preferably in the About dialog or credits screen.
