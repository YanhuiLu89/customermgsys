#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QSqlTableModel>
#include <QSqlQuery>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    /****************数据库初始化**********************/
    m_databaseMg=new databasemg();
    m_cusmodel=new QSqlTableModel(this,m_databaseMg->database());
    m_cusmodel->setEditStrategy(QSqlTableModel::OnFieldChange);
    m_cusmodel->setTable("customer");
    m_cusmodel->select();
    int count=m_cusmodel->rowCount();
    count=count;

    /*********ui布局初始化*******************/
    ui->setupUi(this);
    //客户类型
    ui->comboBox_ctype->addItem(QString::fromLocal8Bit("企业"));
    ui->comboBox_ctype->addItem(QString::fromLocal8Bit("个人"));
    ui->tableView->setModel(m_cusmodel);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_cusmodel;
    delete m_databaseMg;
}


void MainWindow::on_groupBox_clicked()
{

}

void MainWindow::on_createcusBtn_clicked()
{
    QString name=ui->lineEdit_cname->text();
    QSqlQuery query(m_databaseMg->database());
    query.exec("select * from customer");
    int id=query.value(0).toInt();
    QString str=query.value(1).toString();
    if(name.isEmpty())
    {
        QMessageBox::warning(0,nullptr,QString::fromLocal8Bit("客户姓名不能为空！"));
        return;
    }
    if(m_databaseMg->addCustomer(name,ui->comboBox_ctype->currentText(),
                              ui->lineEdit_cpthone->text(),ui->textEdit_caddress->toPlainText()))
    {
        QMessageBox::information(0,nullptr,QString::fromLocal8Bit("新建客户成功"));
    }
    else
    {
        QMessageBox::warning(0,nullptr,QString::fromLocal8Bit("新建客户失败！"));
    }
    return;
}
