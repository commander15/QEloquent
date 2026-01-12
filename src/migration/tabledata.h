#ifndef QELOQUENT_TABLEDATA_H
#define QELOQUENT_TABLEDATA_H

#include <QEloquent/global.h>
#include <QEloquent/tableblueprint.h>

namespace QEloquent {

class QELOQUENT_EXPORT TableData
{
public:
    TableData();
    TableData(const TableData &);
    TableData(TableData &&);
    TableData &operator=(const TableData &);
    TableData &operator=(TableData &&);
    ~TableData();

    QString tableName() const;
    bool newTable() const;

    QList<ColumnData> columns() const;
    QList<QPair<QString, QStringList>> indexes() const;

protected:
    TableData(const QExplicitlySharedDataPointer<TableBlueprintPrivate> &d);

private:
    QExplicitlySharedDataPointer<TableBlueprintPrivate> data;

    friend class TableBlueprint;
};

} // namespace QEloquent

#endif // QELOQUENT_TABLEDATA_H
