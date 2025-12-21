#include "productcatalogview.h"
#include "../models/product.h"
#include <QHeaderView>
#include <QLabel>

namespace Store {

ProductCatalogView::ProductCatalogView(QWidget *parent) : QWidget(parent)
{
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(40, 40, 40, 40);

    auto title = new QLabel("Product Catalog", this);
    title->setStyleSheet("font-size: 24px; font-weight: bold; color: #333; margin-bottom: 20px;");
    layout->addWidget(title);

    m_table = new QTableWidget(this);
    m_table->setColumnCount(5);
    m_table->setHorizontalHeaderLabels({"ID", "Name", "Category", "Price", "Stock"});
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setAlternatingRowColors(true);
    m_table->setStyleSheet("QTableWidget { background-color: #333; border-radius: 8px; border: 1px solid #ddd; }");

    layout->addWidget(m_table);

    refresh();
}

void ProductCatalogView::refresh()
{
    m_table->setRowCount(0);

    // QEloquent: Eager load stock and category for efficient listing
    auto productsResult = Product::all(Product::query().with(QString("stock")));
    if (!productsResult) return;

    auto products = productsResult.value();
    m_table->setRowCount(products.size());

    for (int i = 0; i < products.size(); ++i) {
        const auto &p = products[i];
        m_table->setItem(i, 0, new QTableWidgetItem(QString::number(p.id)));
        m_table->setItem(i, 1, new QTableWidgetItem(p.name));
        
        // Relation access (hasMany -> belongsTo)
        // Note: For simplicity, we just display the ID or fetch if needed.
        // Eager loading already populated them if 'with' was used correctly.
        m_table->setItem(i, 2, new QTableWidgetItem(p.category()->name));
        m_table->setItem(i, 3, new QTableWidgetItem(QString::number(p.price, 'f', 2) + " $"));
        m_table->setItem(i, 4, new QTableWidgetItem(QString::number(p.stock()->quantity)));
    }
}

} // namespace Store
