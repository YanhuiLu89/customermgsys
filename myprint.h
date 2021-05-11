#ifndef MYPRINT_H
#define MYPRINT_H

#include <QObject>
#include <QAbstractItemModel>
#include <QTextStream>
#include <QPrinter>

class MyPrint : public QObject
{
    Q_OBJECT
public:
    explicit MyPrint(QObject *parent = nullptr);

    ~MyPrint(){}
   void setCharSet(const QString &set = QString("utf-8")){ m_charSet = set; }
   void dataBegin();
   void dataEnd(){ out << QString("</html>"); }
   // 添加主标题（两行：第一行居中显示文字，第二行居右显示时间）
   void insertTitle(const QString &title);
   // 添加次级标题
   void insertTitle2(const QString &title2);
   // 空出一行
   void insertSeperator(){ out << QString("<br>\n"); }
   // 不支持嵌套TABLE
   void tableBegin(const QStringList &head);
   void tableEnd(){ out << QString("</table></div>\n"); }
   void insert2TableRow(const QStringList &rowData);
   void printWithPreview();// 带预览
   void printDirect();     // 直接打印，不带预览

private:
   void resetState();

private slots:
   void getPreviewData(QPrinter *printer);

private:
   QString m_charSet;
   QString m_data;
   QTextStream out;

};

#endif // MYPRINT_H
