#ifndef QELOQUENT_RELATION_H
#define QELOQUENT_RELATION_H

#include <QEloquent/global.h>
#include <QEloquent/entity.h>
#include <QEloquent/metaobject.h>

#include <QList>
#include <QVariant>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>

#include <source_location>

namespace QEloquent {

class Query;
class Error;
class ModelData;

class RelationData : public Entity, public QSharedData
{
public:
    RelationData();
    RelationData(const RelationData &other) = default;
    virtual ~RelationData();

    virtual void init(NamingConvention *convention) = 0;
    virtual bool multiple() const = 0;

    virtual void fill(const QJsonValue &value) = 0;
    virtual QJsonValue extract() const = 0;

    // No need for full CRUD for now
    bool insert() override { return false; }
    bool update() override { return false; };
    bool deleteData() override { return false; }

    virtual RelationData *clone() const = 0;

    QString name;
    Model *parent = nullptr;
    QMap<int, QVariantMap> associatedData;

    MetaObject primaryObject;
    MetaObject relatedObject;

    bool isLoaded = false;

    static QExplicitlySharedDataPointer<RelationData> fromParentModel(const QString &name, Model *parent, const std::function<RelationData *()> &creationCallback);
    static QExplicitlySharedDataPointer<RelationData> fromParentModel(const std::source_location &location, Model *parent, const std::function<RelationData *()> &creationCallback);

protected:
    QVariant parentPrimary() const;
    void setParentPrimary(const QVariant &value);

    QVariant parentField(const QString &name) const;
    void setParentField(const QString &name, const QVariant &value);

    void conserve(const Query &query);
    void conserve(const Query &query, const Error &error);
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

    void fill(const QJsonValue &value) override
    {
        if (value.type() == QJsonValue::Object) {
            RelatedModel m;
            m.fill(value.toObject());
            related = { m };
            return;
        }

        if (value.type() == QJsonValue::Array) {
            related.clear();

            const QJsonArray array = value.toArray();
            for (const QJsonValue &value : array) {
                RelatedModel m;
                m.fill(value.toObject());
                related.append(m);
            }
            return;
        }
    }

    QJsonValue extract() const override
    {
        if (!multiple()) {
            return (related.isEmpty() ? QJsonObject() : related.first().toJsonObject());
        } else {
            QJsonArray array;
            for (const RelatedModel &m : related)
                array.append(m.toJsonObject());
            return array;
        }
    }

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
    /** @brief Constructor */
    Relation(const std::source_location &location, ParentModel *parent, const std::function<RelationData *()> &creationCallback)
        : data(RelationData::fromParentModel(location, parent, creationCallback)) {}
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

    QJsonObject toJsonObhect() const
    { return data->extract().toObject(); }

    QJsonArray toJsonArray() const
    { return data->extract().toArray(); }

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
