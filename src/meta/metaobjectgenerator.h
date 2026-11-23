#ifndef QELOQUENT_METAOBJECTGENERATOR_H
#define QELOQUENT_METAOBJECTGENERATOR_H

#include <QEloquent/global.h>
#include <QEloquent/metaobject.h>

class QMetaObject;

namespace QEloquent {

class MetaObjectGeneration;

class QELOQUENT_EXPORT MetaObjectGenerator
{
public:
    template<typename Model> MetaObject generate(bool cache = true) { return generate(Model::staticMetaObject, cache); }
    MetaObject generate(const QMetaObject &modelMetaObject, bool cache = true);
};

} // namespace QEloquent

#endif // QELOQUENT_METAOBJECTGENERATOR_H
