# 🚀 QEloquent v1.0.0 - The Eloquent ORM for Qt6

We are thrilled to announce the first stable release of **QEloquent**, a powerful, developer-friendly ORM for C++ and Qt6, inspired by the elegance of Laravel's Eloquent.

QEloquent bridges the gap between modern C++ efficiency and the productivity of high-level ORMs, providing a fluent interface for database interactions.

---

## 🌟 Key Features

### 💎 Eloquent-Style Models
Define your data structures using standard Qt properties and metadata. Leverage the power of `Q_GADGET` and `Q_PROPERTY` to create self-documenting, powerful models.

```cpp
class Product : public QEloquent::Model, public QEloquent::ModelHelpers<Product> {
    Q_GADGET
    Q_PROPERTY(int id MEMBER id)
    Q_PROPERTY(QString name MEMBER name)
    // ...
    Q_CLASSINFO("table", "products") // Optional: guessed from class name
public:
    Q_INVOKABLE QEloquent::Relation<Category> category() const {
        return belongsTo<Category>();
    }
};
```

### 🔗 Advanced Relationships
Go beyond simple tables. QEloquent supports a wide array of relationships:
- ✅ **One-to-One / One-to-Many**: `hasOne`, `hasMany`, `belongsTo`.
- ✅ **Many-to-Many**: `belongsToMany` with automatic pivot table management.
- ✅ **Through Relationships**: `hasManyThrough` and the powerful `belongsToManyThrough`.

### 🛠️ Fluent Query Builder
Stop writing raw SQL. Build queries with a beautiful, chainable API.

```cpp
auto query = Product::query()
    .where("price", ">", 100)
    .orderBy("name")
    .limit(10);

auto products = Product::find(query);
```

### 🚄 Performance & Eager Loading
Solve the N+1 problem before it starts. Use `with()` to eager-load relationships in a single go.

```cpp
auto query = Category::query().with("products.stock");
auto categories = Category::find(query);
```

---

## 📦 What's New in v1.0.0

- **Initial Stable API**: The core architecture is now solidified for production use.
- **Join Support**: Native `JOIN` operations within the Query builder.
- **Naming Conventions**: Fully customizable naming strategies (snake_case, camelCase, etc.).
- **Automatic Metadata**: Smart detection of table names and foreign keys based on conventions.
- **Comprehensive Test Suite**: 100% pass rate on integration tests covering all relationship types.

---

## 🚀 Getting Started

Simply include QEloquent in your CMake project:

```cmake
find_package(QEloquent REQUIRED)
target_link_libraries(your_app PRIVATE QEloquent::QEloquent)
```

Configure your connection:

```cpp
Connection::addConnection("default", "QSQLITE", "database.sqlite");
```

---

## 🤝 Contributing
We're just getting started! Feel free to open issues or pull requests on [GitHub](https://github.com/commander15/QEloquent).

---

*Made with ❤️ by the Amadou Benjamain.*
