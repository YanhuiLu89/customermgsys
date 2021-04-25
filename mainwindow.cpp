#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    m_databaseMg=new databasemg();
    ui->setupUi(this);

    //客户类型
    ui->comboBox_ctype->addItem(QString::fromLocal8Bit("企业"));
    ui->comboBox_ctype->addItem(QString::fromLocal8Bit("个人"));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_databaseMg;
}

