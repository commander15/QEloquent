#ifndef QELOQUENT_SCHEMAGRAMMAR_H
#define QELOQUENT_SCHEMAGRAMMAR_H

#include <QEloquent/global.h>
#include <QEloquent/columndata.h>

namespace QEloquent {

class Query;
class DataMap;
class Driver;

class QELOQUENT_EXPORT SchemaGrammar
{
public:
    using ColumnType = ColumnData::ColumnType;
    using NumberSign = ColumnData::NumberSign;

    enum GrammarFeature {
        NumberSignSupport,
        FullAlterColumnDefinitionSupport,
    };

    Q_DECLARE_FLAGS(GrammarFeatures, GrammarFeature);

    SchemaGrammar(Driver *d) : driver(d) {}
    virtual ~SchemaGrammar() = default;

    virtual bool hasFeature(GrammarFeature feature) const = 0;

    virtual QPair<ColumnType, NumberSign> primaryKeyType() const = 0;
    virtual QString autoIncrementKeyword() const = 0;
    virtual ColumnData::Constraints inlineConstraints() const;
    virtual ColumnData::Constraints namedConstraints() const;

    virtual QStringList createTableStatements(const TableBlueprint &blueprint) const;
    virtual QStringList alterTableStatements(const TableBlueprint &blueprint) const;
    virtual QStringList renameTableStatements(const QString &from, const QString &to) const;
    virtual QStringList dropTableStatements(const QString &table, bool ifExists) const;

    QString columnDefinition(const ColumnData &column, bool full) const;
    virtual QString columnType(ColumnType type, int length = -1, int precision = -1) const;

    QStringList constraintsDefinitions(const ColumnData &column, ColumnData::Constraints supported, bool named) const;

    virtual QStringList addColumnStatements(const ColumnData &column) const;
    virtual QStringList renameColumnStatements(const QString &from, const QString &to, const QString &table) const;
    virtual QStringList dropColumnStatements(const QString &column, const QString &table) const;

    virtual QStringList createIndexStatements(const QString &index, const QStringList &columns, const QString &table, bool unique) const;
    virtual QStringList dropIndexStatements(const QString &index, const QString &table) const;

    static SchemaGrammar *createDefault(Driver *driver);

protected:
    QString fieldName(const ColumnData &column, bool escaped = true) const;
    QString tableName(const ColumnData &column, bool escaped = true) const;

    Driver *driver;
};

} // namespace QEloquent

#endif // QELOQUENT_SCHEMAGRAMMAR_H
