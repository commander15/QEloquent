#ifndef QELOQUENT_MODELHELPERS_H
#define QELOQUENT_MODELHELPERS_H

#include <QEloquent/model.h>
#include <QEloquent/modelinfo.h>
#include <QEloquent/modelinfobuilder.h>
#include <QEloquent/modelquery.h>
#include <QEloquent/modelerror.h>
#include <QEloquent/connection.h>
#include <QEloquent/querybuilder.h>
#include <QEloquent/queryrunner.h>

#include <QMetaClassInfo>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QJsonObject>

namespace QEloquent {

template <size_t N>
struct fixed_string {
    char data[N]{};

    // The constructor takes a reference to a C-style array
    // and copies the contents. The compiler will deduce the size N.
    constexpr fixed_string(const char (&str)[N]) {
        std::copy_n(str, N, data);
    }

    QString string() const {
        return QString::fromLatin1(data).trimmed();
    }

    operator bool() const {
        return N > 1;
    }

    // Defaulted comparison operators are required for a class NTTP
    constexpr auto operator<=>(const fixed_string&) const = default;
};

template<typename Model, fixed_string table = "", fixed_string connection = "">
class ModelHelpers
{
public:
    static Model make(const QJsonObject &object = QJsonObject());
    static QList<Model> make(const QList<QJsonObject> &objects);

    static expected<Model, ModelError> find(const QVariant &primary);
    static expected<QList<Model>, ModelError> find(const ModelQuery &query);

    static expected<int, ModelError> count(const ModelQuery &query = ModelQuery());

    static expected<Model, ModelError> create(const QJsonObject &object);
    static expected<QList<Model>, ModelError> create(const QList<QJsonObject> &objects);

    static expected<int, ModelError> remove(const ModelQuery &query);

private:
    static ModelInfo detectInfo();
    static QString className(bool includeNamespace = true);
    static QString classInfo(const char *name, const QString &defaultValue = QString());

    static inline QMap<QString, ModelInfo> s_modelCache;
};

template<typename Model, fixed_string table, fixed_string connection>
inline Model ModelHelpers<Model, table, connection>::make(const QJsonObject &object)
{
    if (table) {
        Model m(detectInfo());
        m.fill(object);
        return m;
    } else {
        Model m;
        m.fill(object);
        return m;
    }
}

template<typename Model, fixed_string table, fixed_string connection>
inline QList<Model> ModelHelpers<Model, table, connection>::make(const QList<QJsonObject> &objects)
{
    QList<Model> models;
    for (const QJsonObject &object : objects) {
        Model m = make(object);
        models.append(m);
    }
    return models;
}

template<typename Model, fixed_string table, fixed_string connection>
inline expected<Model, ModelError> ModelHelpers<Model, table, connection>::find(const QVariant &primary)
{
    ModelQuery query;
    query.where(detectInfo().primaryKey(), primary);
    query.limit(1);

    expected<QList<Model>, ModelError> result = find(query);
    if (result) {
        const QList<Model> models = result.value();
        return (models.isEmpty() ? make() : models.first());
    } else {
        return unexpected(result.error());
    }
}

template<typename Model, fixed_string table, fixed_string connection>
inline expected<QList<Model>, ModelError> ModelHelpers<Model, table, connection>::find(const ModelQuery &query)
{
    const ModelInfo info = detectInfo();
    const Connection con = info.connection();

    const QString statement = QueryBuilder::selectStatement(query, info);

    auto result = QueryRunner::exec(statement, con);
    if (result) {
        QList<Model> models;

        QStringList relations = info.with() + query.relations();
        relations.removeDuplicates();

        while (result->next()) {
            Model m = make();
            m.fill(result->record());
            if (m.load(relations))
                models.append(m);
            else
                return unexpected(m.lastError());
        }

        return models;
    } else {
        return QList<Model>();
    }
}

template<typename Model, fixed_string table, fixed_string connection>
inline expected<int, ModelError> ModelHelpers<Model, table, connection>::count(const ModelQuery &query)
{
    const ModelInfo info = detectInfo();
    const Connection con = info.connection();

    const QString statementBlueprint = QStringLiteral("SELECT COUNT(%1) FROM %2 %3");
    const QString statement = statementBlueprint
                                  .arg(QueryBuilder::escapeFieldName(info.primaryKey(), con), QueryBuilder::escapeTableName(info.table(), con), query.toString(con))
                                  .trimmed();

    auto result = QueryRunner::exec(statement, con);
    if (result)
        return (result->next() ? result->value(0).toInt() : 0);
    else
        return unexpected(ModelError::fromQuery(result.value()));
}

template<typename Model, fixed_string table, fixed_string connection>
inline expected<Model, ModelError> ModelHelpers<Model, table, connection>::create(const QJsonObject &object)
{
    Model model = make(object);
    if (model.save())
        return model;
    else
        return unexpected(model.lastError());
}

template<typename Model, fixed_string table, fixed_string connection>
inline expected<QList<Model>, ModelError> ModelHelpers<Model, table, connection>::create(const QList<QJsonObject> &objects)
{
    QList<Model> models;

    for (const QJsonObject &object : objects) {
        auto result = create(object);
        if (result)
            models.append(result.value());
        else
            return unexpected(result.error());
    }

    return models;
}

template<typename Model, fixed_string table, fixed_string connection>
inline expected<int, ModelError> ModelHelpers<Model, table, connection>::remove(const ModelQuery &query)
{
    const ModelInfo info = detectInfo();
    const Connection con = info.connection();

    const QString statement = QueryBuilder::deleteStatement(query, info);

    auto result = QueryRunner::exec(statement, con);
    if (result)
        return result->numRowsAffected();
    else
        return unexpected(ModelError(ModelError::DatabaseError, QString(), result.error()));
}

template<typename Model, fixed_string table, fixed_string connection>
inline ModelInfo ModelHelpers<Model, table, connection>::detectInfo()
{
    const QString tableName = table.string();
    if (!tableName.isEmpty()) {
        const Connection con = Connection::connection(connection.string());
        return (con.isValid() ? con : Connection::defaultConnection()).modelInfo(tableName);
    }

    const QMetaObject *metaObject = &Model::staticMetaObject;

    // If we already known it, we just return, no need to waste CPU cycles and power
    if (s_modelCache.contains(metaObject->className())) {
        return s_modelCache.value(metaObject->className());
    }

    // If it's a generic model, we just return empty
    if (strcmp(metaObject->className(), "QLoquent::GenericModel") == 0) {
        return ModelInfo();
    }

    // We create our custom ModelInfo from the Meta Object
    ModelInfoBuilder builder;
    builder.table(classInfo("table", className(false).toLower() + 's'));
    builder.primaryKey(classInfo("primaryKey"));
    builder.foreignKey(classInfo("foreignKey"));
    builder.hidden(classInfo("hidden"));
    builder.fillable(classInfo("fillable").remove(' ').split(","));
    builder.metaObject(&Model::staticMetaObject);

    Connection con = Connection::connection(classInfo("connection"));
    if (!con.isValid())
        con = Connection::defaultConnection();
    return con.registerModel(builder);
}

template<typename Model, fixed_string table, fixed_string connection>
inline QString ModelHelpers<Model, table, connection>::className(bool includeNamespace)
{
    const QMetaObject *metaObject = &Model::staticMetaObject;
    const QString name = metaObject->className();
    return (includeNamespace ? name : name.section("::", -1));
}

template<typename Model, fixed_string table, fixed_string connection>
inline QString ModelHelpers<Model, table, connection>::classInfo(const char *name, const QString &defaultValue)
{
    const QMetaObject *metaObject = &Model::staticMetaObject;
    const int index = metaObject->indexOfClassInfo(name);
    return (index < 0 ? defaultValue : QString(metaObject->classInfo(index).value()));
}

} // namespace QEloquent

#endif // QELOQUENT_MODELHELPERS_H
