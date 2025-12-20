# QEloquent 🚀

**QEloquent** is a powerful, flexible, and developer-friendly ORM (Object-Relational Mapping) library for Qt 6. Inspired by Laravel's Eloquent, it brings a fluent, expressive API to the world of C++ and Qt, making database interactions as simple as possible.

Built on top of Qt's robust Meta-Object system, QEloquent allows you to define models using standard Qt properties and metadata, offering a "zero-config" experience for most use cases.

---

## ✨ Features

- **Eloquent-like API**: Fluent and expressive syntax that feels natural to Laravel and RoR developers.
- **Qt Integration**: Deeply integrated with `Q_GADGET`, `Q_PROPERTY`, and `QSqlDatabase`.
- **Zero-Config Models**: Uses Qt's meta-data (`Q_CLASSINFO`) for table naming and primary key detection.
- **Powerful Relations**: Built-in support for `hasOne`, `belongsTo`, and `hasMany` relationships with transparent loading.
- **Friendly API**: Support for range-based for loops and pointer-like access to related models.
- **Fluent Query Builder**: Chainable methods for building complex SQL queries without writing raw SQL.
- **JSON Support**: Seamless conversion to and from `QJsonObject` for modern API integration.

---

## 📦 Installation

### Requirements
- **Qt 6.x** (with Sql module)
- **CMake 3.30+**
- **C++23** compliant compiler (GCC 13+, Clang 16+, or MSVC 2022+)

### Building from Source
```bash
git clone https://github.com/your-username/QEloquent.git
cd QEloquent
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH=/path/to/Qt6
make
```

---

## 🚀 Getting Started

### 1. Database Connection
Before using models, set up a database connection. QEloquent uses `QSqlDatabase` under the hood.

```cpp
#include <QEloquent/connection.h>

void setup() {
    auto conn = QEloquent::Connection::addConnection("DB", "QSQLITE", ":memory:");
    if (!conn.open()) {
        qCritical() << "Failed to open database:" << conn.lastError().text();
        return;
    }
    QEloquent::Connection::setDefault(conn);
}
```

### 2. Defining your Model
Inherit from `Model` and `ModelHelpers<T>` to unlock all functionality.

```cpp
#include <QEloquent/model.h>
#include <QEloquent/modelhelpers.h>

struct User : public QEloquent::Model, public QEloquent::ModelHelpers<User> {
    Q_GADGET
    Q_PROPERTY(int id MEMBER id)
    Q_PROPERTY(QString name MEMBER name)

    Q_CLASSINFO("table", "users")
public:
    int id;
    QString name;
};
```

---

## 🛠 Usage Guide

### Basic CRUD Operations with Error Handling

#### Create
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
    if (product.category()) {
        qDebug() << "Category Name:" << product.category()->name;
    }
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
```cpp
auto query = User::query()
    .where("active", true)
    .andWhere("age", ">", 18)
    .orderBy("created_at", Qt::DescendingOrder)
    .limit(10);

auto results = User::find(query);
```

---

## 📄 License
MIT License.
