#include "relationbase.h"

#include <utility>

namespace QEloquent {

class RelationBaseData : public QSharedData
{
public:
};

RelationBase::RelationBase()
    : data(new RelationBaseData)
{}

RelationBase::RelationBase(const RelationBase &rhs)
    : data{rhs.data}
{}

RelationBase::RelationBase(RelationBase &&rhs)
    : data{std::move(rhs.data)}
{}

RelationBase &RelationBase::operator=(const RelationBase &rhs)
{
    if (this != &rhs)
        data = rhs.data;
    return *this;
}

RelationBase &RelationBase::operator=(RelationBase &&rhs)
{
    if (this != &rhs)
        data = std::move(rhs.data);
    return *this;
}

RelationBase::~RelationBase() {}

} // namespace QEloquent
