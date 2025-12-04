#include "modelinfo.h"
#include "modelinfo_p.h"

#include <QEloquent/connection.h>
#include <QEloquent/querybuilder.h>

#include <QSqlDatabase>
#include <QSqlRecord>

#include <QJsonObject>
#include <QJsonValue>

namespace QEloquent {

ModelInfo::ModelInfo()
    : EntityInfo(new ModelInfoData)
{}

ModelInfo::ModelInfo(const QSharedDataPointer<EntityInfoData> &data)
    : EntityInfo(data)
{
}

ModelInfo::ModelInfo(const ModelInfo &rhs)
    : EntityInfo(rhs)
{}

ModelInfo::ModelInfo(ModelInfo &&rhs)
    : EntityInfo(std::move(rhs.data))
{}

ModelInfo &ModelInfo::operator=(const ModelInfo &rhs)
{
    EntityInfo::operator=(rhs);
    return *this;
}

ModelInfo &ModelInfo::operator=(ModelInfo &&rhs)
{
    EntityInfo::operator=(rhs);
    return *this;
}

ModelInfo::~ModelInfo() {}

QString ModelInfo::primaryKey() const
{
    ENTITY_INFO_DATA(const ModelInfo);
    return data.primaryKey;
}

QStringList ModelInfo::hidden() const
{
    ENTITY_INFO_DATA(const ModelInfo);
    return data.hidden;
}

QStringList ModelInfo::fillables() const
{
    ENTITY_INFO_DATA(const ModelInfo);
    return data.fillables;
}

QStringList ModelInfo::fields() const
{
    ENTITY_INFO_DATA(const ModelInfo);

    QStringList fields;

    const QSqlRecord record = Connection::connection(data.connection).database().record(data.table);
    for (int i(0); i < record.count(); ++i)
        fields.append(record.fieldName(i));

    return fields;
}

bool ModelInfo::hasCreatedAt() const
{
    ENTITY_INFO_DATA(const ModelInfo);
    return !data.createdAt.isEmpty();
}

QString ModelInfo::createdAt() const
{
    ENTITY_INFO_DATA(const ModelInfo);
    return data.createdAt;
}

bool ModelInfo::hasUpdatedAt() const
{
    ENTITY_INFO_DATA(const ModelInfo);
    return !data.updatedAt.isEmpty();
}

QString ModelInfo::updatedAt() const
{
    ENTITY_INFO_DATA(const ModelInfo);
    return data.updatedAt;
}

bool ModelInfo::hasDeletedAt() const
{
    ENTITY_INFO_DATA(const ModelInfo);
    return !data.deletedAt.isEmpty();
}

QString ModelInfo::deletedAt() const
{
    ENTITY_INFO_DATA(const ModelInfo);
    return data.deletedAt;
}

Connection ModelInfo::connection() const
{
    ENTITY_INFO_DATA(const ModelInfo);
    return Connection::connection(data.connection);
}

const QMetaObject *ModelInfo::metaObject() const
{
    ENTITY_INFO_DATA(const ModelInfo);
    return data.metaObect;
}

}
