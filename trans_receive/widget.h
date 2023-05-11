#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <QFile>
#include "rsamodule.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_btnConnect_clicked();
    void RcvFile();

private:
    Ui::Widget *ui;
    QTcpSocket * m_tcpSocket;
    QFile m_file;
    QString m_fileName;
    qint64 m_fileSize;
    qint64 m_rcvSize;
    bool isHead = true;

    rsaModule rm;
};
#endif // WIDGET_H
