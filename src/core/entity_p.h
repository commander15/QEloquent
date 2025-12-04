#ifndef QELOQUENT_ENTITY_P_H
#define QELOQUENT_ENTITY_P_H

#include "entity.h"

#include <QStringList>

namespace QEloquent {

class EntityInfoData : public QSharedData
{
public:
    virtual ~EntityInfoData() = default;

    virtual EntityInfoData *clone() const
    { return new EntityInfoData(*this); }

    QString table;
    QString foreignKey;
    QStringList with;
};

}

#endif // QELOQUENT_ENTITY_P_H
