#ifndef QELOQUENT_RELATION_H
#define QELOQUENT_RELATION_H

#include <QEloquent/global.h>
#include <QEloquent/entity.h>
#include <QEloquent/serializable.h>
#include <QEloquent/metaobject.h>
#include <QEloquent/itemproxy.h>
#include <QEloquent/listproxy.h>
#include <QEloquent/datamap.h>

#include <QList>

#include <source_location>

namespace QEloquent {

class Query;
class Error;

class RelationData : public QSharedData,
                     public Entity,
                     public Serializable
{
public:
    RelationData();
    RelationData(const RelationData &other);
    virtual ~RelationData();

    virtual void init(NamingConvention *convention) = 0;
    virtual bool multiple() const = 0;

    // No need for full CRUD for now, insert/update not handled separately
    bool save() override { return false; }
    bool insert() override final { return save(); }
    bool update() override final { return save(); };
    bool deleteData() override { return false; }

    template<typename Model> QList<Model> &relatedModels() const
    { return *static_cast<QList<Model>*>(relatedList()); }
    virtual void *relatedList() const = 0;

    QString serializationContext() const override final;
    bool isListSerializable() const override final { return multiple(); }

    virtual RelationData *clone() const = 0;

    QString name;
    QMap<int, DataMap> pivotData;
    MetaObject primaryObject;
    MetaObject relatedObject;
    Model *parent = nullptr;

    bool isLoaded = false;

    static QExplicitlySharedDataPointer<RelationData> create(const QString &name, const Model *parent, const std::function<RelationData *()> &creationCallback);
    static QExplicitlySharedDataPointer<RelationData> create(const std::source_location &location, const Model *parent, const std::function<RelationData *()> &creationCallback);

protected:
    QVariant parentPrimary() const;
    void setParentPrimary(const QVariant &value);

    QVariant parentField(const QString &name) const;
    void setParentField(const QString &name, const QVariant &value);

    void conserve(const Query &query);
    void conserve(const Query &query, const Error &error);
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
class Relation : public Entity,
                 public AbstractItemProxy<Model>,
                 public AbstractListProxy<Model>,
                 public Serializable
{
    using ParentModel = QEloquent::Model;
    using RelatedModel = Model;

public:
    /** @brief Default constructor (uninitialized) */
    Relation() {}
    /** @brief Internal constructor used by Model factory methods */
    Relation(const QString &name, ParentModel *parent, const std::function<RelationData *()> &creationCallback)
        : data(RelationData::create(name, parent, creationCallback)) { ensureLoaded(); }
    /** @brief Constructor */
    Relation(const std::source_location &location, const ParentModel *parent, const std::function<RelationData *()> &creationCallback)
        : data(RelationData::create(location, parent, creationCallback)) { ensureLoaded(); }
    /** @brief Copy constructor */
    Relation(const Relation &other) = default;
    /** @brief Move constructor */
    Relation(Relation &&other) = default;
    /** @brief Copy assignment operator */
    Relation<Model> &operator=(const Relation &other) = default;
    /** @brief Move assignment operator */
    Relation<Model> &operator=(Relation &&other) = default;

    /** @brief Check if associated metadata exists at a given index */
    bool hasPivotData(int index, const QString &name) const
    { return data->pivotData.value(index).contains(name); }

    /** @brief Retrieve associated metadata */
    QVariant pivotData(int index, const QString &name) const
    { return data->pivotData.value(index).value(name); }

    /** @brief Set associated metadata */
    void setPivotData(int index, const QString &name, const QVariant &value)
    { data->pivotData[index].insert(name, value); }

    /** @brief Returns true if related models exist in the database (triggers load) */
    bool exists() override { ensureLoaded(); return data->exists(); }
    /** @brief Manually fetches related models from the database */
    bool get() override { ensureLoaded(); return data->get(); }
    /** @brief Unsupported for relations directly */
    bool save() override { return data->save(); }
    /** @brief Unsupported for relations directly */
    bool insert() override { return data->insert(); }
    /** @brief Unsupported for relations directly */
    bool update() override { return data->update(); }
    /** @brief Unsupported for relations directly */
    bool deleteData() override { return data->deleteData(); }

    /** @brief Explicitly returns the list of related models */
    QList<RelatedModel> related() const { ensureLoaded(); return data->relatedModels<RelatedModel>(); }

    QString serializationContext() const override final { return data->serializationContext(); }
    bool isListSerializable() const override final { return data->multiple(); }

    QList<DataMap> serialize() const override final {
        QList<DataMap> maps;
        const QList<RelatedModel> &models = data->relatedModels<RelatedModel>();

        std::transform(models.begin(), models.end(), std::back_inserter(maps), [](const Serializable &model) {
            const QList<DataMap> maps = model.serialize();
            // Model always return a single map
            return maps.first();
        });

        return maps;
    }

private:
    /** @brief Internal helper to trigger lazy loading if needed */
    void ensureLoaded() const {
        if (!data->isLoaded) {
            data->get();
            data->isLoaded = true;
        }
    }

    const RelatedModel *constItem() const override {
        ensureLoaded();
        const QList<RelatedModel> *list = this->constList();
        return (list->isEmpty() ? this->defaultItem() : &list->first());
    }

    RelatedModel *mutableItem() override {
        ensureLoaded();
        QList<RelatedModel> *list = this->mutableList();
        return (list->isEmpty() ? this->defaultItem() : &list->first());
    }

    RelatedModel *defaultItem() const {
        static RelatedModel m;
        m = RelatedModel(); // We clear each time
        return &m;
    }

    const QList<RelatedModel> *constList() const override {
        ensureLoaded();
        return &data->relatedModels<RelatedModel>();
    }

    QList<RelatedModel> *mutableList() override {
        ensureLoaded();
        return &data->relatedModels<RelatedModel>();
    }

    QExplicitlySharedDataPointer<RelationData> data;
};

} // namespace QEloquent

Q_DECLARE_SEQUENTIAL_CONTAINER_METATYPE(QEloquent::Relation)

#endif // QELOQUENT_RELATION_H
