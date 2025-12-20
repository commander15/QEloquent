#ifndef RELATIONBASE_H
#define RELATIONBASE_H

#include <QEloquent/global.h>
#include <QEloquent/relation.h>

#include <QSharedDataPointer>

namespace QEloquent {

class RelationBaseData;
class QELOQUENT_EXPORT RelationBase
{
public:
    RelationBase();
    RelationBase(const RelationBase &);
    RelationBase(RelationBase &&);
    RelationBase &operator=(const RelationBase &);
    RelationBase &operator=(RelationBase &&);
    ~RelationBase();

    template<typename Model>
    Relation<Model> build() const
    { return Relation<Model>(); }

private:
    QSharedDataPointer<RelationBaseData> data;
};

} // namespace QEloquent

#endif // RELATIONBASE_H
