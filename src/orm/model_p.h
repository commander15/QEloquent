#ifndef QLOQUENT_ORM_MODEL_P_H
#define QLOQUENT_ORM_MODEL_P_H

#include "model.h"

#include <QEloquent/query.h>
#include <QEloquent/error.h>
#include <QEloquent/metaobject.h>

#include <QVariant>
#include <QMetaProperty>

namespace QEloquent {

class ModelData : public QSharedData
{
public:
    QVariantMap dynamicProperties;
    QMap<QString, QExplicitlySharedDataPointer<RelationData>> relationData;
    MetaObject metaObject;

    Query lastQuery;
    Error lastError;
};

} // namespace QELoquent

#endif // QLOQUENT_ORM_MODEL_P_H
