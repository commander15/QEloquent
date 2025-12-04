#ifndef METATABLE_H
#define METATABLE_H

#include <QEloquent/global.h>

#include <QSharedDataPointer>

class QSqlRecord;

namespace QEloquent {

class QELOQUENT_EXPORT AbstractMetaTable
{
public:
    virtual ~AbstractMetaTable() = default;

    virtual QString className() const = 0;
    virtual QString tableName() const
    { return tableName(className()); }

    virtual QString primaryProperty() const = 0;
    virtual QString primaryField() const
    { return fieldName(primaryProperty()); }
    inline QString primaryKey() const
    { return primaryField(); }

    virtual QString foreignProperty() const = 0;
    virtual QString foreignField() const
    { return fieldName(foreignProperty()); }
    inline QString foreignKey() const
    { return foreignField(); }

    virtual QStringList fillableProperties() const = 0;
    virtual QStringList fillableFields() const;

    virtual QStringList hiddenProperties() const = 0;
    virtual QStringList hiddenFields() const;

    virtual QStringList properties() const = 0;
    virtual QStringList fields() const;

    bool hasCreatedAt() const
    { return !createdAtProperty().isEmpty(); }
    virtual QString createdAtProperty() const = 0;
    QString createdAtField() const
    { return fieldName(createdAtProperty()); }

    bool hasUpdatedAt() const
    { return !updatedAtProperty().isEmpty(); }
    virtual QString updatedAtProperty() const = 0;
    QString updatedAtField() const
    { return fieldName(updatedAtProperty()); }

    bool hasDeletedAt() const
    { return !deletedAtProperty().isEmpty(); }
    virtual QString deletedAtProperty() const = 0;
    QString deletedAtField() const
    { return fieldName(deletedAtProperty()); }

    QString className(const QString &tableName) const;
    QString tableName(const QString &className) const;
    QString propertyName(const QString &fieldName) const;
    QString fieldName(const QString &propertyName) const;

private:
    QString m_namingConvention;
};

class MetaTableData;
class QELOQUENT_EXPORT MetaTable : public AbstractMetaTable
{
public:
    MetaTable();
    MetaTable(const MetaTable &);
    MetaTable(MetaTable &&);
    MetaTable &operator=(const MetaTable &);
    MetaTable &operator=(MetaTable &&);
    ~MetaTable();

private:
    QSharedDataPointer<MetaTableData> data;
};

}

#endif // METATABLE_H
