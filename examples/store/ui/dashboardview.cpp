#include "dashboardview.h"
#include "session.h"
#include "../models/product.h"
#include "../models/sale.h"
#include <QEloquent/queryrunner.h>
#include <QDateTime>
#include <QHBoxLayout>
#include <QFrame>
#include <QHeaderView>

namespace Store {

DashboardView::DashboardView(QWidget *parent) : QWidget(parent)
{
    setupUi();
    refresh();
}

void DashboardView::setupUi()
{
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(30, 30, 30, 30);
    layout->setSpacing(20);

    // Header Row
    auto headerLayout = new QHBoxLayout();
    auto title = new QLabel("DASHBOARD OVERVIEW", this);
    title->setStyleSheet("font-size: 20px; font-weight: bold; color: #7f8c8d; letter-spacing: 1px;");
    
    m_dateLabel = new QLabel(this);
    m_dateLabel->setStyleSheet("font-size: 16px; color: #95a5a6;");
    m_dateLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    
    headerLayout->addWidget(title);
    headerLayout->addStretch();
    headerLayout->addWidget(m_dateLabel);
    layout->addLayout(headerLayout);

    // User Welcome Row
    m_userNameLabel = new QLabel(this);
    m_userNameLabel->setStyleSheet("font-size: 28px; font-weight: 300; color: #2c3e50; margin-top: 10px;");
    layout->addWidget(m_userNameLabel);

    // Stats Row
    auto statsLayout = new QHBoxLayout();
    statsLayout->setSpacing(20);
    statsLayout->addWidget(createStatCard("Total Products", m_totalProductsLabel, "📦", "#3498db"));
    statsLayout->addWidget(createStatCard("Low Stock Items", m_lowStockLabel, "⚠️", "#e67e22"));
    statsLayout->addWidget(createStatCard("Today's Revenue", m_dailyRevenueLabel, "💰", "#27ae60"));
    layout->addLayout(statsLayout);

    // Top Products Table
    auto tableContainer = new QFrame();
    tableContainer->setStyleSheet("background-color: white; border-radius: 10px; border: 1px solid #dfe6e9;");
    auto tableLayout = new QVBoxLayout(tableContainer);
    
    auto tableTitle = new QLabel("TOP 10 BEST SELLING PRODUCTS", this);
    tableTitle->setStyleSheet("font-size: 14px; font-weight: bold; color: #2d3436; padding: 5px;");
    tableLayout->addWidget(tableTitle);

    m_topProductsTable = new QTableWidget(this);
    m_topProductsTable->setColumnCount(4);
    m_topProductsTable->setHorizontalHeaderLabels({"Product Name", "Units Sold", "Category", "Revenue"});
    m_topProductsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_topProductsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_topProductsTable->setAlternatingRowColors(true);
    m_topProductsTable->setStyleSheet("QTableWidget { border: none; } QHeaderView::section { background-color: #f8f9fa; font-weight: bold; border: none; }");
    tableLayout->addWidget(m_topProductsTable);

    layout->addWidget(tableContainer);
}

QWidget* DashboardView::createStatCard(const QString &title, QLabel *&valueLabel, const QString &icon, const QString &color)
{
    auto card = new QFrame();
    card->setFixedSize(280, 120);
    card->setStyleSheet(QString(
        "QFrame { background-color: white; border-radius: 10px; border: 1px solid #dfe6e9; }"
        "QLabel { border: none; }"
    ));

    auto cardLayout = new QVBoxLayout(card);
    
    auto topLayout = new QHBoxLayout();
    auto iconLabel = new QLabel(icon);
    iconLabel->setStyleSheet(QString("font-size: 24px; color: %1;").arg(color));
    auto titleLabel = new QLabel(title);
    titleLabel->setStyleSheet("font-size: 13px; font-weight: bold; color: #636e72; text-transform: uppercase;");
    topLayout->addWidget(iconLabel);
    topLayout->addWidget(titleLabel);
    topLayout->addStretch();
    
    valueLabel = new QLabel("0");
    valueLabel->setStyleSheet(QString("font-size: 32px; font-weight: bold; color: %1;").arg(color));

    cardLayout->addLayout(topLayout);
    cardLayout->addWidget(valueLabel);
    
    return card;
}

void DashboardView::refresh()
{
    // Date and User
    m_dateLabel->setText(QDateTime::currentDateTime().toString("dddd, MMMM d, yyyy"));
    m_userNameLabel->setText("Welcome back, <b>" + Session::instance().user().name + "</b>!");

    // Stats
    auto prodCount = Product::count();
    m_totalProductsLabel->setText(QString::number(prodCount.value_or(0)));

    auto lowStockCount = Stock::count(Stock::query().where("quantity", "<", 10));
    m_lowStockLabel->setText(QString::number(lowStockCount ? lowStockCount.value() : 0));

    // Daily Revenue using QueryRunner
    auto query = Sale::query().where("date(created_at) = date('now')");
    auto revenueResult = QEloquent::QueryRunner::select("SUM(amount)", query);
    double totalRevenue = (revenueResult && revenueResult->next()) ? revenueResult->value(0).toDouble() : 0.0;
    m_dailyRevenueLabel->setText(QString::number(totalRevenue, 'f', 2) + " $");

    // Top 10 Products
    m_topProductsTable->setRowCount(0);
    QString topQuery = "SELECT p.name, SUM(si.quantity) as total_qty, c.name as cat_name, SUM(si.quantity * si.unit_price) as revenue "
                       "FROM SaleItems si "
                       "JOIN Products p ON si.product_id = p.id "
                       "JOIN Categories c ON p.category_id = c.id "
                       "GROUP BY si.product_id "
                       "ORDER BY total_qty DESC LIMIT 10";
    
    auto topResult = QEloquent::QueryRunner::exec(topQuery);
    if (topResult) {
        int row = 0;
        while (topResult->next()) {
            m_topProductsTable->insertRow(row);
            m_topProductsTable->setItem(row, 0, new QTableWidgetItem(topResult->value(0).toString()));
            m_topProductsTable->setItem(row, 1, new QTableWidgetItem(topResult->value(1).toString()));
            m_topProductsTable->setItem(row, 2, new QTableWidgetItem(topResult->value(2).toString()));
            m_topProductsTable->setItem(row, 3, new QTableWidgetItem(topResult->value(3).toString() + " $"));
            row++;
        }
    }
}

} // namespace Store
