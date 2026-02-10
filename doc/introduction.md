# QEloquent

**Modern, fluent ORM for Qt 6 / C++ — inspired by Laravel Eloquent**

QEloquent brings the productivity and elegance of Eloquent-style ORMs to Qt applications — without sacrificing performance, type-safety, or Qt-native feel.



## ✨ Features

- Fluent model definitions with Q_PROPERTY + Q_CLASSINFO
- Zero-config defaults (table names, primary keys, timestamps)
- Full relationship support: hasOne, belongsTo, hasMany, belongsToMany
- Eager loading (`with()`) & lazy loading
- Beautiful debug output (`qDebug() << model` → nested YAML-like)
- Laravel-like migrations (fluent builder + SQL script support)
- Idempotent seeding with progress monitoring
- Serialization / deserialization (JSON, YAML, CSV, QDataStream, QSqlRecord)
- Cross-platform & multi-database (SQLite, MySQL/MariaDB, PostgreSQL coming)

 

## Why QEloquent?

| Feature                | Raw QSqlQuery / QSql*     | QEloquent                 | Why it matters                  |
| ---------------------- | ------------------------- | ------------------------- | ------------------------------- |
| Readable queries       | String concatenation hell | Fluent chaining           | Less bugs, easier maintenance   |
| Relationships          | Manual joins & mapping    | hasOne / hasMany / with() | No N+1 queries, clean code      |
| Migrations             | Raw SQL files             | Fluent builder + scripts  | Versioned schema, safe rollback |
| Seeding                | Manual inserts            | Idempotent seeders        | Safe dev/test/prod seeding      |
| Debugging              | qDebug() << query         | YAML-like model output    | Instant insight into data       |
| Startup time (repeat)  | Varies                    | ~1–4 ms                   | Instant app feel                |
| Size (deployed Qt app) | Minimal                   | +60–90 MB                 | Acceptable trade-off            |

## 60-second example

```cpp
// User model
struct User : public StandardModel<User>, public ModelHelpers<User>
{
    Q_GADGET
    Q_PROPERTY(QString name MEMBER name)
    Q_PROPERTY(QString email MEMBER email)

    Q_CLASSINFO("fillable", "name,email")
    Q_CLASSINFO("hidden", "password")

public:
    QString name;
    QString email;

    Q_INVOKABLE Relation<Profile> profile() const { return hasOne<Profile>(); }
};

// Migration
Schema::create("users", TableBlueprint &table {
    table.id();
    table.string("name", 100);
    table.string("email", 255).unique();
    table.timestamps();
});

// Usage
User user;
user.name = "Amadou";
user.email = "amadou@example.com";
user.save();

auto users = User::with("profile").get();
qDebug() << users.first();   // → beautiful YAML-like output
```

## Quick links

- Getting Started -> Installation and first model
- Models → Defining properties, fillable, relations
- Querying → Fluent queries, scopes, pagination
- Relationships → hasOne, hasMany, belongsToMany, eager loading
- Migrations → Schema API, createTable, script files
- Seeding → Writing & running seeders
- Serialization → toJson, toYaml, from QSqlRecord
- Advanced → Multiple DBs, custom drivers, performance
- FAQ → Common questions & troubleshooting
- API Reference → Full Doxygen docs (link to generated HTML)

## 

## Contributing & feedback

Star the repo, open issues, or send pull requests — every bit helps.
Repository: https://github.com/commander15/QEloquent
Issues / discussions: https://github.com/commander15/QEloquent/issues



## 📄 Licence

QEloquent is licenced under GNU LGPLv3.



**Happy coding!**
