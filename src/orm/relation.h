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

    // No need for full CRUD for now
    bool insert() override { return false; }
    bool update() override { return false; };
    bool deleteData() override { return false; }

    virtual RelationData *clone() const = 0;

    QString name;
    Model *parent;
    QMap<int, QVariantMap> associatedData;

    MetaObject primaryObject;
    MetaObject relatedObject;

    bool isLoaded = false;

    static QExplicitlySharedDataPointer<RelationData> fromParentModel(const QString &name, Model *parent, const std::function<RelationData *()> &creationCallback);
};

template<typename RelatedModel>
class RelationBaseData : public RelationData
{
public:
    RelationBaseData() { relatedObject = MetaObject::from<RelatedModel>(); }
    virtual ~RelationBaseData() = default;

    bool exists() override { return related.count() > 0; }

    QList<RelatedModel> related;
};

template<typename Model>
class Relation : public Entity
{
    using ParentModel = QEloquent::Model;

public:
    Relation() {}
    Relation(const QString &name, ParentModel *parent, const std::function<RelationData *()> &creationCallback)
        : data(RelationData::fromParentModel(name, parent, creationCallback)) {}
    Relation(const QString &name, ParentModel *parent)
        : data(RelationData::fromParentModel(name, parent, [] { return nullptr; })) {}
    Relation(RelationData *data) : data(data) {}
    Relation(const Relation &other) = default;
    Relation(Relation &&other) = default;

    Relation<Model> &operator=(const Relation &other) = default;
    Relation<Model> &operator=(Relation &&other) = default;

    bool hasAssociatedData(int index, const QString &name) const
    { return data->associatedData.value(index).contains(name); }

    QVariant associatedData(int index, const QString &name) const
    { return data->associatedData.value(index).value(name); }

    void setAssociatedData(int index, const QString &name, const QVariant &value)
    { data->associatedData[index].insert(name, value); }

    QStringList associatedDataNames() const
    { return (data->associatedData.isEmpty() ? QStringList() : data->associatedData.value(data->associatedData.firstKey()).keys()); }

    bool exists() override { ensureLoaded(); return data->exists(); }
    bool get() override { data->isLoaded = true; return data->get(); }
    bool save() override { return data->save(); }
    bool insert() override { return data->insert(); }
    bool update() override { return data->update(); }
    bool deleteData() override { return data->deleteData(); }

    void ensureLoaded() const {
        if (!data->isLoaded) {
            const_cast<Relation*>(this)->get();
        }
    }

    typedef typename QList<Model>::iterator iterator;
    typedef typename QList<Model>::const_iterator const_iterator;

    iterator begin() { ensureLoaded(); return const_cast<RelationBaseData<Model> *>(static_cast<const RelationBaseData<Model> *>(data.get()))->related.begin(); }
    iterator end() { ensureLoaded(); return const_cast<RelationBaseData<Model> *>(static_cast<const RelationBaseData<Model> *>(data.get()))->related.end(); }
    const_iterator begin() const { ensureLoaded(); return static_cast<const RelationBaseData<Model> *>(data.get())->related.begin(); }
    const_iterator end() const { ensureLoaded(); return static_cast<const RelationBaseData<Model> *>(data.get())->related.end(); }
    const_iterator cbegin() const { ensureLoaded(); return static_cast<const RelationBaseData<Model> *>(data.get())->related.cbegin(); }
    const_iterator cend() const { ensureLoaded(); return static_cast<const RelationBaseData<Model> *>(data.get())->related.cend(); }

    int count() const { ensureLoaded(); return static_cast<const RelationBaseData<Model> *>(data.get())->related.count(); }
    int size() const { return count(); }
    bool isEmpty() const { return count() == 0; }

    const Model &at(int i) const { ensureLoaded(); return static_cast<const RelationBaseData<Model> *>(data.get())->related.at(i); }
    const Model &operator[](int i) const { return at(i); }

    Model first() const { return count() > 0 ? at(0) : Model(); }
    Model last() const { return count() > 0 ? at(count() - 1) : Model(); }

    Model* operator->() {
        ensureLoaded();
        RelationBaseData<Model> *d = const_cast<RelationBaseData<Model> *>(static_cast<const RelationBaseData<Model> *>(data.get()));
        if (d->related.isEmpty()) return nullptr;
        return &d->related.first();
    }

    const Model* operator->() const {
        ensureLoaded();
        const RelationBaseData<Model> *d = static_cast<const RelationBaseData<Model> *>(data.get());
        if (d->related.isEmpty()) return nullptr;
        return &d->related.first();
    }

    Model& operator*() { return *operator->(); }
    const Model& operator*() const { return *operator->(); }

    operator Model() const { return first(); }

    QList<Model> related() const
    {
        ensureLoaded();
        auto d = static_cast<const RelationBaseData<Model> *>(data.get());
        return d ? d->related : QList<Model>();
    }

private:
    QExplicitlySharedDataPointer<RelationData> data;
};

} // namespace QEloquent

#endif // QELOQUENT_RELATION_H
