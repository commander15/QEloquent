#include "relation.h"

#include <QEloquent/model.h>
#include <QEloquent/private/model_p.h>

namespace QEloquent {

RelationData::RelationData() = default;
RelationData::RelationData(const RelationData &other) = default;
RelationData::~RelationData() = default;

QString RelationData::serializationContext() const
{
    return parent->serializationContext() + '.' + name;
}

QExplicitlySharedDataPointer<RelationData> RelationData::create(const QString &name, const Model *parent, const std::function<RelationData *()> &creationCallback)
{
    Model *pa = const_cast<Model *>(parent);
    ModelData *d = pa->data.get();
    if (d->relationData.contains(name)) {
        auto p = d->relationData.value(name);
        p->parent = pa; // we update the parent model
        return p;
    } else {
        auto p = QExplicitlySharedDataPointer<RelationData>(creationCallback());
        p->name = name;
        p->parent = pa;
        p->primaryObject = pa->metaObject();
        p->init(d->metaObject.namingConvention());
        d->relationData.insert(name, p);
        return p;
    }
}

QExplicitlySharedDataPointer<RelationData> RelationData::create(const std::source_location &location, const Model *parent, const std::function<RelationData *()> &creationCallback)
{
    QString name(location.function_name());

    const QStringList parts = name.split(' ');
    for (const QString &part : parts) {
        if (!part.endsWith(')')) continue;

        name = part.section("::", -1);
        int i = name.indexOf('(');
        name.remove(i, name.length() - i);
    }

    return create(name, parent, creationCallback);
}

QVariant RelationData::parentPrimary() const
{
    return (parent == nullptr ? QVariant() : parent->primary());
}

void RelationData::setParentPrimary(const QVariant &value)
{
    if (parent != nullptr)
        parent->setPrimary(value);
}

QVariant RelationData::parentField(const QString &name) const
{
    return (parent == nullptr ? QVariant() : parent->field(name));
}

void RelationData::setParentField(const QString &name, const QVariant &value)
{
    if (parent != nullptr)
        parent->setField(name, value);
}

void RelationData::conserve(const Query &query)
{
}

void RelationData::conserve(const Query &query, const Error &error)
{
}

} // namespace QEloquent

template<>
QEloquent::RelationData *QExplicitlySharedDataPointer<QEloquent::RelationData>::clone()
{
    return d->clone();
}
