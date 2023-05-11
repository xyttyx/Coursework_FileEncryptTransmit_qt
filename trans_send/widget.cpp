#include "widget.h"
#include "ui_widget.h"
#include <QFileDialog>
#include <QMessageBox>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    m_tcpServer = new QTcpServer(this);
    m_tcpSocket = nullptr;

    m_tcpServer->listen(QHostAddress::Any,6666);
    connect(m_tcpServer,&QTcpServer::newConnection,this,&::Widget::onConnction);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::onConnction(){
    m_tcpSocket = m_tcpServer->nextPendingConnection();
    QMessageBox::information(this,"成功","成功連接");
    connect(m_tcpSocket,&QTcpSocket::readyRead,this,&Widget::sendFile);
}

void Widget::sendFile(){
    QByteArray buf = m_tcpSocket->readAll();
    QString reply_text = QString::fromLatin1(buf);
    QString reply_filename;
    if(this->isHead){
        reply_filename = reply_text.section("##",0,0);
        this->rm.setPubKey(reply_text.section("##",1,1));
        isHead = false;
    }
    if(reply_text == "##$$%%" || reply_filename == m_fileName){
        Send();
    }
}

void Widget::Send()
{
    qint64 cnt = 0;
    qint64 len = 0;
    qint64 send_len = 0;
    QByteArray send_file;

    send_file.clear();
    char buffer[KEY_LENGTH / 8 - 11] = {0};
    len = m_file.read(buffer,sizeof(buffer));
    QByteArray* clear_file = new QByteArray(buffer,len);
    send_file = rm.rsaPubEncrypt(*clear_file);
    send_len = m_tcpSocket->write(send_file,send_file.length());
    m_sendSize += len;
    delete clear_file;
    qDebug()<<cnt++;

    if(m_sendSize == m_fileSize){
        m_file.close();
    }
}

void Widget::on_btnChoose_clicked()
{
#ifdef DEBUG_MODE
    QString filePath = "C:/Users/xyt/Desktop/test.txt";
#else
    QString filePath = QFileDialog::getOpenFileName(this,"open","../");
#endif
    if(!filePath.isEmpty()){
        m_fileName.clear();
        m_fileSize = 0;
        m_sendSize = 0;
        QFileInfo info(filePath);
        m_fileName = info.fileName();
        m_fileSize = info.size();
        m_file.setFileName(filePath);
        m_file.open(QIODevice::ReadOnly);
    }
}


void Widget::on_btnSend_clicked()
{
    QString head = tr("%1##%2").arg(m_fileName).arg(m_fileSize);
    m_tcpSocket->write(head.toUtf8().data());
}
