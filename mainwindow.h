#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "databasemg.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QSqlTableModel;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_groupBox_clicked();

    void on_createcusBtn_clicked();

    void on_searchCusBtn_clicked();

    void on_showCusBtn_clicked();

    void on_inportCusBtn_clicked();

    void on_createsupBtn_clicked();

    void on_searchSupBtn_clicked();

    void on_showSupBtn_clicked();

    void on_inportSupBtn_clicked();

    void on_delCusBtn_clicked();

    void on_delSupBtn_clicked();

private:
    void setCusHeaders();
    void setSupHeaders();
private:
    Ui::MainWindow *ui;
    databasemg* m_databaseMg;
    QSqlTableModel* m_cusmodel;
    QSqlTableModel* m_supmodel;
};
#endif // MAINWINDOW_H
