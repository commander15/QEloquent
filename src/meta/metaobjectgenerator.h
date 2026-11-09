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

private:
    bool initGeneration(MetaObjectGeneration *generation);
    void discoverProperties(MetaObjectGeneration *generation);

    MetaPropertyData *newStandardProperty(int index, const QMetaProperty &meta, MetaObjectGeneration *generation);
    MetaPropertyData *newDynamicProperty(int index, const QString &name, MetaObjectGeneration *generation);
    MetaPropertyData *newComputedProperty(int index, const QMetaMethod &method, MetaObjectGeneration *generation);
    MetaPropertyData *newRelationProperty(int index, const QMetaMethod &method, MetaObjectGeneration *generation);
    void tuneProperty(int index, MetaPropertyData *property, MetaObjectGeneration *generation);
    //void registerProperty(int index, MetaPropertyData *property, MetaObjectGeneration *generation);

    void initDynamicProperty(MetaPropertyData *property, MetaObjectGeneration *generation);
    void initComputedProperty(MetaPropertyData *property, MetaObjectGeneration *generation);
    void initRelationProperty(MetaPropertyData *property, MetaObjectGeneration *generation);

    void tuneProperty(int &index, MetaPropertyData *property, MetaObjectGeneration *generation, bool save);

    MetaPropertyData *newProperty(int &index, MetaObjectGeneration *generation) const;
    void registerProperty(int index, MetaPropertyData *property, MetaObjectGeneration *generation);
};

} // namespace QEloquent

#endif // QELOQUENT_METAOBJECTGENERATOR_H
