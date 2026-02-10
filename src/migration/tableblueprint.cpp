#include "tableblueprint.h"
#include "tableblueprint_p.h"

#include <QEloquent/tabledata.h>
#include <QEloquent/connection.h>
#include <QEloquent/driver.h>
#include <QEloquent/schemagrammar.h>

#include <QSqlField>

namespace QEloquent {

using FieldType = TableBlueprintPrivate::FieldType;

/*!
 * \class TableBlueprint
 * \brief Fluent builder for defining database table structure.
 *
 * TableBlueprint provides a chainable, readable API for defining tables during
 * schema migrations (CREATE or ALTER). It collects column definitions, constraints,
 * indexes, and foreign keys, then generates the appropriate SQL via SchemaGrammar.
 *
 * Typical usage in migrations:
 * \code
 * Schema::create("users", [](TableBlueprint &table) {
 *     table.id();
 *     table.string("name", 100);
 *     table.foreignId("role_id").references("id").on("roles");
 *     table.foreignIdFor<UserGroup>("main_group_id");
 *     table.timestamps();
 * });
 * \endcode
 *
 * \sa Schema for creating/altering tables.
 * \sa ColumnDefinition for column configuration.
 * \sa TableData for read-only snapshot of the built structure.
 */

/*!
 * \brief Default constructor.
 *
 * Creates an empty table blueprint with no fields defined.
 * Internal private data is initialized.
 */
TableBlueprint::TableBlueprint()
    : data(new TableBlueprintPrivate())
{}

/*!
 * \brief Copy constructor.
 *
 * Creates a new table blueprint as a copy of another instance.
 * This is the default implementation (member-wise copy).
 */
TableBlueprint::TableBlueprint(const TableBlueprint &) = default;

/*!
 * \brief Move constructor.
 *
 * Creates a new table blueprint by moving from another instance.
 * This is the default implementation (member-wise move).
 */
TableBlueprint::TableBlueprint(TableBlueprint &&) = default;

/*!
 * \brief Copy assignment operator.
 *
 * Assigns the contents of another table blueprint to this instance.
 * This is the default implementation (member-wise copy assignment).
 *
 * \return A reference to this object.
 */
TableBlueprint &TableBlueprint::operator=(const TableBlueprint &) = default;

/*!
 * \brief Move assignment operator.
 *
 * Moves the contents of another table blueprint into this instance.
 * This is the default implementation (member-wise move assignment).
 *
 * \return A reference to this object.
 */
TableBlueprint &TableBlueprint::operator=(TableBlueprint &&) = default;

/*!
 * \brief Destructor.
 *
 * Destroys this table blueprint instance.
 * This is the default implementation.
 */
TableBlueprint::~TableBlueprint() = default;

/*!
 * \brief Adds an auto-incrementing primary key column.
 *
 * Creates a primary key column (usually named "id") with auto-increment
 * behavior, primary key constraint, and non-nullable property.
 *
 * \param name Optional custom name for the primary key column (defaults to "id").
 * \return A \c ColumnDefinition object for further customization.
 */
ColumnDefinition TableBlueprint::id(const QString &name)
{
    return idTyped(name).primaryKey().autoIncrement().nullable();
}

/*!
 * \brief Adds a UUID primary key column.
 *
 * Creates a 36-character unique string column suitable for UUIDs.
 *
 * \param name Name of the UUID column.
 * \return A \c ColumnDefinition object for further customization.
 */
ColumnDefinition TableBlueprint::uuid(const QString &name)
{
    return character(name, 36).unique();
}

/*!
 * \brief Adds a boolean column.
 *
 * \param name Name of the boolean column.
 * \return A \c ColumnDefinition object for further customization.
 */
ColumnDefinition TableBlueprint::boolean(const QString &name)
{
    return ColumnDefinition(data->fieldData(name, FieldType::Boolean));
}

/*!
 * \brief Adds an integer column.
 *
 * \param name Name of the integer column.
 * \param positive If \c true, adds UNSIGNED constraint.
 * \param autoIncrement If \c true, adds auto-increment behavior.
 * \return A \c ColumnDefinition object for further customization.
 */
ColumnDefinition TableBlueprint::integer(const QString &name, bool positive, bool autoIncrement)
{
    auto d = data->fieldData(name, Column::ColumnType::Integer);
    d->constraints.setFlag(Column::AutoIncrement, autoIncrement);
    d->numberSign = (positive ? Column::NumberSign::Unsigned : Column::NumberSign::Signed);
    return ColumnDefinition(d);
}

/*!
 * \brief Adds an unsigned integer column.
 *
 * Convenience overload equivalent to \c integer(name, autoIncrement, true).
 *
 * \param name Name of the unsigned integer column.
 * \return A \c ColumnDefinition object for further customization.
 * \param autoIncrement If \c true, adds auto-increment behavior.
 */
ColumnDefinition TableBlueprint::unsignedInteger(const QString &name, bool autoIncrement)
{
    return integer(name, autoIncrement, true);
}

/*!
 * \brief Adds a big integer column.
 *
 * \param name Name of the big integer column.
 * \param positive If \c true, adds UNSIGNED constraint.
 * \param autoIncrement If \c true, adds auto-increment behavior.
 * \return A \c ColumnDefinition object for further customization.
 */
ColumnDefinition TableBlueprint::bigInteger(const QString &name, bool positive, bool autoIncrement)
{
    auto d = data->fieldData(name, Column::ColumnType::BigInteger);
    d->constraints.setFlag(Column::AutoIncrement, autoIncrement);
    d->numberSign = (positive ? Column::NumberSign::Unsigned : Column::NumberSign::Signed);
    return ColumnDefinition(d);
}

/*!
 * \brief Adds an unsigned big integer column.
 *
 * Convenience overload equivalent to \c bigInteger(name, autoIncrement, true).
 *
 * \param name Name of the unsigned big integer column.
 * \param autoIncrement If \c true, adds auto-increment behavior.
 * \return A \c ColumnDefinition object for further customization.
 */
ColumnDefinition TableBlueprint::unsignedBigInteger(const QString &name, bool autoIncrement)
{
    return bigInteger(name, autoIncrement, true);
}

/*!
 * \brief Adds a tiny integer column.
 *
 * \param name Name of the tiny integer column.
 * \param autoIncrement If \c true, adds auto-increment behavior.
 * \param positive If \c true, adds UNSIGNED constraint.
 * \return A \c ColumnDefinition object for further customization.
 */
ColumnDefinition TableBlueprint::tinyInteger(const QString &name, bool positive, bool autoIncrement)
{
    auto d = data->fieldData(name, Column::ColumnType::TinyInteger);
    d->constraints.setFlag(Column::AutoIncrement, autoIncrement);
    d->numberSign = (positive ? Column::NumberSign::Unsigned : Column::NumberSign::Signed);
    return ColumnDefinition(d);
}

/*!
 * \brief Adds an unsigned tiny integer column.
 *
 * Convenience overload equivalent to \c tinyInteger(name, autoIncrement, true).
 *
 * \param name Name of the unsigned tiny integer column.
 * \param autoIncrement If \c true, adds auto-increment behavior.
 * \return A \c ColumnDefinition object for further customization.
 */
ColumnDefinition TableBlueprint::unsignedTinyInteger(const QString &name, bool autoIncrement)
{
    return tinyInteger(name, autoIncrement, true);
}

/*!
 * \brief Adds a double-precision floating-point column.
 *
 * \param name Name of the double column.
 * \return A \c ColumnDefinition object for further customization.
 */
ColumnDefinition TableBlueprint::doubleNumber(const QString &name)
{
    return ColumnDefinition(data->fieldData(name, Column::ColumnType::Double));
}

/*!
 * \brief Adds a floating-point column with specified precision.
 *
 * \param name Name of the float column.
 * \param precision The precision value (e.g., 53 for double-like).
 * \return A \c ColumnDefinition object for further customization.
 */
ColumnDefinition TableBlueprint::floatNumber(const QString &name, int precision)
{
    auto d = data->fieldData(name, Column::ColumnType::Float);
    d->floatPrecision = precision;
    return ColumnDefinition(d);
}

/*!
 * \brief Adds a decimal column with specified total digits and decimal places.
 *
 * \param name Name of the decimal column.
 * \param total Total number of digits (precision).
 * \param places Number of digits after the decimal point (scale).
 * \return A \c ColumnDefinition object for further customization.
 */
ColumnDefinition TableBlueprint::decimal(const QString &name, int total, int places)
{
    auto d = data->fieldData(name, Column::ColumnType::Decimal);
    d->length = total;
    d->decimalPlaces = places;
    return ColumnDefinition(d);
}

/*!
 * \brief Adds a fixed-length character column (CHAR).
 *
 * \param name Name of the character column.
 * \param length Fixed length of the column.
 * \return A \c ColumnDefinition object for further customization.
 */
ColumnDefinition TableBlueprint::character(const QString &name, int length)
{
    auto d = data->fieldData(name, Column::ColumnType::Char);
    d->length = length;
    return ColumnDefinition(d);
}

/*!
 * \brief Adds a variable-length string column (VARCHAR).
 *
 * \param name Name of the string column.
 * \param length Maximum length of the string.
 * \return A \c ColumnDefinition object for further customization.
 */
ColumnDefinition TableBlueprint::string(const QString &name, int length)
{
    auto d = data->fieldData(name, Column::ColumnType::String);
    d->length = length;
    return ColumnDefinition(d);
}

/*!
 * \brief Adds a date column (DATE).
 *
 * \param name Name of the date column.
 * \return A \c ColumnDefinition object for further customization.
 */
ColumnDefinition TableBlueprint::date(const QString &name)
{
    return ColumnDefinition(data->fieldData(name, Column::ColumnType::Date));
}

/*!
 * \brief Adds a time column (TIME).
 *
 * \param name Name of the time column.
 * \return A \c ColumnDefinition object for further customization.
 */
ColumnDefinition TableBlueprint::time(const QString &name)
{
    return ColumnDefinition(data->fieldData(name, Column::ColumnType::Time));
}

/*!
 * \brief Adds a date-time column (DATETIME).
 *
 * \param name Name of the date-time column.
 * \return A \c ColumnDefinition object for further customization.
 */
ColumnDefinition TableBlueprint::datetime(const QString &name)
{
    return ColumnDefinition(data->fieldData(name, Column::ColumnType::DateTime));
}

/*!
 * \brief Adds a timestamp column (TIMESTAMP).
 *
 * \param name Name of the timestamp column.
 * \return A \c ColumnDefinition object for further customization.
 */
ColumnDefinition TableBlueprint::timestamp(const QString &name)
{
    return ColumnDefinition(data->fieldData(name, FieldType::Timestamp));
}

/*!
 * \brief Adds standard timestamp columns (created_at and updated_at).
 *
 * Adds two timestamp columns with default names "created_at" and "updated_at".
 * The "updated_at" column is nullable by default.
 *
 * \param creation Name of the creation timestamp column (default: "created_at").
 * \param update Name of the update timestamp column (default: "updated_at").
 */
void TableBlueprint::timestamps(const QString &creation, const QString &update)
{
    timestamp(creation);
    timestamp(update).nullable();
}

/*!
 * \brief Adds a foreign key column referencing another table's primary key.
 *
 * Convenience method that creates an auto-incrementing ID column and
 * immediately returns a \c ForeignKeyDefinition for further configuration.
 *
 * \param name Name of the foreign key column (e.g., "user_id").
 * \return A \c ForeignKeyDefinition object for chaining (references, onUpdate, etc.).
 */
ForeignKeyDefinition TableBlueprint::foreignId(const QString &name)
{
    idTyped(name);
    return foreign(name);
}

/*!
 * \fn template<typename Model> ForeignKeyDefinition TableBlueprint::foreignIdFor()
 * \brief Adds a foreign key column using the convention for the referenced model.
 *
 * Automatically generates a foreign key column name based on the referenced
 * model's convention (typically "model_id" or "modelname_id").
 *
 * The column is created as an auto-incrementing primary key style ID,
 * then immediately converted to a foreign key constraint pointing to
 * the model's primary key column (usually "id").
 *
 * Example:
 * \code
 * table.foreignIdFor<User>();  // creates "user_id" → references users.id
 * \endcode
 *
 * \tparam Model The model class type (must derive from Model).
 * \return A \c ForeignKeyDefinition object for further configuration
 *         (e.g., .onDelete("cascade"), .references("custom_pk")).
 */

/*!
 * \fn template<typename Model> ForeignKeyDefinition TableBlueprint::foreignIdFor(const QString &name)
 * \brief Adds a foreign key column with custom name using the referenced model's convention.
 *
 * Similar to the overload without \a name, but allows specifying a custom
 * column name instead of using the automatic convention.
 *
 * The column is created as an auto-incrementing primary key style ID,
 * then converted to a foreign key constraint pointing to the model's
 * primary key column.
 *
 * Example:
 * \code
 * table.foreignIdFor<User>("owner_id");  // creates "owner_id" → references users.id
 * \endcode
 *
 * \tparam Model The model class type (must derive from Model).
 * \param name Custom name for the foreign key column.
 * \return A \c ForeignKeyDefinition object for further configuration.
 */

/*!
 * \brief Adds a foreign key constraint to an existing column.
 *
 * Returns a \c ForeignKeyDefinition for the specified column name.
 * If the column does not exist, returns an empty/invalid definition.
 *
 * \param name Name of the column to turn into a foreign key.
 * \return A \c ForeignKeyDefinition object for chaining.
 */
ForeignKeyDefinition TableBlueprint::foreign(const QString &name)
{
    if (!data->columnIndexes.contains(name))
        return ForeignKeyDefinition();
    return ForeignKeyDefinition(data->fieldData(name, ColumnData::Raw));
}

/*!
 * \brief Adds a raw SQL column definition.
 *
 * Allows defining a column using raw SQL syntax (bypassing fluent builder).
 * Useful for database-specific column types or complex definitions.
 *
 * \param name Name of the column.
 * \param definition Raw SQL definition string (e.g., "JSON NOT NULL").
 */
void TableBlueprint::rawColumn(const QString &name, const QString &definition)
{
    auto d = data->fieldData(name, Column::ColumnType::Raw);
    d->rawDefinition = definition;
}

/*!
 * \brief Returns the current table data snapshot.
 *
 * Provides a read-only view of the table definition as built so far.
 *
 * \return A \c TableData object containing the current table structure.
 */
TableData TableBlueprint::table() const
{
    return TableData(data);
}

/*!
 * \brief Static factory to create a new table blueprint.
 *
 * \param table Name of the table.
 * \param create \c true if this is a new table creation, \c false for alteration.
 * \param connectionName Optional connection name (defaults to current).
 * \return A new \c TableBlueprint instance.
 */
TableBlueprint TableBlueprint::create(const QString &table, bool create, const QString &connectionName)
{
    const Connection conn = Connection::connection(connectionName);
    const Driver *driver = conn.driver();
    TableBlueprintPrivate *data = new TableBlueprintPrivate(table, create, driver);
    return TableBlueprint(data);
}

/*!
 * \internal
 * \brief Internal helper to create a typed primary key column.
 *
 * Uses the driver's grammar to determine the correct type and sign for primary keys.
 *
 * \param name Name of the primary key column.
 * \return A \c ColumnDefinition object configured as primary key.
 */
ColumnDefinition TableBlueprint::idTyped(const QString &name)
{
    const SchemaGrammar *grammar = data->driver->schemaGrammar();
    const QPair<ColumnData::ColumnType, ColumnData::NumberSign> type = grammar->primaryKeyType();
    auto d = data->fieldData(name, type.first);
    d->numberSign = type.second;
    return ColumnDefinition(d);
}

TableBlueprint::TableBlueprint(TableBlueprintPrivate *data)
    : data(data)
{}

QExplicitlySharedDataPointer<ColumnDefinitionPrivate> &TableBlueprintPrivate::fieldData(const QString &name, FieldType type)
{
    if (columnIndexes.contains(name))
        return columns[columnIndexes.value(name)];

    ColumnDefinitionPrivate *data = new ColumnDefinitionPrivate(name, type, tableName);

    if (!newTable && record.contains(name)) {
        // Help distinguish new/old field
        data->oldColumn = name;
        const QSqlField field = record.field(name);
        data->constraints.setFlag(ColumnData::NotNull, field.requiredStatus() == QSqlField::Required);
        data->defaultValue = field.defaultValue();
    }

    columns.append(QExplicitlySharedDataPointer<ColumnDefinitionPrivate>(data));
    columnIndexes.insert(name, columns.size() - 1);
    return columns.last();
}

/*!
 * \class ColumnDefinition
 * \brief Fluent builder for defining a single database column during schema migrations.
 *
 * ColumnDefinition provides a chainable API to configure column properties such as type,
 * constraints, defaults, indexes, and check expressions. Instances are typically created
 * via \c TableBlueprint methods (e.g. \c table.string("email"), \c table.id(), etc.)
 * and returned for further modification.
 *
 * Example usage in a migration:
 * \code
 * Schema::create("users", [](TableBlueprint &table) {
 *     table.id();
 *     table.string("email", 255).unique().nullable().defaultValue("guest@example.com");
 *     table.integer("age").unsigned().check("age >= 13");
 *     table.foreignId("role_id").references("id").on("roles").onDelete("cascade");
 * });
 * \endcode
 *
 * \note
 * - All methods return a reference to \c *this for fluent chaining.
 * - Constraints and modifiers are accumulated and translated to SQL by SchemaGrammar.
 * - Some operations (e.g. auto-increment) are only valid for certain column types.
 *
 * \sa TableBlueprint for creating columns within table definitions.
 * \sa ForeignKeyDefinition for specialized foreign key columns.
 */

/*!
 * \brief Default constructor for column definition.
 *
 * Initializes an empty column definition with default private data.
 */
ColumnDefinition::ColumnDefinition()
    : data(new ColumnDefinitionPrivate())
{}

/*!
 * \brief Private constructor from raw private data pointer.
 *
 * Used internally when creating column definitions.
 *
 * \param data Pointer to the private implementation data.
 */
ColumnDefinition::ColumnDefinition(ColumnDefinitionPrivate *data)
    : data(data)
{}

/*!
 * \brief Constructs a column definition from shared private data.
 *
 * Used internally for sharing data between objects.
 *
 * \param data Shared pointer to existing private data.
 */
ColumnDefinition::ColumnDefinition(const QExplicitlySharedDataPointer<ColumnDefinitionPrivate> &data)
    : data(data)
{}

/*!
 * \brief Copy constructor.
 *
 * Creates a new column definition as a copy of another instance.
 * This is the default implementation (member-wise copy).
 */
ColumnDefinition::ColumnDefinition(const ColumnDefinition &) = default;

/*!
 * \brief Move constructor.
 *
 * Creates a new column definition by moving from another instance.
 * This is the default implementation (member-wise move).
 */
ColumnDefinition::ColumnDefinition(ColumnDefinition &&) = default;

/*!
 * \brief Copy assignment operator.
 *
 * Assigns the contents of another column definition to this instance.
 * This is the default implementation (member-wise copy assignment).
 *
 * \return A reference to this object.
 */
ColumnDefinition &ColumnDefinition::operator=(const ColumnDefinition &) = default;

/*!
 * \brief Move assignment operator.
 *
 * Moves the contents of another column definition into this instance.
 * This is the default implementation (member-wise move assignment).
 *
 * \return A reference to this object.
 */
ColumnDefinition &ColumnDefinition::operator=(ColumnDefinition &&) = default;

/*!
 * \brief Destructor.
 *
 * Destroys this column definition instance.
 * This is the default implementation.
 */
ColumnDefinition::~ColumnDefinition() = default;

/*!
 * \brief Marks this column as the primary key.
 *
 * Sets or clears the primary key constraint flag.
 *
 * \param enable \c true to set as primary key, \c false to remove.
 * \return Reference to this object (for fluent chaining).
 */
ColumnDefinition &ColumnDefinition::primaryKey(bool enable)
{
    data->constraints.setFlag(ColumnData::PrimaryKey, enable);
    return *this;
}

/*!
 * \brief Marks this column as auto-incrementing.
 *
 * Sets the auto-increment flag (typically for primary keys).
 *
 * \return Reference to this object (for fluent chaining).
 */
ColumnDefinition &ColumnDefinition::autoIncrement()
{
    data->constraints.setFlag(ColumnData::AutoIncrement, true);
    return *this;
}

/*!
 * \brief Adds a regular index on this column.
 *
 * Creates an index with a default name ("idx_" + column name).
 *
 * \return Reference to this object (for fluent chaining).
 */
ColumnDefinition &ColumnDefinition::index()
{
    const QString &name = "idx_" + data->columnName;
    return index(name);
}

/*!
 * \brief Enables or disables indexing on this column.
 *
 * If \a enable is \c true, adds a regular index with default name.
 * If \c false, removes any existing index name.
 *
 * \param enable \c true to add index, \c false to remove.
 * \return Reference to this object (for fluent chaining).
 */
ColumnDefinition &ColumnDefinition::index(bool enable)
{
    if (enable) {
        const QString &name = "idx_" + data->columnName;
        return index(name);
    } else {
        return index(QString());
    }
}

/*!
 * \brief Adds a named index on this column.
 *
 * Sets a custom index name. Passing an empty string removes indexing.
 *
 * \param name Custom name for the index, or empty to remove.
 * \return Reference to this object (for fluent chaining).
 */
ColumnDefinition &ColumnDefinition::index(const QString &name)
{
    data->indexName = name;
    return *this;
}

/*!
 * \brief Marks this column as unique.
 *
 * Sets or clears the unique constraint flag.
 *
 * \param enable \c true to set unique constraint, \c false to remove.
 * \return Reference to this object (for fluent chaining).
 */
ColumnDefinition &ColumnDefinition::unique(bool enable)
{
    data->constraints.setFlag(ColumnData::Unique, enable);
    return *this;
}

/*!
 * \brief Sets whether the column allows NULL values.
 *
 * \param enable \c true to allow NULL (nullable), \c false to disallow (NOT NULL).
 * \return Reference to this object (for fluent chaining).
 */
ColumnDefinition &ColumnDefinition::nullable(bool enable)
{
    data->constraints.setFlag(ColumnData::NotNull, !enable);
    return *this;
}

/*!
 * \brief Sets a literal default value for the column.
 *
 * The value is used as-is in SQL (e.g., DEFAULT 0, DEFAULT 'active').
 * Sets the default flag and clears expression mode.
 *
 * \param value The literal default value (QVariant).
 * \return Reference to this object (for fluent chaining).
 */
ColumnDefinition &ColumnDefinition::defaultValue(const QVariant &value)
{
    data->defaultValue = value;
    data->defaultValueIsExpr = false;
    data->constraints.setFlag(ColumnData::Default, true);
    return *this;
}

/*!
 * \brief Sets a SQL expression as the default value.
 *
 * The expression is used verbatim (e.g., DEFAULT CURRENT_TIMESTAMP).
 * Sets the default flag and expression mode.
 *
 * \param expr SQL expression for the default value.
 * \return Reference to this object (for fluent chaining).
 */
ColumnDefinition &ColumnDefinition::defaultExpression(const QString &expr)
{
    data->defaultValue = expr;
    data->defaultValueIsExpr = true;
    data->constraints.setFlag(ColumnData::Default, true);
    return *this;
}

/*!
 * \brief Adds a minimum value check constraint.
 *
 * Adds a CHECK constraint ensuring values are >= \a value.
 * Null values are ignored.
 *
 * \param value Minimum allowed value.
 * \return Reference to this object (for fluent chaining).
 */
ColumnDefinition &ColumnDefinition::min(const QVariant &value)
{
    if (value.isNull()) return *this;
    data->minValue = value;
    data->constraints.setFlag(ColumnData::Check, true);
    return *this;
}

/*!
 * \brief Adds a maximum value check constraint.
 *
 * Adds a CHECK constraint ensuring values are <= \a value.
 * Null values are ignored.
 *
 * \param value Maximum allowed value.
 * \return Reference to this object (for fluent chaining).
 */
ColumnDefinition &ColumnDefinition::max(const QVariant &value)
{
    if (value.isNull()) return *this;
    data->maxValue = value;
    data->constraints.setFlag(ColumnData::Check, true);
    return *this;
}

/*!
 * \brief Adds a range check constraint (min to max).
 *
 * Adds a CHECK constraint ensuring values are between \a min and \a max.
 * Null values are ignored.
 *
 * \param min Minimum allowed value.
 * \param max Maximum allowed value.
 * \return Reference to this object (for fluent chaining).
 */
ColumnDefinition &ColumnDefinition::range(const QVariant &min, const QVariant &max)
{
    if (!min.isValid() || !max.isValid()) return *this;
    data->minValue = min;
    data->maxValue = max;
    data->constraints.setFlag(ColumnData::Check, true);
    return *this;
}

/*!
 * \brief Adds an IN-list check constraint.
 *
 * Adds a CHECK constraint ensuring values are in the provided list.
 *
 * \param values List of allowed values.
 * \return Reference to this object (for fluent chaining).
 */
ColumnDefinition &ColumnDefinition::in(const QVariantList &values)
{
    data->inValues.append(values);
    data->constraints.setFlag(ColumnData::Check, true);
    return *this;
}

/*!
 * \brief Adds a custom CHECK constraint expression.
 *
 * Adds a raw SQL CHECK constraint.
 * Multiple calls append expressions with AND.
 *
 * \param expr SQL expression for the check constraint.
 * \return Reference to this object (for fluent chaining).
 */
ColumnDefinition &ColumnDefinition::check(const QString &expr)
{
    if (expr.isEmpty()) return *this;
    data->checkExpr.append(expr);
    data->constraints.setFlag(ColumnData::Check, true);
    return *this;
}

/*!
 * \class ForeignKeyDefinition
 * \brief Specialized column definition for foreign key constraints.
 *
 * ForeignKeyDefinition extends \c ColumnDefinition to define foreign key
 * relationships, including the referenced table, column(s), and actions
 * on update/delete (ON UPDATE / ON DELETE).
 *
 * Instances are typically created via \c TableBlueprint::foreignId() or similar
 * fluent methods during schema definition.
 *
 * \sa ColumnDefinition for base column properties.
 * \sa TableBlueprint for creating foreign keys in migrations.
 */

/*!
 * \enum ForeignKeyDefinition::ForeignKeyAction
 * \brief Defines the action to take when a referenced row is updated or deleted.
 *
 * These actions correspond to the SQL `ON UPDATE` and `ON DELETE` clauses
 * in foreign key constraints.
 *
 * The behavior controls what happens to child records when the parent row
 * is modified or removed.
 */

/*!
 * \var ForeignKeyDefinition::NoAction
 * \brief No action is taken (default database behavior).
 *
 * The database performs its default action (usually restrict or error).
 */

/*!
 * \var ForeignKeyDefinition::Restrict
 * \brief Prevents the update/delete if any child records exist.
 *
 * Raises an error if the operation would violate referential integrity.
 */

/*!
 * \var ForeignKeyDefinition::Cascade
 * \brief Propagates the update/delete to all child records.
 *
 * If the parent is deleted → all referencing child rows are deleted.
 * If the parent key is updated → all child foreign keys are updated.
 */

/*!
 * \var ForeignKeyDefinition::SetNull
 * \brief Sets the foreign key column(s) to NULL in child records.
 *
 * On parent update/delete, all matching child foreign keys become NULL.
 * Requires the foreign key column(s) to allow NULL values.
 */

/*!
 * \brief Default constructor.
 *
 * Creates an empty foreign key definition with default values.
 * Internal private data is initialized.
 */
ForeignKeyDefinition::ForeignKeyDefinition()
    : ColumnDefinition(new ColumnDefinitionPrivate())
{}

/*!
 * \brief Constructs a foreign key definition from shared private data.
 *
 * Used internally when sharing private implementation data with the base class.
 *
 * \param data Shared pointer to the existing private data.
 */
ForeignKeyDefinition::ForeignKeyDefinition(const QExplicitlySharedDataPointer<ColumnDefinitionPrivate> &data)
    : ColumnDefinition(data)
{}

/*!
 * \brief Copy constructor.
 *
 * Creates a new foreign key definition as a copy of another instance.
 * This is the default implementation (member-wise copy).
 */
ForeignKeyDefinition::ForeignKeyDefinition(const ForeignKeyDefinition &) = default;

/*!
 * \brief Move constructor.
 *
 * Creates a new foreign key definition by moving from another instance.
 * This is the default implementation (member-wise move).
 */
ForeignKeyDefinition::ForeignKeyDefinition(ForeignKeyDefinition &&) = default;

/*!
 * \brief Copy assignment operator.
 *
 * Assigns the contents of another foreign key definition to this instance.
 * This is the default implementation (member-wise copy assignment).
 *
 * \return A reference to this object.
 */
ForeignKeyDefinition &ForeignKeyDefinition::operator=(const ForeignKeyDefinition &) = default;

/*!
 * \brief Move assignment operator.
 *
 * Moves the contents of another foreign key definition into this instance.
 * This is the default implementation (member-wise move assignment).
 *
 * \return A reference to this object.
 */
ForeignKeyDefinition &ForeignKeyDefinition::operator=(ForeignKeyDefinition &&) = default;

/*!
 * \brief Destructor.
 *
 * Destroys this foreign key definition instance.
 * This is the default implementation.
 */
ForeignKeyDefinition::~ForeignKeyDefinition() = default;

/*!
 * \brief Sets the referenced table name.
 *
 * Specifies the parent table that this foreign key references.
 *
 * \param table Name of the referenced table.
 * \return Reference to this object (for fluent chaining).
 */
ForeignKeyDefinition &ForeignKeyDefinition::on(const QString &table)
{
    data->refTable = table;
    return *this;
}

/*!
 * \brief Sets the referenced column name (single-column foreign key).
 *
 * Specifies the column in the referenced table that this foreign key points to.
 * Can be called multiple times to build composite foreign keys.
 *
 * \param column Name of the referenced column.
 * \return Reference to this object (for fluent chaining).
 */
ForeignKeyDefinition &ForeignKeyDefinition::references(const QString &column)
{
    data->refColumns.append(column);
    return *this;
}

/*!
 * \brief Sets multiple referenced columns (composite foreign key).
 *
 * Appends the list of columns in the referenced table.
 * Useful for foreign keys spanning multiple columns.
 *
 * \param columns List of column names in the referenced table.
 * \return Reference to this object (for fluent chaining).
 */
ForeignKeyDefinition &ForeignKeyDefinition::references(const QStringList &columns)
{
    data->refColumns.append(columns);
    return *this;
}

/*!
 * \brief Sets the action to perform when the referenced row is updated.
 *
 * Defines the ON UPDATE behavior for the foreign key constraint.
 *
 * \param action The desired action (e.g., Cascade, SetNull, Restrict, NoAction).
 * \return Reference to this object (for fluent chaining).
 *
 * \sa onDelete() for delete behavior.
 */
ForeignKeyDefinition &ForeignKeyDefinition::onUpdate(ForeignKeyAction action)
{
    data->onUpdate = action;
    return *this;
}

/*!
 * \brief Sets the action to perform when the referenced row is deleted.
 *
 * Defines the ON DELETE behavior for the foreign key constraint.
 *
 * \param action The desired action (e.g., Cascade, SetNull, Restrict, NoAction).
 * \return Reference to this object (for fluent chaining).
 *
 * \sa onUpdate() for update behavior.
 */
ForeignKeyDefinition &ForeignKeyDefinition::onDelete(ForeignKeyAction action)
{
    data->onDelete = action;
    return *this;
}

} // namespace QEloquent
