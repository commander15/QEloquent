#include "metaobjectgenerator.h"

#include "metaobjectgeneration_p.h"
#include "metaobjectregistry.h"

#include <QMetaObject>

namespace QEloquent {

MetaObject MetaObjectGenerator::generate(const QMetaObject &modelMetaObject, bool cache)
{
    if (cache) {
        const QString className(modelMetaObject.className());
        if (MetaObjectRegistry::contains(className))
            return MetaObjectRegistry::metaObject(className);
    }

    MetaObjectGeneration gen(&modelMetaObject);
    MetaObject object = gen.generate();

    if (cache && object.isValid())
        MetaObjectRegistry::registerMetaObject(object);

    return object;
}



} // namespace QEloquent
