#ifndef PRODUCTCATALOGVIEW_H
#define PRODUCTCATALOGVIEW_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>

namespace Store {

class ProductCatalogView : public QWidget
{
    Q_OBJECT
public:
    explicit ProductCatalogView(QWidget *parent = nullptr);
    void refresh();

private:
    QTableWidget *m_table;
};

} // namespace Store

#endif // PRODUCTCATALOGVIEW_H
