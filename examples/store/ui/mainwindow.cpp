#include "mainwindow.h"
#include "dashboardview.h"
#include <QToolBar>
#include <QToolButton>
#include <QIcon>
#include <QStatusBar>
#include <QLabel>
#include "session.h"

namespace Store {

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setupUi();
    setWindowTitle("QEloquent Professional Store Manager");
    resize(1280, 850);
}

void MainWindow::setupUi()
{
    // Toolbar
    auto toolbar = addToolBar("Main Toolbar");
    toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolbar->setIconSize(QSize(32, 32));
    toolbar->setMovable(false);
    toolbar->setStyleSheet(
        "QToolBar { spacing: 20px; padding: 10px; background-color: white; border-bottom: 1px solid #dfe6e9; }"
        "QToolButton { padding: 8px 15px; border-radius: 5px; color: #2d3436; font-weight: bold; min-width: 80px; }"
        "QToolButton:hover { background-color: #f1f2f6; }"
        "QToolButton:checked { background-color: #3498db; color: white; }"
    );

    addToolbarAction(toolbar, "Dashboard", "🏠", 0);
    addToolbarAction(toolbar, "Inventory", "📦", 1);
    addToolbarAction(toolbar, "Sales", "🛒", 2);
    addToolbarAction(toolbar, "Stats", "📊", 3);
    addToolbarAction(toolbar, "Settings", "⚙️", 4);

    // Content Area
    m_contentArea = new QStackedWidget(this);
    m_contentArea->setStyleSheet("background-color: #f5f6fa;");

    m_dashboard = new DashboardView(this);
    m_contentArea->addWidget(m_dashboard);
    
    // Fillers for other views
    for (int i=0; i<4; ++i) {
        auto label = new QLabel("View " + QString::number(i+1) + " Under Construction", this);
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("font-size: 24px; color: #bdc3c7;");
        m_contentArea->addWidget(label);
    }

    setCentralWidget(m_contentArea);

    // Status Bar
    auto status = statusBar();
    status->setStyleSheet("background-color: white; color: #636e72; padding: 5px; border-top: 1px solid #dfe6e9;");
    status->showMessage("Logged in as: " + Session::instance().user().name + " (" + Session::instance().user().email + ")");
}

void MainWindow::addToolbarAction(QToolBar *toolbar, const QString &text, const QString &iconStr, int index)
{
    auto action = new QAction(text, this);
    action->setData(index);
    action->setCheckable(true);
    
    if (index == 0) action->setChecked(true);
    
    connect(action, &QAction::triggered, this, &MainWindow::switchView);
    
    auto btn = new QToolButton();
    btn->setDefaultAction(action);
    btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    btn->setText(iconStr + ' ' + text);
    // Use the icon string as a placeholder if no real icon
    btn->setIcon(QIcon()); 
    
    toolbar->addWidget(btn);
}

void MainWindow::switchView()
{
    auto action = qobject_cast<QAction*>(sender());
    if (!action) return;

    // Uncheck others
    for (auto a : findChildren<QAction*>()) {
        if (a != action) a->setChecked(false);
    }
    action->setChecked(true);

    int index = action->data().toInt();
    m_contentArea->setCurrentIndex(index);
    
    if (index == 0) m_dashboard->refresh();
}

} // namespace Store
