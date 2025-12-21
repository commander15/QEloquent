#ifndef DASHBOARDVIEW_H
#define DASHBOARDVIEW_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>

namespace Store {

class DashboardView : public QWidget
{
    Q_OBJECT
public:
    explicit DashboardView(QWidget *parent = nullptr);
    void refresh();

private:
    QLabel *m_totalSalesLabel;
    QLabel *m_totalRevenueLabel;
    QLabel *m_lowStockLabel;

    QWidget* createStatCard(const QString &title, QLabel *&valueLabel, const QString &color);
};

} // namespace Store

#endif // DASHBOARDVIEW_H
