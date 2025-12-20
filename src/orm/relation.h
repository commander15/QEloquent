#ifndef QELOQUENT_RELATION_H
#define QELOQUENT_RELATION_H

#include <QEloquent/global.h>
#include <QEloquent/entity.h>
#include <QEloquent/metaobject.h>

#include <QList>
#include <QVariant>

namespace QEloquent {

class RelationData : public Entity, public QSharedData
{
public:
    virtual ~RelationData() = default;

    static QExplicitlySharedDataPointer<RelationData> fromParentModel(const QString &name, Model *parent, const std::function<RelationData *()> &creationCallback);

    virtual RelationData *clone() const = 0;

    QString name;
    Model *parent;
    QMap<int, QVariantMap> associatedData;

    MetaObject primaryObject;
    MetaObject relatedObject;
};

template<typename RelatedModel>
class RelationBaseData : public RelationData
{
public:
    QList<RelatedModel> related;
};

template<typename Model>
class Relation : public Entity
{
public:
    Relation();
    Relation(const QString &name, Model *parent)
        : data(RelationData::fromParentModel(name, parent, [] { return nullptr; })) {}
    Relation(const Relation &other);

    bool hasAssociatedData(int index, const QString &name) const
    { return data->associatedData.value(index).contains(name); }

    QVariant associatedData(int index, const QString &name) const
    { return data->associatedData.value(index).value(name); }

    void setAssociatedData(int index, const QString &name, const QVariant &value)
    { data->associatedData[index].insert(name, value); }

    QStringList associatedDataNames() const
    { return (data->associatedData.isEmpty() ? QStringList() : data->associatedData.value(data->associatedData.firstKey()).keys()); }

    bool exists() override { return data->exists(); }
    bool get() override { return data.get(); }
    bool save() override { return data->save(); }
    bool insert() override { return data->insert(); }
    bool update() override { return data->update(); }
    bool deleteData() override { return data->deleteData(); }

private:
    QExplicitlySharedDataPointer<RelationData> data;
};

} // namespace QEloquent

#endif // QELOQUENT_RELATION_H
