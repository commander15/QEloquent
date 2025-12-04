#ifndef QELOQUENT_MODELINFOBUILDER_H
#define QELOQUENT_MODELINFOBUILDER_H

#include <QEloquent/global.h>
#include <QEloquent/modelinfo.h>

#include <QSharedDataPointer>

namespace QEloquent {

class Connection;

class ModelInfoBuilderData;
class QELOQUENT_EXPORT ModelInfoBuilder
{
public:
    ModelInfoBuilder();
    ModelInfoBuilder(const QString &table);
    ~ModelInfoBuilder();

    ModelInfoBuilder &table(const QString &name);
    ModelInfoBuilder &primaryKey(const QString &field);
    ModelInfoBuilder &foreignKey(const QString &field);

    ModelInfoBuilder &hidden(const QString &field);
    ModelInfoBuilder &hidden(const QStringList &fields);

    ModelInfoBuilder &fillable(const QString &field);
    ModelInfoBuilder &fillable(const QStringList &fields);

    ModelInfoBuilder &metaObject(const QMetaObject *object);

    bool isValid() const;

    void loadJson(const QJsonObject &object);

    ModelInfo build(const Connection &connection) const;

protected:
    QSharedDataPointer<ModelInfoBuilderData> data;
};

} // namespace QEloquent

#endif // QELOQUENT_MODELINFOBUILDER_H
