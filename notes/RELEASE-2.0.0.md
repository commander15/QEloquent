# QEloquent v2.0.0 🚀
*Qt's most expressive, Eloquent-like C++20 ORM framework.*

We are beyond thrilled to announce the release of **QEloquent v2.0.0**! This major milestone elevates QEloquent to a whole new level, bringing a seamless, Laravel-inspired developer experience directly into the C++20 and Qt6 ecosystem. If you love Laravel's Eloquent, writing data-driven desktop and mobile apps in C++ will now feel instantly familiar and delightful.

---

## 🌟 Major Highlights in v2.0.0

### 1. True Eloquent-Style Fluent Queries (`QueryHelper`)
The brand new `QueryHelper` brings the elegance and ergonomics of active-record fluent querying to C++. Chain your constraints with expressive readability:
* **Rich Filtering & Joins**: Full-featured forwarding for `where`, `andWhere`, `orWhere`, `whereRaw`, standard and inner joins, group-bys, and sorting.
* **Strict Lookups (`first()`)**: No silent failures or dummy models. Calling `first()` cleanly returns an explicit `Error::NotFoundError` when a record doesn't exist, letting you handle missing data with robust monadic `Result` types.
* **Developer Diagnostics**: Debugging is a breeze with built-in `dump()` and `dumpAndDie()` utilities leveraging `std::source_location` for exact file and line precision.

```cpp
// Feels just like Laravel Eloquent, but in modern C++20!
Result<QList<User>, Error> activeAdmins = User::query()
    .where("status", "active")
    .andWhere("role", "admin")
    .orderBy("created_at", Qt::DescendingOrder)
    .limit(10)
    .get();

// Single record ? No problems
Result<User, Error> firstUser = User::find(1);

// Want more Laravel like ? Let's go !
Result<QList<User>, Error> activeGuests = User::where("status", "active")
    .andWhere("role", "guest");
    
// Eager load from start ? Let's do it
Result<QList<User>, Error> profiles = User::with("profile")->get();

// Pagination needed ? Fo for it
Result<QList<User>, Error> paginated = User::where("status", "active")
    .paginate(1, 25); // Page 1, 25 per page, 20 items per page (default)
    
// You can now delete in bulk without retrieving
Result<int, Error> deleted = User::where("status", "inactive").deleteData();

// And there is even moren just looks at QEloquent::ModelHelpers in doc !

```

### 2. RAII-Based `TransactionGuard`

Say goodbye to manual transaction boilerplate and uncommitted database locks. The new `TransactionGuard` acts as an RAII guard (`QMutexLocker` style) for your database transactions:

* Automatically begins a transaction upon construction.
* Automatically triggers a **rollback** on destruction if `.commit()` was never called—protecting your database against early returns and exceptions.
* Exposes explicit error and state diagnostics (`transactionStarted()`, `errorString()`, and `errorCode()`).

```cpp
bool processCheckout() {
    TransactionGuard guard;
    if (!guard.transactionStarted()) {
        qDebug() << "Transaction failed to start:" << guard.errorString();
        return false;
    }
    
    if (!chargeCustomer() || !fulfillOrder()) {
        return false; // Scope exits -> destructor automatically rolls back!
    }
    
    if (!quard.commit()) {
        qDebug() << "Transaction failed to commit:" << guard.errorString();
        return false;
    }
    
    return true;
}

```

### 3. Polymorphic Primary Keys (`ID`)

* Seamlessly handle different primary key strategies out of the box with the `ID` class.
* First-class support for **64-bit Integers**, **Strings**, and **UUIDs** with type-safe accessors (`integer()`, `string()`, `uuid()`).

### 4. Smart Model Auditing (`SmartModel`)

* Built on top of the old Model class, it features somme common fields built-in.
* Leverage the brand new ID type to propose polymorphic id field, wheter it's integer, string or uuid based pk, this class handle all for you and is able to generate UUIDv7 automatically on save.
* Automatically provisions and tracks auditing timestamps: `createdAt`, `updatedAt`, and `deletedAt` for robust soft-deletion capabilities.

---

## 📦 Installation & Integration

Integrate QEloquent v2.0.0 into your CMake project:

```cmake
find_package(QEloquent REQUIRED)

target_link_libraries(YourApp PRIVATE QEloquent::QEloquent)

```

---

## 📋 Full Changelog (v2.0.0)

* **Architectural Leap**: Promoted framework to version `2.0.0` with polished fluent query semantics.
* **Query Layer**: Introduced `QueryHelper` with robust pagination, eager relation loading (`with`), and strict `Error::NotFoundError` handling on `first()`.
* **Diagnostics**: Integrated `std::source_location` into `dumpAndDie()` for precise source-code tracing.
* **Safety**: Added RAII `TransactionGuard` with automatic rollback on scope abandonment and granular error code extraction.
* **Documentation**: Generated complete, modern Doxygen sidebar documentation.

---

*Thank you for building with QEloquent! Explore the [Documentation Hub](https://www.google.com/search?q=MainPage) for comprehensive guides and class references.*
