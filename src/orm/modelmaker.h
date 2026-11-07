#ifndef QELOQUENT_MODELMAKER_H
#define QELOQUENT_MODELMAKER_H

#include <QEloquent/metaobject.h>

namespace QEloquent {

/**
 * @brief Internal helper to create model instances and access metadata.
 * @tparam Model The model type.
 */
template<typename Model>
class ModelMaker {
public:
    /** @brief Creates a default model instance */
    static Model make() { return Model(); }
    /** @brief Returns the MetaObject for the model type */
    static MetaObject metaObject() { return MetaObject::fromQtMetaObject(Model::staticMetaObject); }
};

} // namespace QEloquent

#endif // QELOQUENT_MODELMAKER_H
