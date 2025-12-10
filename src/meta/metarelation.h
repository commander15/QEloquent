#ifndef QELOQUENT_METARELATION_H
#define QELOQUENT_METARELATION_H

#include <QEloquent/global.h>

#include <QSharedDataPointer>

namespace QEloquent {

class Model;

class MetaRelationData;
class MetaRelation
{
public:
    enum RelationAttributeFlag {
        AutoLoad = 0x1
    };

    MetaRelation();
    MetaRelation(const MetaRelation &);
    MetaRelation(MetaRelation &&);
    MetaRelation &operator=(const MetaRelation &);
    MetaRelation &operator=(MetaRelation &&);
    ~MetaRelation();

    QString relationName() const;

    QVariant read(Model *model) const;

private:
    QExplicitlySharedDataPointer<MetaRelationData> data;
};

} // namespace QEloquent

#endif // QELOQUENT_METARELATION_H
