#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "databasemg.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QSqlTableModel;
class MySqlRelationTableModel;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
   void setSellSelectedNum(QString num){m_sellSelectedNum=num;}
   QString sellSelectedRow(){return m_sellSelectedNum;}

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

    void on_searchProBtn_clicked();

    void on_showProBtn_clicked();

    void on_sell_prono_lineEdit_textChanged(const QString &arg1);

    void on_pushButton_clicked();

    void on_sell_cnt_spinBox_valueChanged(int arg1);

    void on_sell_price_doubleSpinBox_valueChanged(double arg1);

    void on_sell_totalprice_doubleSpinBox_valueChanged(double arg1);

    void on_sell_payed_doubleSpinBox_valueChanged(double arg1);

    void on_sell_addBtn_clicked();

    void on_sell_importBtn_clicked();

    void on_sell_searchBtn_clicked();

    void on_sell_showallBtn_clicked();

    void on_sell_exportBtn_clicked();

    void on_sell_printBtn_clicked();

    void on_sell_delBtn_clicked();

    void on_tableViewSell_clicked(const QModelIndex &index);

private:
    void setCusHeaders();
    void setSupHeaders();
    void setProHeaders();
    void setSellHeaders();
    void updateSellCusComBox();
    void exportSellTable(const QString &path);
    void doPrint(QString path);
    bool selectProduct(QString pronum);
    bool selectCustomer(QString name);
private:
    Ui::MainWindow *ui;
    databasemg* m_databaseMg;
    QSqlTableModel* m_cusmodel;
    QSqlTableModel* m_supmodel;
    QSqlTableModel* m_promodel;
    QString m_sellSelectedNum;
    MySqlRelationTableModel* m_sellmodel;
};
#endif // MAINWINDOW_H
