#include "metatable.h"

#include <QEloquent/namingconvention.h>

#include <utility>

namespace QEloquent {

class MetaTableData : public QSharedData
{
public:
    //
};

QString AbstractMetaTable::className(const QString &tableName) const
{
    return QString();
}

QString AbstractMetaTable::tableName(const QString &className) const
{
    return NamingConvention::convention(m_namingConvention)->tableName(className);
}

QString AbstractMetaTable::propertyName(const QString &fieldName) const
{
    return NamingConvention::convention(m_namingConvention)->propertyName(fieldName);
}

QString AbstractMetaTable::fieldName(const QString &propertyName) const
{
    return NamingConvention::convention(m_namingConvention)->fieldName(propertyName);
}

MetaTable::MetaTable()
    : data(new MetaTableData)
{}

MetaTable::MetaTable(const MetaTable &rhs)
    : data{rhs.data}
{}

MetaTable::MetaTable(MetaTable &&rhs)
    : data{std::move(rhs.data)}
{}

MetaTable &MetaTable::operator=(const MetaTable &rhs)
{
    if (this != &rhs)
        data = rhs.data;
    return *this;
}

MetaTable &MetaTable::operator=(MetaTable &&rhs)
{
    if (this != &rhs)
        data = std::move(rhs.data);
    return *this;
}

MetaTable::~MetaTable() {}

}
