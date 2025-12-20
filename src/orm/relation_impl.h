#ifndef QELOQUENT_RELATION_IMPL_H
#define QELOQUENT_RELATION_IMPL_H

#include <QEloquent/global.h>
#include <QEloquent/relation.h>
#include <QEloquent/model.h>

namespace QEloquent {

template<typename RelatedModel>
class HasOneRelationImpl {
public:
    static bool get(RelationBaseData<RelatedModel> *data, const QVariantHash &parameters)
    {
        auto result = RelatedModel::find(data->parent->property(parameters.value("foreign_key", data->relatedObject.foreignProperty().fieldName())));
        if (result) {
            data->related = { result };
            return true;
        } else {
            //
            return false;
        }
    }
};

}

#endif // QELOQUENT_RELATION_IMPL_H
