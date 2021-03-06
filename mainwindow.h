#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>
#include "databasemg.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QSqlTableModel;
class MySqlRelationTableModel;
class QTimer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
   void setSrchProSelectedNum(QString num){m_srchProSelectedNum=num;}
   QString srchProSelectedNum(){return m_srchProSelectedNum;}

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

    void on_sell_addBtn_clicked();

    void on_sell_importBtn_clicked();

    void on_sell_searchBtn_clicked();

    void on_sell_showallBtn_clicked();

    void on_sell_exportBtn_clicked();

    void on_sell_printBtn_clicked();

    void on_sell_delBtn_clicked();

    void on_tableViewSell_clicked(const QModelIndex &index);

    void on_stock_prono_lineEdit_textChanged(const QString &arg1);

    void on_stock_pushButton_clicked();

    void on_stock_cnt_spinBox_valueChanged(int arg1);

    void on_stock_addBtn_clicked();

    void on_stock_searchBtn_clicked();

    void on_sell_price_spinBox_valueChanged(int arg1);

    void on_sell_totalprice_spinBox_valueChanged(int arg1);

    void on_sell_payed_spinBox_valueChanged(int arg1);

    void on_stock_price_spinBox_valueChanged(int arg1);

    void on_stock_totalprice_spinBox_valueChanged(int arg1);

    void on_stock_payed_spinBox_valueChanged(int arg1);

    void on_stock_showallBtn_clicked();

    void on_stock_importBtn_clicked();

    void on_stock_delBtn_clicked();

    void on_stock_exportBtn_clicked();

    void on_stock_printBtn_clicked();

    void on_tableViewStock_clicked(const QModelIndex &index);

    void sellTimerFunc();
    void stockTimerFunc();
    void on_tableViewSell_doubleClicked(const QModelIndex &index);

    void on_tableViewStock_doubleClicked(const QModelIndex &index);

private:
    void setCusHeaders();
    void setSupHeaders();
    void setProHeaders();
    void setSellHeaders();
    void setStockHeaders();
    void updateSellCusComBox();
    void updateStockSupComBox();
    void exportTable(const QSqlTableModel* sqlMode,const QString &path,const QStringList heads);
    bool selectProduct(QString pronum);
    bool selectCustomer(QString name);
    bool selectSupplier(QString name);
    //print相关

private:
    Ui::MainWindow *ui;
    databasemg* m_databaseMg;
    QSqlTableModel* m_cusmodel;
    QSqlTableModel* m_supmodel;
    QSqlTableModel* m_promodel;
    QString m_srchProSelectedNum;
    MySqlRelationTableModel* m_sellmodel;
    MySqlRelationTableModel* m_stockmodel;
    QTimer* m_sellClickTimer;
    QTimer* m_stockClickTimer;
    QModelIndex m_sellModelIndex;
    QModelIndex m_stockModelIndex;
};
#endif // MAINWINDOW_H
