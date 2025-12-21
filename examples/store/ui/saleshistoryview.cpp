#include "saleshistoryview.h"
#include "../models/sale.h"
#include "../models/user.h"
#include <QHeaderView>
#include <QLabel>

namespace Store {

SalesHistoryView::SalesHistoryView(QWidget *parent) : QWidget(parent)
{
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(40, 40, 40, 40);

    auto title = new QLabel("Sales History", this);
    title->setStyleSheet("font-size: 24px; font-weight: bold; color: #333; margin-bottom: 20px;");
    layout->addWidget(title);

    m_table = new QTableWidget(this);
    m_table->setColumnCount(4);
    m_table->setHorizontalHeaderLabels({"Sale #", "Amount", "Seller", "Date"});
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setAlternatingRowColors(true);
    m_table->setStyleSheet("QTableWidget { background-color: gray; border-radius: 8px; border: 1px solid #ddd; }");

    layout->addWidget(m_table);

    refresh();
}

void SalesHistoryView::refresh()
{
    m_table->setRowCount(0);

    // QEloquent: Fetch all sales and eager load the seller relationship
    auto salesResult = Sale::all(Sale::query().with(QString("seller")).orderBy("id", Qt::DescendingOrder));
    if (!salesResult) return;

    auto sales = salesResult.value();
    m_table->setRowCount(sales.size());

    for (int i = 0; i < sales.size(); ++i) {
        const auto &s = sales[i];
        m_table->setItem(i, 0, new QTableWidgetItem(QString::number(s.number)));
        m_table->setItem(i, 1, new QTableWidgetItem(QString::number(s.amount, 'f', 2) + " $"));
        m_table->setItem(i, 2, new QTableWidgetItem(s.seller()->name));
        
        // Note: created_at is not in our model yet but we could add it if needed.
        // For now just placeholders or skip.
        m_table->setItem(i, 3, new QTableWidgetItem("N/A"));
    }
}

} // namespace Store
