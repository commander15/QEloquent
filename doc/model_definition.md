# Model Definition {#model_definition}

In QEloquent, a **Model** is a C++ class or struct that maps to a database table. By using Qt's meta-object system, you can define your schema once and have it automatically reflected in the ORM.

## Basic Structure

To define a model in QEloquent, inherit from @ref QEloquent::Model and @ref QEloquent::ModelHelpers (the latter unlocks all the powerful static methods like *find()*, *all()*, *create()*, *etc*.).

QEloquent heavily leverages Qt's meta-object system, so most configuration is done via **Q_PROPERTY** and **Q_CLASSINFO**. This gives you a zero-config experience by default, while still offering full control when needed. 



```cpp
#include <QEloquent/model.h>
#include <QEloquent/modelhelpers.h>

struct User : public QEloquent::Model, public QEloquent::ModelHelpers<User>
{
    Q_GADGET

    // Core fields — mapped directly to database columns
    Q_PROPERTY(int id MEMBER id)
    Q_PROPERTY(QString name MEMBER name)
    Q_PROPERTY(QString email MEMBER email)
    Q_PROPERTY(QString password MEMBER password)

    // Optional: Explicit table name.
    // By default, QEloquent converts the class name to to an appropriate table name
    // according to the naming convention set.
    // For Laravel naming convention, we can have: "User" → "users".
    Q_CLASSINFO("table", "users")

    // Optional: Naming convention.
    // "Laravel" (default): class name → snake_case + pluralized for table name,
    // property name → snake_case for column name.
    // "OneOne" alternative: property name = column name exactly,
    // table name = class name pluralized.
    Q_CLASSINFO("naming", "Laravel")

    // Optional: Hide fields from serialization (e.g., JSON output).
    // Use property names here — the corresponding database fields will be omitted
    // from serialized output.
    // If the properties are not regular (Q_PROPERTY based), they are considered as
    // dynamic properties so don't need to worry if they are used for relation resolution
    // They don't appear during serialization but they are still there and accessible 
    // using property() method.
    Q_CLASSINFO("hidden", "password, roleId")

    // Optional: Mass-assignable (fillable) properties.
    // By default, all writable Q_PROPERTY and dynamic properties are fillable.
    // Restricting this protects against mass-assignment vulnerabilities.
    // Note: only regular writable and dynamic properties supported
    Q_CLASSINFO("fillable", "name, email, password")

    // Optional: Computed/appended attributes.
    // These are not stored in the database but are included in serialization and debug output.
    // The key name in output will follow the naming convention (e.g., snake_case) if no
    // override had been provided (see note below).
    // Requires a parameterless Q_INVOKABLE accessor with the same name.
    Q_CLASSINFO("append", "emailHeader")

    // Optional: Eager loading.
    // Relations listed here are automatically loaded whenever the model is retrieved.
    // Requires a parameterless Q_INVOKABLE relation method with the same name.
    Q_CLASSINFO("with", "role")

    // Required when inheriting from a model that already includes ModelHelpers.
    // Resolves potential static method ambiguity.
    QELOQUENT_MODEL_HELPERS(User)

public:
    // Database-mapped fields
    int id = 0;
    QString name;
    QString email;
    QString password;

    // Appended (computed) attribute
    Q_INVOKABLE QString emailHeader() const {
        // This value will appear in JSON/debug output thanks to "append"
        return name + " <" + email + ">";
    }

    // One-to-one / many-to-one relation
    Q_INVOKABLE QEloquent::Relation<UserRole> role() const {
        // Parameters are optional — QEloquent can often infer them automatically
        return belongsTo<UserRole>("role_id", "id");
    }

    // Many-to-many relation
    Q_INVOKABLE QEloquent::Relation<UserGroup> groups() const {
        // Pivot table and key names — also often auto-deducible
        return belongsToMany<UserGroup>("UserGroupMembers", "user_id", "group_id");
    }
};
```

### Additional note

- For any property (standard **Q_PROPERTY**, appended attribute, or relation), you can manually override the database field name used for serialization using **Q_CLASSINFO**("<propertyName>Field", "custom_field_name").
  This bypasses the naming convention entirely for that specific property.

- Both append and with require a matching parameterless **Q_INVOKABLE** method with the exact same name as the entry.
  QEloquent uses the Qt meta-system to discover and call these automatically.



This approach keeps your models *clean*, *expressive*, and *highly configurable* without boilerplate.



Once your model is defined, you can start using it for [CRUD operations](@ref usage).
