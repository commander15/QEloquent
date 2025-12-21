#ifndef SALESHISTORYVIEW_H
#define SALESHISTORYVIEW_H

#include <QWidget>
#include <QTableWidget>
#include <QVBoxLayout>

namespace Store {

class SalesHistoryView : public QWidget
{
    Q_OBJECT
public:
    explicit SalesHistoryView(QWidget *parent = nullptr);
    void refresh();

private:
    QTableWidget *m_table;
};

} // namespace Store

#endif // SALESHISTORYVIEW_H
