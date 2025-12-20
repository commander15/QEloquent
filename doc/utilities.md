# Utilities {#utilities}

QEloquent includes several utility classes and patterns to improve your development experience.

## Result<T, E> Pattern

To avoid exceptions and provide clear error handling, QEloquent uses the `Result` type alias (based on `std::expected`).

```cpp
Result<User, Error> result = User::find(1);

if (result) {
    User user = result.value();
} else {
    qWarning() << result.error().text();
}
```

## Naming Conventions

The @ref QEloquent::NamingConvention class manages the translation between your C++ code and your database schema.

- **LaravelConvention (Default)**: Follows Laravel/Eloquent conventions (pluralized tables, `id` primary keys, `model_id` foreign keys).
- **Custom Conventions**: You can implement your own `NamingConvention` subclass to match existing legacy databases.

```cpp
// Change global convention
NamingConvention::setDefault(new MyCustomConvention());
```

## Global Macros

QEloquent uses several macros for export and configuration:

- `QELOQUENT_EXPORT`: Handles library symbols for shared building.
- `Q_GADGET`: Used in models for reflection.
- `Q_PROPERTY`: Maps class members to database columns.

Return to the [Introduction](@ref intro).
