#ifndef QELOQUENT_RELATION_H
#define QELOQUENT_RELATION_H

#include <QEloquent/global.h>
#include <QEloquent/entity.h>
#include <QEloquent/metaobject.h>

#include <QList>
#include <QVariant>

namespace QEloquent {

/**
 * @brief Base class for internal relation data storage.
 * 
 * This class is managed by Relation objects using QExplicitlySharedDataPointer.
 */
class RelationData : public Entity, public QSharedData
{
public:
    virtual ~RelationData() = default;

    // No need for full CRUD for now
    bool insert() override { return false; }
    bool update() override { return false; };
    bool deleteData() override { return false; }

    /** @brief Creates a deep copy of the relation data */
    virtual RelationData *clone() const = 0;

    QString name;        /**< @brief Name of the relationship */
    Model *parent;       /**< @brief The parent model owning this relation */
    QMap<int, QVariantMap> associatedData; /**< @brief Metadata for many-to-many pivots */

    MetaObject primaryObject; /**< @brief MetaObject of the parent model */
    MetaObject relatedObject; /**< @brief MetaObject of the related model */

    bool isLoaded = false;    /**< @brief True if related data has been fetched from DB */

    /** @brief Internal factory to create relation data associated with a parent model */
    static QExplicitlySharedDataPointer<RelationData> fromParentModel(const QString &name, Model *parent, const std::function<RelationData *()> &creationCallback);
};

/**
 * @brief Typed base class for relation data.
 * @tparam RelatedModel The type of the related model.
 */
template<typename RelatedModel>
class RelationBaseData : public RelationData
{
public:
    RelationBaseData() { relatedObject = MetaObject::from<RelatedModel>(); }
    virtual ~RelationBaseData() = default;

    /** @brief Returns true if any related models have been fetched */
    bool exists() override { return related.count() > 0; }

    QList<RelatedModel> related; /**< @brief The list of related model instances */
};

/**
 * @brief User-facing class for managing model relationships.
 * 
 * Relationships are accessed as methods on the model:
 * @code
 * Relation<Stock> stock = product.stock();
 * if (stock) {
 *     qDebug() << stock->quantity;
 * }
 * @endcode
 * 
 * @tparam Model The type of the related model.
 */
template<typename Model>
class Relation : public Entity
{
    using ParentModel = QEloquent::Model;

public:
    /** @brief Default constructor (uninitialized) */
    Relation() {}
    /** @brief Internal constructor used by Model factory methods */
    Relation(const QString &name, ParentModel *parent, const std::function<RelationData *()> &creationCallback)
        : data(RelationData::fromParentModel(name, parent, creationCallback)) {}
    /** @brief Internal constructor */
    Relation(const QString &name, ParentModel *parent)
        : data(RelationData::fromParentModel(name, parent, [] { return nullptr; })) {}
    /** @brief Internal constructor from raw data pointer */
    Relation(RelationData *data) : data(data) {}
    /** @brief Copy constructor */
    Relation(const Relation &other) = default;
    /** @brief Move constructor */
    Relation(Relation &&other) = default;

    /** @brief Copy assignment operator */
    Relation<Model> &operator=(const Relation &other) = default;
    /** @brief Move assignment operator */
    Relation<Model> &operator=(Relation &&other) = default;

    /** @brief Check if associated metadata exists at a given index */
    bool hasAssociatedData(int index, const QString &name) const
    { return data->associatedData.value(index).contains(name); }

    /** @brief Retrieve associated metadata */
    QVariant associatedData(int index, const QString &name) const
    { return data->associatedData.value(index).value(name); }

    /** @brief Set associated metadata */
    void setAssociatedData(int index, const QString &name, const QVariant &value)
    { data->associatedData[index].insert(name, value); }

    /** @brief List of metadata keys available */
    QStringList associatedDataNames() const
    { return (data->associatedData.isEmpty() ? QStringList() : data->associatedData.value(data->associatedData.firstKey()).keys()); }

    /** @brief Returns true if related models exist in the database (triggers load) */
    bool exists() override { ensureLoaded(); return data->exists(); }
    /** @brief Manually fetches related models from the database */
    bool get() override { data->isLoaded = true; return data->get(); }
    /** @brief Unsupported for relations directly */
    bool save() override { return data->save(); }
    /** @brief Unsupported for relations directly */
    bool insert() override { return data->insert(); }
    /** @brief Unsupported for relations directly */
    bool update() override { return data->update(); }
    /** @brief Unsupported for relations directly */
    bool deleteData() override { return data->deleteData(); }

    /** @brief Internal helper to trigger lazy loading if needed */
    void ensureLoaded() const {
        if (!data->isLoaded) {
            const_cast<Relation*>(this)->get();
        }
    }

    typedef typename QList<Model>::iterator iterator;
    typedef typename QList<Model>::const_iterator const_iterator;

    /** @brief Returns an iterator to the beginning (triggers load) */
    iterator begin() { ensureLoaded(); return const_cast<RelationBaseData<Model> *>(static_cast<const RelationBaseData<Model> *>(data.get()))->related.begin(); }
    /** @brief Returns an iterator to the end (triggers load) */
    iterator end() { ensureLoaded(); return const_cast<RelationBaseData<Model> *>(static_cast<const RelationBaseData<Model> *>(data.get()))->related.end(); }
    /** @brief Returns a const iterator to the beginning (triggers load) */
    const_iterator begin() const { ensureLoaded(); return static_cast<const RelationBaseData<Model> *>(data.get())->related.begin(); }
    /** @brief Returns a const iterator to the end (triggers load) */
    const_iterator end() const { ensureLoaded(); return static_cast<const RelationBaseData<Model> *>(data.get())->related.end(); }
    /** @brief Returns a const iterator to the beginning (triggers load) */
    const_iterator cbegin() const { ensureLoaded(); return static_cast<const RelationBaseData<Model> *>(data.get())->related.cbegin(); }
    /** @brief Returns a const iterator to the end (triggers load) */
    const_iterator cend() const { ensureLoaded(); return static_cast<const RelationBaseData<Model> *>(data.get())->related.cend(); }

    /** @brief Returns the number of related models (triggers load) */
    int count() const { ensureLoaded(); return static_cast<const RelationBaseData<Model> *>(data.get())->related.count(); }
    /** @brief Returns the number of related models (triggers load) */
    int size() const { return count(); }
    /** @brief Returns true if no related models exist (triggers load) */
    bool isEmpty() const { return count() == 0; }

    /** @brief Access a related model at a given index */
    const Model &at(int i) const { ensureLoaded(); return static_cast<const RelationBaseData<Model> *>(data.get())->related.at(i); }
    /** @brief Access a related model at a given index */
    const Model &operator[](int i) const { return at(i); }

    /** @brief Returns the first related model or a default instance if none found */
    Model first() const { return count() > 0 ? at(0) : Model(); }
    /** @brief Returns the last related model or a default instance if none found */
    Model last() const { return count() > 0 ? at(count() - 1) : Model(); }

    /** @brief Member access operator pointing to the first related model */
    Model* operator->() {
        ensureLoaded();
        RelationBaseData<Model> *d = const_cast<RelationBaseData<Model> *>(static_cast<const RelationBaseData<Model> *>(data.get()));
        if (d->related.isEmpty()) return nullptr;
        return &d->related.first();
    }

    /** @brief Const member access operator pointing to the first related model */
    const Model* operator->() const {
        ensureLoaded();
        const RelationBaseData<Model> *d = static_cast<const RelationBaseData<Model> *>(data.get());
        if (d->related.isEmpty()) return nullptr;
        return &d->related.first();
    }

    /** @brief Dereference operator for the first related model */
    Model& operator*() { return *operator->(); }
    /** @brief Const dereference operator for the first related model */
    const Model& operator*() const { return *operator->(); }

    /** @brief Implicit conversion to the related Model type (returns first()) */
    operator Model() const { return first(); }

    /** @brief Explicitly returns the list of related models */
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
