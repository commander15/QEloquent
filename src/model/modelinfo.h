#ifndef QELOQUENT_MODELINFO_H
#define QELOQUENT_MODELINFO_H

#include <QEloquent/global.h>
#include <QEloquent/entity.h>

class QJsonObject;

namespace QEloquent {

class Connection;

class ModelInfoData;
class QELOQUENT_EXPORT ModelInfo : public EntityInfo
{
public:
    ModelInfo();
    ModelInfo(const ModelInfo &);
    ModelInfo(ModelInfo &&);
    ModelInfo &operator=(const ModelInfo &);
    ModelInfo &operator=(ModelInfo &&);
    ~ModelInfo();

    QString primaryKey() const;

    QStringList hidden() const;
    QStringList fillables() const;
    QStringList fields() const;

    bool hasCreatedAt() const;
    QString createdAt() const;

    bool hasUpdatedAt() const;
    QString updatedAt() const;

    bool hasDeletedAt() const;
    QString deletedAt() const;

    Connection connection() const;

    const QMetaObject *metaObject() const;

private:
    ModelInfo(const QSharedDataPointer<EntityInfoData> &data);

    friend class ModelInfoBuilder;
};

}

#endif // QELOQUENT_MODELINFO_H
