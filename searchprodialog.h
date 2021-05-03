#ifndef SEARCHPRODIALOG_H
#define SEARCHPRODIALOG_H

#include <QDialog>
#include "databasemg.h"
namespace Ui {
class SearchProDialog;
}

class QSqlTableModel;
class SearchProDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SearchProDialog(QWidget *parent = nullptr);
    ~SearchProDialog();

private slots:
    void on_searchProBtn_2_clicked();
    virtual void accept();
private:
    void setProHeaders();
private:
    databasemg* m_databaseMg;
    QSqlTableModel* m_promodel;
    Ui::SearchProDialog *ui;
};

#endif // SEARCHPRODIALOG_H
