#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QToolBar>
#include <QAction>

namespace Store {

class DashboardView;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void switchView();

private:
    QStackedWidget *m_contentArea;
    DashboardView *m_dashboard;
    
    void setupUi();
    void addToolbarAction(QToolBar *toolbar, const QString &text, const QString &icon, int index);
};

} // namespace Store

#endif // MAINWINDOW_H
