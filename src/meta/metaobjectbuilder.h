#ifndef QELOQUENT_METAOBJECTBUILDER_H
#define QELOQUENT_METAOBJECTBUILDER_H

#include <QEloquent/global.h>
#include <QEloquent/metatable.h>

namespace QEloquent {

class MetaObjectBuilderPrivate;
class QELOQUENT_EXPORT MetaObjectBuilder
{
public:
    MetaObjectBuilder(const QString &className);
    ~MetaObjectBuilder();

    MetaObjectBuilder &table(const QString &name);

    MetaObjectBuilder &primary(const QString &property);
    MetaObjectBuilder &foreign(const QString &property);

    MetaObjectBuilder &hidden(const QString &property);
    MetaObjectBuilder &hidden(const QStringList &properties);

    MetaObjectBuilder &fillable(const QString &property);
    MetaObjectBuilder &fillable(const QStringList &properties);

    MetaObjectBuilder &timestamps(const QString &creationProperty, const QString &updateProperty);
    MetaObjectBuilder &timestamps(const QString &creationProperty, const QString &updateProperty, const QString &deletionProperty);

    MetaObjectBuilder &property(const QString &name, const QMetaType &type);
    MetaObjectBuilder &property(const QString &name, const QString &field, const QMetaType &type);

    MetaObjectBuilder &with(const QString &relation);
    MetaObjectBuilder &with(const QStringList &relations);

    MetaObjectBuilder &naming(const QString &name);
    MetaObjectBuilder &connection(const QString &name);

    MetaObject build();

private:
    QSharedDataPointer<MetaObjectBuilderPrivate> d;
};

} // namespace QEloquent

#endif // QELOQUENT_METAOBJECTBUILDER_H
