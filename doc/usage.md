# Simple Usage {#usage}

QEloquent provides a fluent API for standard CRUD (Create, Read, Update, Delete) operations.

## Creating Records

Use the static `create()` method to quickly persist data.

```cpp
QJsonObject data;
data["name"] = "Smartphone";
data["price"] = 699.99;

auto result = Product::create(data);
if (result) {
    Product p = result.value();
    qDebug() << "Created product with ID:" << p.id;
}
```

## Reading Records

### Finding by Primary Key
```cpp
auto result = Product::find(1);
if (result) {
    Product p = result.value();
    qDebug() << "Product Name:" << p.name;
}
```

### Using the Query Builder
The Query Builder allows for chainable filtering.

```cpp
auto query = Product::query()
    .where("price", ">", 500)
    .orderBy("name", Qt::AscendingOrder);

auto result = Product::find(query);
if (result) {
    QList<Product> products = result.value();
    // ...
}
```

## Updating Records

Modify a model instance and call `save()`.

```cpp
auto result = Product::find(1);
if (result) {
    Product p = result.value();
    p.price = 599.99;
    if (p.save()) {
        qDebug() << "Update successful";
    }
}
```

## Deleting Records

### Instance Deletion
```cpp
product.deleteData();
```

### Static Deletion
```cpp
Product::remove(Product::query().where("id", 1));
```

Learn more about handling potential database issues in [Error Handling](@ref error_handling).
