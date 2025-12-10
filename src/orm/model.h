#ifndef QELOQUENT_MODEL_H
#define QELOQUENT_MODEL_H

#include <QEloquent/global.h>
#include <QEloquent/entity.h>

#include <QObject>
#include <QSharedDataPointer>

class QSqlRecord;

namespace QEloquent {

class MetaObject;
class Query;
class Error;
class Connection;

template<typename T>
using Relation = QList<T>;

class ModelData;
class QELOQUENT_EXPORT Model : public Entity
{
    Q_GADGET

public:
    Model(const Model &);
    Model(Model &&);
    Model &operator=(const Model &);
    Model &operator=(Model &&);
    virtual ~Model();

    QVariant primary() const;
    void setPrimary(const QVariant &value);

    QVariant property(const QString &name) const;
    void setProperty(const QString &name, const QVariant &value);

    void fill(const QJsonObject &object);
    void fill(const QSqlRecord &record);

    bool exists() const override;
    bool get() override;
    bool insert() override;
    bool update() override;
    bool deleteData() override;

    bool load(const QString &relation);
    bool load(const QStringList &relations);

    Query lastQuery() const;
    Error lastError() const;

    MetaObject metaObject() const;
    Connection connection() const;

    QJsonObject toJsonObject() const;
    QSqlRecord toSqlRecord() const;

protected:
    template<typename T, std::enable_if<std::is_base_of<Model, T>::value>::type* = nullptr> Model(T *self);
    Model(const QMetaObject &metaObject);
    Model(ModelData *data);

    template<typename T>
    Relation<T> hasOne(const QString &foreignKey = QString(), const QString &localKey = QString()) const
    { return Relation<T>(); }

    QSharedDataPointer<ModelData> data;

private:
    //RelationBase relation(RelationImpl *) const;

    Query newQuery(bool filter = true) const;

    friend class MetaProperty;
    friend class MetaRelation;
};

template<typename T, std::enable_if<std::is_base_of<Model, T>::value>::type*>
inline Model::Model(T *) : Model(T::staticMetaObject) {}

} // namespace QELoquent

#endif // QELOQUENT_MODEL_H
