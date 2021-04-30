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

    void on_createpro_Btn_clicked();

    void on_procount_spinBox_valueChanged(int arg1);

    void on_proprice_spinBox_valueChanged(int arg1);

    void on_inportProBtn_clicked();

    void on_delProBtn_clicked();

private:
    void setCusHeaders();
    void setSupHeaders();
    void setProHeaders();
private:
    Ui::MainWindow *ui;
    databasemg* m_databaseMg;
    QSqlTableModel* m_cusmodel;
    QSqlTableModel* m_supmodel;
    QSqlTableModel* m_promodel;
};
#endif // MAINWINDOW_H
