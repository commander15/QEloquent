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
    MetaObjectGenerator();

    template<typename Model> MetaObject generate(bool cache = true) { return generate(Model::staticMetaObject, cache); }
    MetaObject generate(const QMetaObject &modelMetaObject, bool cache = true);

private:
    void initGeneration(MetaObjectGeneration *generation);
    void discoverProperties(MetaObjectGeneration *generation);
    void tuneProperty(int &index, class MetaPropertyData *property, MetaObjectGeneration *generation, bool save);
};

} // namespace QEloquent

#endif // QELOQUENT_METAOBJECTGENERATOR_H
