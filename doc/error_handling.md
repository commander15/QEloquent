# Error Handling {#error_handling}

Database operations are inherently risky. Instead of using exceptions, QEloquent uses a type-safe `Result` pattern to communicate success or failure.

## The Result Class

Most static methods return a `Result<T, E>` object. 

- **Success**: The object contains a value of type `T`.
- **Failure**: The object contains an `Error` object.

```cpp
auto result = User::find(1);

if (result) {
    // Access the value
    User user = result.value();
} else {
    // Access the error
    Error err = result.error();
    qWarning() << "Database Error:" << err.text();
}
```

## The Error Class

The `Error` object provides details about what went wrong:

- `type()`: Categorizes the error (e.g., DatabaseError, NotFound).
- `text()`: A human-readable description.
- `sqlError()`: Access to the underlying `QSqlError` if available.

```cpp
if (!result) {
    if (result.error().type() == Error::NotFound) {
        qDebug() << "The record does not exist.";
    }
}
```

## Instance Errors

For instance methods like `save()`, which return a `bool`, you can retrieve the last error from the model itself:

```cpp
if (!user.save()) {
    qWarning() << "Save failed:" << user.lastError().text();
}
```

See how to model complex data structures in [Relationship Handling](@ref relations).
