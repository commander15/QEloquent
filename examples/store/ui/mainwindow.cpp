#include "mainwindow.h"
#include "dashboardview.h"
#include "productcatalogview.h"
#include "saleshistoryview.h"
#include <QHBoxLayout>
#include <QFrame>

namespace Store {

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setupUi();
    setWindowTitle("QEloquent Store Manager");
    resize(1200, 800);
}

void MainWindow::setupUi()
{
    auto centralWidget = new QWidget(this);
    auto mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Sidebar
    m_sidebar = new QListWidget(this);
    m_sidebar->setFixedWidth(240);
    m_sidebar->setStyleSheet(
        "QListWidget { "
        "  background-color: #2c3e50; "
        "  border: none; "
        "  color: #ecf0f1; "
        "  font-size: 16px; "
        "  outline: none; "
        "  padding-top: 20px; "
        "}"
        "QListWidget::item { "
        "  padding: 15px 25px; "
        "}"
        "QListWidget::item:selected { "
        "  background-color: #34495e; "
        "  border-left: 5px solid #3498db; "
        "}"
    );

    m_sidebar->addItem("Dashboard");
    m_sidebar->addItem("Inventory");
    m_sidebar->addItem("Sales History");
    m_sidebar->setCurrentRow(0);

    mainLayout->addWidget(m_sidebar);

    // Content Area
    m_contentArea = new QStackedWidget(this);
    m_contentArea->setStyleSheet("background-color: #f5f6fa;");

    m_dashboard = new DashboardView(this);
    m_catalog = new ProductCatalogView(this);
    m_history = new SalesHistoryView(this);

    m_contentArea->addWidget(m_dashboard);
    m_contentArea->addWidget(m_catalog);
    m_contentArea->addWidget(m_history);

    mainLayout->addWidget(m_contentArea);

    setCentralWidget(centralWidget);

    connect(m_sidebar, &QListWidget::currentRowChanged, this, &MainWindow::onNavigationChanged);
}

void MainWindow::onNavigationChanged(int index)
{
    m_contentArea->setCurrentIndex(qMin(index, m_contentArea->count() - 1));
    
    // Refresh the view being shown
    if (index == 0) m_dashboard->refresh();
    else if (index == 1) m_catalog->refresh();
    else if (index == 2) m_history->refresh();
}

} // namespace Store
