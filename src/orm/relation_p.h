#ifndef QELOQUENT_RELATION_P_H
#define QELOQUENT_RELATION_P_H

#include <QEloquent/global.h>
#include <QEloquent/entity.h>
#include <QEloquent/serializable.h>
#include <QEloquent/datamap.h>
#include <QEloquent/metaobject.h>

#include <source_location>

namespace QEloquent {

class Query;
class Error;

class QELOQUENT_EXPORT RelationData : public QSharedData,
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

} // namespace QEloquent

#endif // QELOQUENT_RELATION_P_H
