#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>
#include <QString>
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
    void onConnction();
    void sendFile();
    void Send();
    void on_btnChoose_clicked();

    void on_btnSend_clicked();

private:
    Ui::Widget *ui;
    QTcpServer * m_tcpServer;
    QTcpSocket * m_tcpSocket;
    QFile m_file;
    QString m_fileName;
    qint64 m_fileSize;
    qint64 m_sendSize;

    rsaModule rm;
    bool isHead = true;
};
#endif // WIDGET_H
