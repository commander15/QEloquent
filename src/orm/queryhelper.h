#ifndef QELOQUENT_QUERYHELPER_H
#define QELOQUENT_QUERYHELPER_H

#include <QEloquent/query.h>
#include <QEloquent/querybuilder.h>
#include <QEloquent/queryrunner.h>
#include <QEloquent/modelmaker.h>
#include <QEloquent/result.h>

#include <QSqlQuery>
#include <QSqlRecord>

#include <source_location>

namespace QEloquent {

/**
 * @brief Fluent query builder and execution proxy for QEloquent models.
 *
 * @tparam Model The model class type being queried (e.g., `User`, `Product`).
 * @tparam Maker The factory policy class used to instantiate models and fetch metadata (defaults to `ModelMaker<Model>`).
 *
 * The `QueryHelper` class serves as the backbone of QEloquent's fluent query API.
 * It forwards query constraints (such as `where`, `join`, `orderBy`, etc.) down to
 * the underlying `Query` builder while exposing execution methods (`get`, `first`, `count`,
 * `paginate`, `deleteData`) that map database rows directly back into strongly typed model instances
 * with automatic relationship loading.
 */
template<typename Model, typename Maker = ModelMaker<Model>>
class QueryHelper
{
public:
    /**
     * @brief Constructs a default query helper initialized with model table bindings.
     */
    QueryHelper() { fixQuery(m_query); }

    /**
     * @brief Constructs a query helper wrapping an existing lvalue query.
     * @param query The `Query` configuration object to wrap.
     */
    QueryHelper(const Query &query) : m_query(query) {}

    /**
     * @brief Constructs a query helper wrapping an existing rvalue query.
     * @param query The rvalue `Query` configuration object to move from.
     */
    QueryHelper(Query &&query) : m_query(std::move(query)) {}

    // --- Fluent Query Builder Forwarding ---

    /**
     * @brief Adds a basic WHERE equality clause to the query.
     * @param field The database column or property name.
     * @param value The value to match.
     * @return QueryHelper& A reference to this query helper for method chaining.
     */
    QueryHelper &where(const QString &field, const QVariant &value) { m_query.where(field, value); return *this; }

    /**
     * @brief Adds a WHERE clause with a custom comparison operator.
     * @param field The database column or property name.
     * @param op The comparison operator (e.g., ">", "LIKE", "!=").
     * @param value The value to compare against.
     * @return QueryHelper& A reference to this query helper for method chaining.
     */
    QueryHelper &where(const QString &field, const QString &op, const QVariant &value) { m_query.where(field, op, value); return *this; }

    /**
     * @brief Adds a raw SQL WHERE expression string to the query.
     * @param expression The raw SQL condition snippet.
     * @return QueryHelper& A reference to this query helper for method chaining.
     */
    QueryHelper &whereRaw(const QString &expression) { m_query.whereRaw(expression); return *this; }

    /**
     * @brief Appends an AND WHERE equality clause to the query.
     * @param field The column name.
     * @param value The value to match.
     * @return QueryHelper& A reference to this query helper for method chaining.
     */
    QueryHelper &andWhere(const QString &field, const QVariant &value) { m_query.andWhere(field, value); return *this; }

    /**
     * @brief Appends an AND WHERE clause with a custom operator.
     * @param field The column name.
     * @param op The comparison operator.
     * @param value The value to compare against.
     * @return QueryHelper& A reference to this query helper for method chaining.
     */
    QueryHelper &andWhere(const QString &field, const QString &op, const QVariant &value) { m_query.andWhere(field, op, value); return *this; }

    /**
     * @brief Appends a raw SQL AND expression to the query.
     * @param expression The raw SQL expression.
     * @return QueryHelper& A reference to this query helper for method chaining.
     */
    QueryHelper &andWhereRaw(const QString &expression) { m_query.andWhereRaw(expression); return *this; }

    /**
     * @brief Appends an OR WHERE equality clause to the query.
     * @param field The column name.
     * @param value The value to match.
     * @return QueryHelper& A reference to this query helper for method chaining.
     */
    QueryHelper &orWhere(const QString &field, const QVariant &value) { m_query.orWhere(field, value); return *this; }

    /**
     * @brief Appends an OR WHERE clause with a custom operator.
     * @param field The column name.
     * @param op The comparison operator.
     * @param value The value to compare against.
     * @return QueryHelper& A reference to this query helper for method chaining.
     */
    QueryHelper &orWhere(const QString &field, const QString &op, const QVariant &value) { m_query.orWhere(field, op, value); return *this; }

    /**
     * @brief Appends a raw SQL OR expression to the query.
     * @param expression The raw SQL expression.
     * @return QueryHelper& A reference to this query helper for method chaining.
     */
    QueryHelper &orWhereRaw(const QString &expression) { m_query.orWhereRaw(expression); return *this; }

    /**
     * @brief Adds an INNER JOIN clause to the query.
     * @param table The target table name to join.
     * @param first The local or reference column.
     * @param op The comparison operator (typically "=").
     * @param second The foreign column to join on.
     * @return QueryHelper& A reference to this query helper for method chaining.
     */
    QueryHelper &innerJoin(const QString &table, const QString &first, const QString &op, const QString &second) {
        return join(table, first, op, second, "INNER");
    }

    /**
     * @brief Adds a customized SQL JOIN clause to the query.
     * @param table The target table name.
     * @param first The first column.
     * @param op The comparison operator.
     * @param second The second column.
     * @param type The join type (e.g., "INNER", "LEFT", "RIGHT").
     * @return QueryHelper& A reference to this query helper for method chaining.
     */
    QueryHelper &join(const QString &table, const QString &first, const QString &op, const QString &second, const QString &type = "INNER") {
        m_query.join(table, first, op, second, type);
        return *this;
    }

    /**
     * @brief Adds a GROUP BY clause to the query.
     * @param field The column name to group results by.
     * @return QueryHelper& A reference to this query helper for method chaining.
     */
    QueryHelper &groupBy(const QString &field) { m_query.groupBy(field); return *this; }

    /**
     * @brief Adds an ORDER BY clause to the query.
     * @param field The column name to sort by.
     * @param order The sort direction (`Qt::AscendingOrder` or `Qt::DescendingOrder`).
     * @return QueryHelper& A reference to this query helper for method chaining.
     */
    QueryHelper &orderBy(const QString &field, Qt::SortOrder order = Qt::DescendingOrder) { m_query.orderBy(field, order); return *this; }

    /**
     * @brief Limits the maximum number of records returned by the query.
     * @param limit The maximum row count.
     * @return QueryHelper& A reference to this query helper for method chaining.
     */
    QueryHelper &limit(int limit) { m_query.limit(limit); return *this; }

    /**
     * @brief Sets the row offset for result pagination.
     * @param offset The number of rows to skip.
     * @return QueryHelper& A reference to this query helper for method chaining.
     */
    QueryHelper &offset(int offset) { m_query.offset(offset); return *this; }

    /**
     * @brief Eager loads a single relation for the model results.
     * @param relation The name of the relation to load.
     * @return QueryHelper& A reference to this query helper for method chaining.
     */
    QueryHelper &with(const QString &relation) { m_query.with(relation); return *this; }

    /**
     * @brief Eager loads multiple relations for the model results.
     * @param relations A string list of relation names to load.
     * @return QueryHelper& A reference to this query helper for method chaining.
     */
    QueryHelper &with(const QStringList &relations) { m_query.with(relations); return *this; }

    // --- Execution & Conversion Operators ---

    /**
     * @brief Executes the query and dumps the resulting model collection to debug output.
     * @return QueryHelper& A reference to this query helper for method chaining.
     */
    QueryHelper &dump() {
        auto result = this->get();
        if (result) {
            for (int i(0); i < result->count(); ++i) {
                qDebug().noquote() << result->at(i) << (i < result->count() - 1 ? "\n" : "");
            }
        } else {
            qCritical() << "Query failed during dump:" << result.error().message();
        }
        return *this;
    }

    /**
     * @brief Dumps the query results and halts application execution with file and line precision.
     * @param loc The source code location capturing file name and line number automatically.
     */
    void dumpAndDie(const std::source_location &loc = std::source_location::current()) {
        dump();
        qFatal("\nExecution halted via QEloquent dumpAndDie() at %s, line %d.", loc.file_name(), loc.line());
    }

    /**
     * @brief Counts the total records matching the current query constraints.
     * @return Result<int, Error> The row count on success, or an Error object on failure.
     */
    Result<int, Error> count() {
        const QString statement = QueryBuilder::selectStatement("COUNT(1)", m_query);

        auto result = QueryRunner::exec(statement, m_query.connectionName());
        if (result)
            return (result->next() ? result->value(0).toInt() : 0);
        else
            return failWith(Error::fromSqlError("SQL: " + statement, result.error()));
    }

    /**
     * @brief Retrieves the first record matching the query criteria.
     * @return Result<Model, Error> The matching model instance on success, or an Error object on failure.
     */
    Result<Model, Error> first() {
        m_query.limit(1);
        auto results = get();
        if (results) {
            if (results->count() > 0)
                return results->constFirst();
            else
                return failWith(Error(Error::NotFoundError));
        } else {
            return failWith(results.error());
        }
    }

    /**
     * @brief Paginates the query results based on page number and items per page.
     * @param page The 1-indexed page number.
     * @param itemsPerPage The maximum number of items per page.
     * @return Result<QList<Model>, Error> A list of models for the requested page, or an Error on failure.
     */
    Result<QList<Model>, Error> paginate(int page = 1, int itemsPerPage = 20) {
        m_query.limit(itemsPerPage).offset((page - 1) * itemsPerPage);
        return get();
    }

    /**
     * @brief Executes the query and retrieves all matching model records with eager-loaded relations.
     * @return Result<QList<Model>, Error> A list of populated model instances on success, or an Error object on failure.
     */
    Result<QList<Model>, Error> get() {
        const MetaObject metaObject = Maker::metaObject();
        const QString statement = QueryBuilder::selectStatement(m_query);

        auto result = QueryRunner::exec(statement, m_query.connectionName());
        if (result) {
            QList<Model> models;

            QStringList relations = metaObject.relations() + m_query.relations();
            relations.removeDuplicates();

            while (result->next()) {
                Model m = Maker::make();
                safeModel(m).fill(result->record());
                if (safeModel(m).load(relations))
                    models.append(m);
                else
                    return failWith(safeModel(m).lastError());
            }

            m_executed = true;
            return models;
        } else {
            return failWith(Error::fromSqlError("SQL: " + statement, result.error()));
        }
    }

    /**
     * @brief Deletes database records matching the query constraints.
     * @return Result<int, Error> The number of affected rows on success, or an Error object on failure.
     */
    Result<int, Error> deleteData() {
        const QString statement = QueryBuilder::deleteStatement(m_query);

        auto result = QueryRunner::exec(statement, m_query.connectionName());
        if (result)
            return result->numRowsAffected();
        else
            return failWith(Error::fromSqlError("SQL: " + statement, result.error()));
    }

    /**
     * @brief Implicit conversion operator to extract a single Model instance.
     */
    operator Model() {
        if (!m_executed)
            get();
        return (m_models.isEmpty() ? Maker::make() : m_models.first());
    }

    /**
     * @brief Implicit conversion operator to extract a list of Model instances.
     */
    operator QList<Model>() {
        if (!m_executed)
            get();
        return m_models;
    }

    /**
     * @brief Implicit conversion operator to extract the underlying Query configuration object.
     */
    operator Query() const
    { return m_query; }

    /**
     * @brief Casts any child model instance to its base `QEloquent::Model` reference.
     * @param instance The concrete model instance.
     * @return QEloquent::Model& A reference to the base model.
     */
    static QEloquent::Model &safeModel(Model &instance)
    { return static_cast<QEloquent::Model &>(instance); }

    /**
     * @brief Casts any child model instance to its base `QEloquent::Model` const reference.
     * @param instance The concrete model instance.
     * @return const QEloquent::Model& A const reference to the base model.
     */
    static const QEloquent::Model &safeModel(const Model &instance)
    { return static_cast<const QEloquent::Model &>(instance); }

    /**
     * @brief Configures and initializes default query properties based on model metadata.
     * @param query The query to fix.
     * @return Query& A reference to the configured query.
     */
    static Query &fixQuery(Query &query);

private:
    Query m_query;
    QList<Model> m_models;
    bool m_executed = false;
};

template<typename Model, typename Maker>
inline Query &QueryHelper<Model, Maker>::fixQuery(Query &query)
{
    const MetaObject meta = Maker::metaObject();
    return query.connection(meta.connectionName())
        .table(meta.tableName())
        .raw(QString());
}

} // namespace QEloquent

#endif // QELOQUENT_QUERYHELPER_H