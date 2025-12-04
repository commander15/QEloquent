#include "modelinfobuilder.h"

#include <QEloquent/querybuilder.h>
#include <QEloquent/connection.h>
#include <QEloquent/private/modelinfo_p.h>

#include <QJsonObject>
#include <QJsonValue>
#include <QSqlDatabase>
#include <QSqlIndex>
#include <QSqlRecord>

namespace QEloquent {

class ModelInfoBuilderData : public QSharedData
{
public:
    ModelInfoData *info = new ModelInfoData();
    QSharedDataPointer<EntityInfoData> shared = QSharedDataPointer<EntityInfoData>(info);
};

ModelInfoBuilder::ModelInfoBuilder()
    : data(new ModelInfoBuilderData())
{
}

ModelInfoBuilder::ModelInfoBuilder(const QString &table)
    : data(new ModelInfoBuilderData())
{
    this->table(table);
}

ModelInfoBuilder::~ModelInfoBuilder()
{
}

ModelInfoBuilder &ModelInfoBuilder::table(const QString &name)
{
    data->info->table = name;
    return *this;
}

ModelInfoBuilder &ModelInfoBuilder::primaryKey(const QString &field)
{
    data->info->primaryKey = field;
    return *this;
}

ModelInfoBuilder &ModelInfoBuilder::foreignKey(const QString &field)
{
    data->info->foreignKey = field;
    return *this;
}

ModelInfoBuilder &ModelInfoBuilder::hidden(const QString &field)
{
    data->info->hidden.append(field);
    return *this;
}

ModelInfoBuilder &ModelInfoBuilder::hidden(const QStringList &fields)
{
    data->info->hidden.append(fields);
    return *this;
}

ModelInfoBuilder &ModelInfoBuilder::fillable(const QString &field)
{
    data->info->fillables.append(field);
    return *this;
}

ModelInfoBuilder &ModelInfoBuilder::fillable(const QStringList &fields)
{
    data->info->fillables.append(fields);
    return *this;
}

ModelInfoBuilder &ModelInfoBuilder::metaObject(const QMetaObject *object)
{
    data->info->metaObect = object;
    return *this;
}

bool ModelInfoBuilder::isValid() const
{
    return !data->info->table.isEmpty();
}

void ModelInfoBuilder::loadJson(const QJsonObject &object)
{
    ModelInfoData *data = this->data->info;
    data->table = object.value("table").toString();
    data->primaryKey = object.value("primary_key").toString();
    data->foreignKey = object.value("foreign_key").toString();
    data->hidden = object.value("hidden").toVariant().toStringList();
    data->fillables = object.value("fillables").toVariant().toStringList();

    const QJsonObject timestamps = object.value("timestamps").toObject();
    data->createdAt = timestamps.value("created_at").toString();
    data->updatedAt = timestamps.value("updated_at").toString();
    data->deletedAt = timestamps.value("deleted_at").toString();
}

ModelInfo ModelInfoBuilder::build(const Connection &connection) const
{
    const QSqlDatabase db = connection.database();

    ModelInfoData *info = data->info;

    if (info->primaryKey.isEmpty()) {
        const QSqlIndex primaryIndex = db.primaryIndex(info->table);
        info->primaryKey = (primaryIndex.isEmpty() ? "id" : primaryIndex.fieldName(0));
    }

    if (info->foreignKey.isEmpty()) {
        info->foreignKey = QueryBuilder::singularise(info->table).toLower() + '_' + info->primaryKey;
    }

    const QSqlRecord record = db.record(info->table);

    if (info->createdAt.isEmpty() && record.contains("created_at")) {
        info->createdAt = "created_at";
    }

    if (info->updatedAt.isEmpty() && record.contains("updated_at")) {
        info->updatedAt = "updated_at";
    }

    if (info->deletedAt.isEmpty() && record.contains("deleted_at")) {
        info->deletedAt = "deleted_at";
    }

    if (info->fillables.empty()) {
        for (int i(0); i < record.count(); ++i) {
            const QString fieldName = record.fieldName(i);

            if (fieldName == data->info->primaryKey)
                continue;

            if (fieldName == data->info->createdAt)
                continue;

            if (fieldName == data->info->updatedAt)
                continue;

            if (fieldName == data->info->deletedAt)
                continue;

            data->info->fillables.append(fieldName);
        }
    }

    info->connection = connection.name();
    return ModelInfo(data->shared);
}

} // namespace QEloquent
