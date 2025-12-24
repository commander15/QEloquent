#ifndef DASHBOARDVIEW_H
#define DASHBOARDVIEW_H

#include <QWidget>
#include <QLabel>
#include <QTableWidget>
#include <QVBoxLayout>

namespace Store {

class DashboardView : public QWidget
{
    Q_OBJECT
public:
    explicit DashboardView(QWidget *parent = nullptr);
    void refresh();

private:
    QLabel *m_dateLabel;
    QLabel *m_totalProductsLabel;
    QLabel *m_lowStockLabel;
    QLabel *m_dailyRevenueLabel;
    QLabel *m_userNameLabel;
    QTableWidget *m_topProductsTable;

    void setupUi();
    QWidget* createStatCard(const QString &title, QLabel *&valueLabel, const QString &icon, const QString &color);
};

} // namespace Store

#endif // DASHBOARDVIEW_H
