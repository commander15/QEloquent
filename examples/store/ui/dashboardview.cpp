#include "dashboardview.h"
#include "../models/sale.h"
#include "../models/product.h"
#include <QEloquent/query.h>
#include <QEloquent/queryrunner.h>

namespace Store {

DashboardView::DashboardView(QWidget *parent) : QWidget(parent)
{
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(40, 40, 40, 40);
    layout->setSpacing(30);

    auto title = new QLabel("Store Dashboard", this);
    title->setStyleSheet("font-size: 32px; font-weight: bold; color: #333;");
    layout->addWidget(title);

    auto cardsLayout = new QHBoxLayout();
    cardsLayout->setSpacing(20);

    cardsLayout->addWidget(createStatCard("Total Sales", m_totalSalesLabel, "#3498db"));
    cardsLayout->addWidget(createStatCard("Monthly Revenue", m_totalRevenueLabel, "#2ecc71"));
    cardsLayout->addWidget(createStatCard("Low Stock Items", m_lowStockLabel, "#e74c3c"));

    layout->addLayout(cardsLayout);
    layout->addStretch();

    refresh();
}

QWidget* DashboardView::createStatCard(const QString &title, QLabel *&valueLabel, const QString &color)
{
    auto card = new QFrame();
    card->setObjectName("StatCard");
    card->setFrameShape(QFrame::StyledPanel);
    card->setFixedSize(250, 150);
    card->setStyleSheet(QString(
        "#StatCard { "
        "  background-color: white; "
        "  border-radius: 12px; "
        "  border-left: 10px solid %1; "
        "  border: 1px solid #ddd; "
        "}"
    ).arg(color));

    auto cardLayout = new QVBoxLayout(card);
    auto titleLabel = new QLabel(title);
    titleLabel->setStyleSheet("color: #7f8c8d; font-size: 14px; text-transform: uppercase; font-weight: bold;");
    
    valueLabel = new QLabel("0");
    valueLabel->setStyleSheet(QString("color: %1; font-size: 36px; font-weight: bold;").arg(color));

    cardLayout->addWidget(titleLabel);
    cardLayout->addWidget(valueLabel);
    cardLayout->addStretch();

    return card;
}

void DashboardView::refresh()
{
    // QEloquent usage: using static helpers from ModelHelpers
    auto saleCount = Sale::count();
    m_totalSalesLabel->setText(QString::number(saleCount ? saleCount.value() : 0));

    // For aggregate SUM, use QueryRunner directly for now
    auto revenueResult = QEloquent::QueryRunner::exec("SELECT SUM(amount) FROM Sales");
    double totalRevenue = (revenueResult && revenueResult->next()) ? revenueResult->value(0).toDouble() : 0.0;
    m_totalRevenueLabel->setText(QString::number(totalRevenue, 'f', 2) + " $");

    auto lowStockCount = Stock::count(Stock::query().where("quantity", "<", 10));
    m_lowStockLabel->setText(QString::number(lowStockCount ? lowStockCount.value() : 0));
}

} // namespace Store
