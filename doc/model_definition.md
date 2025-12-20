# Model Definition {#model_definition}

In QEloquent, a **Model** is a C++ class or struct that maps to a database table. By using Qt's meta-object system, you can define your schema once and have it automatically reflected in the ORM.

## Basic Structure

A model inherits from @ref QEloquent::Model and @ref QEloquent::ModelHelpers. It must use the `Q_GADGET` macro and define properties using `Q_PROPERTY`.

```cpp
#include <QEloquent/model.h>
#include <QEloquent/modelhelpers.h>

struct Product : public QEloquent::Model, public QEloquent::ModelHelpers<Product>
{
    Q_GADGET
    // Database columns are mapped to properties
    Q_PROPERTY(int id MEMBER id)
    Q_PROPERTY(QString name MEMBER name)
    Q_PROPERTY(double price MEMBER price)

    // Optional: Table name customization (defaults to pluralized class name)
    Q_CLASSINFO("table", "products")
    
public:
    int id;
    QString name;
    double price;
};
```

## Naming Conventions

- **Table Names**: By default, QEloquent pluralizes your class name (e.g., `User` -> `users`). You can override this using `Q_CLASSINFO("table", "my_table")`.
- **Primary Keys**: The property named `id` is automatically treated as the primary key. You can specify a different one using `Q_CLASSINFO("primary_key", "uuid")`.
- **Connection**: Specify a non-default database connection name with `Q_CLASSINFO("connection", "my_connection")`.

## Requirements

1. **Q_GADGET**: Essential for the ORM to "see" your properties.
2. **Q_PROPERTY**: Only members declared as properties are persisted to the database.
3. **ModelHelpers**: Enables static methods like `find()`, `create()`, and `query()`.

Once your model is defined, you can start using it for [CRUD operations](@ref usage).
