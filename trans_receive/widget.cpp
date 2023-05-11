#include "widget.h"
#include "ui_widget.h"
#include <QFileDialog>
#include <QMessageBox>

#define DEBUG_MODE


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    m_tcpSocket = new QTcpSocket(this);
    connect(m_tcpSocket,&QTcpSocket::readyRead,this,&Widget::RcvFile);
}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_btnConnect_clicked()
{
#ifdef DEBUG_MODE
    QString ip = "192.168.31.179";
#else
    QString ip = ui->lineEdit->text();
#endif
    quint16 port = 6666;
    m_tcpSocket->connectToHost(ip,port);
}

void Widget::RcvFile()
{
    QByteArray cipher_text;
    QByteArray clear_text;
    int clear_len = 0;
    if(isHead){
        isHead = false;
        QByteArray buf = m_tcpSocket->readAll();
#ifdef DEBUG_MODE
        QString dir = "C:/Users/xyt/Desktop/receive folder";
#else
        QString dir = QFileDialog::getExistingDirectory(this,"选择文件夹","../");
#endif
        m_fileName = dir.append("/").append(QString(buf).section("##",0,0));
        m_fileSize = QString(buf).section("##",1,1).toInt();
        m_rcvSize = 0;

        rm.generateKey();

        QString reply_text = QString(buf).section("##",0,0) + "##" + rm.getPubKey();
        QByteArray reply = reply_text.toLatin1().data();
        m_tcpSocket->write(reply);
        m_file.setFileName(m_fileName);
        m_file.open(QIODevice::WriteOnly);
    }
    else{
        cipher_text.clear();
        clear_text.clear();
        cipher_text.resize(KEY_LENGTH / 8);
        cipher_text = m_tcpSocket->read(KEY_LENGTH / 8);
        clear_text = rm.rsaPriDecrypt(cipher_text);

        m_tcpSocket->write("##$$%%");

        clear_len = m_file.write(clear_text);
        m_rcvSize += clear_len;
        if(m_rcvSize >= m_fileSize){
            m_file.close();
            QMessageBox::information(this,"完成","文件接收完成");
            isHead = true;
        }
    }
}

