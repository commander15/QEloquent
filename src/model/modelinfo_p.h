#ifndef MODELINFO_P_H
#define MODELINFO_P_H

#include <QEloquent/connection.h>
#include <QEloquent/private/entity_p.h>

namespace QEloquent {

class ModelInfoData : public EntityInfoData
{
public:
    ModelInfoData *clone() const override
    { return new ModelInfoData(*this); }

    QString primaryKey;

    QStringList hidden;
    QStringList fillables;

    QString createdAt;
    QString updatedAt;
    QString deletedAt;

    QString connection;

    const QMetaObject *metaObect = nullptr;
};

}

#endif // MODELINFO_P_H
