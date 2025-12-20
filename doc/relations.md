# Relation Handling {#relations}

QEloquent handles relationships between models with a "User-Friendly" API that supports transparent lazy loading.

## Defining Relations

Define relations as methods in your model class using @ref QEloquent::Model::hasOne "hasOne", @ref QEloquent::Model::belongsTo "belongsTo", or @ref QEloquent::Model::hasMany "hasMany".

```cpp
struct Category : public Model, public ModelHelpers<Category> {
    // ...
    Relation<Product> products() const {
        return hasMany<Product>();
    }
};

struct Product : public Model, public ModelHelpers<Product> {
    // ...
    Relation<Category> category() const {
        return belongsTo<Category>();
    }
};
```

## Using the Friendly API

### Transparent Loading
You don't need to manually fetch related data. The first time you access a relation, QEloquent executes the database query for you.

### Range-based Loops
`hasMany` relations behave like standard C++ containers.

```cpp
for (const Product &product : category.products()) {
    qDebug() << "Product:" << product.name;
}
```

### Pointer-like Access
For 1:1 relations, use the `->` operator to access the related model directly.

```cpp
if (product.category()) {
    qDebug() << "Category:" << product.category()->name;
}
```

### Implicit Conversion
Assign a relation directly to a model variable to get the first related record.

```cpp
Category cat = product.category();
```

## Performance Note

While transparent loading is convenient, it can lead to N+1 query problems in loops. For bulk operations, use `load()` to eagerly fetch relationships:

```cpp
auto users = User::find(User::query().limit(10));
for (auto &user : users.value()) {
    user.load("posts"); // Eagerly loads all related posts in one query
}
```

See how to setup database connection in [Connection Management](@ref connections).
