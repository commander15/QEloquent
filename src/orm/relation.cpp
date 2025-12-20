#include "relation.h"

#include <QEloquent/model.h>
#include <QEloquent/private/model_p.h>

namespace QEloquent {

QExplicitlySharedDataPointer<RelationData> RelationData::fromParentModel(const QString &name, Model *parent, const std::function<RelationData *()> &creationCallback)
{
    ModelData *d = parent->data.get();
    if (d->relationData.contains(name)) {
        return d->relationData.value(name);
    } else {
        auto p = QExplicitlySharedDataPointer<RelationData>(creationCallback());
        d->relationData.insert(name, p);
        return p;
    }
}

} // namespace QEloquent
