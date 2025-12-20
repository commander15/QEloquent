# Connection Management {#connections}

QEloquent provides a wrapper around `QSqlDatabase` called @ref QEloquent::Connection. This class simplifies managing multiple database connections and handling transactions.

## Configuring Connections

You can add connections using static methods. The first connection added is usually treated as the "default" connection unless specified otherwise.

### Using a URL
```cpp
auto conn = Connection::addConnection("default", QUrl("sqlite:///path/to/db.sqlite"));
```

### Using Parameters
```cpp
Connection::addConnection("billing", "QPSQL", "billing_db", 5432, "user", "pass");
```

## Accessing Connections

Once configured, you can retrieve a connection by name anywhere in your app:

```cpp
Connection conn = Connection::connection("billing");
if (conn.open()) {
    // ...
}
```

## Transactions

QEloquent supports SQL transactions through the Connection class.

```cpp
Connection conn = Connection::defaultConnection();
if (conn.beginTransaction()) {
    bool success = doWork();
    if (success) {
        conn.commitTransaction();
    } else {
        conn.rollbackTransaction();
    }
}
```

## Model Connections

By default, all models use the "default" connection. You can specify a different connection per model using `Q_CLASSINFO`:

```cpp
struct LogEntry : public Model, public ModelHelpers<LogEntry> {
    Q_GADGET
    Q_CLASSINFO("connection", "logging")
    // ...
};
```

Learn how to build complex queries with the [Query Builder](@ref query_builder).
