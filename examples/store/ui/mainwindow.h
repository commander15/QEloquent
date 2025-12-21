#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QListWidget>

namespace Store {

class DashboardView;
class ProductCatalogView;
class SalesHistoryView;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onNavigationChanged(int index);

private:
    QListWidget *m_sidebar;
    QStackedWidget *m_contentArea;
    
    DashboardView *m_dashboard;
    ProductCatalogView *m_catalog;
    SalesHistoryView *m_history;

    void setupUi();
};

} // namespace Store

#endif // MAINWINDOW_H
