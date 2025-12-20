#ifndef QELOQUENT_ENTITY_P_H
#define QELOQUENT_ENTITY_P_H

#include "entity.h"
#include <QSharedData>

namespace QEloquent {

class EntityInfoData : public QSharedData
{
public:
    virtual ~EntityInfoData() = default;
    virtual EntityInfoData *clone() const = 0;

    QString table;
    QString foreignKey;
};

} // namespace QEloquent

#endif // QELOQUENT_ENTITY_P_H
