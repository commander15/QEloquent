# Query Builder {#query_builder}

The @ref QEloquent::Query builder provides a powerful, fluent interface for constructing SQL queries. It handles the generation of SQL syntax according to the specific database driver.

## Initializing a Query

Start a query from your model class:

```cpp
auto query = Product::query();
```

## Filtering (WHERE)

### Simple Equality
```cpp
query.where("category_id", 5);
```

### Custom Operators
```cpp
query.where("price", ">=", 100);
```

### Logical AND/OR
```cpp
query.where("active", true)
     .orWhere("price", "<", 10);
```

## Ordering and Grouping

### Order By
```cpp
query.orderBy("created_at", Qt::DescendingOrder);
```

### Group By
```cpp
query.groupBy("category_id");
```

## Pagination

### Limit and Offset
```cpp
query.limit(10).offset(20);
```

### Page Helper
```cpp
query.page(2, 50); // Page 2, 50 per page (Offset 50, Limit 50)
```

## Eager Loading (Eager Relations)

To prevent N+1 query problems, you can tell the query to automatically fetch related models:

```cpp
auto result = Product::find(Product::query().with("category"));
```

Next, see the [Utility functions](@ref utilities) available in QEloquent.
