#ifndef RSAMODULE_H
#define RSAMODULE_H

#include <QString>

#define KEY_LENGTH 1024

//只支持1024位
class rsaModule
{
public:
    rsaModule();
    ~rsaModule();
    void generateKey();
    QByteArray rsaPubEncrypt(const QByteArray clear_text);
    QByteArray rsaPriDecrypt(const QByteArray cipher_text);

    QString getPubKey();
    void setPubKey(QString key);

private:
    QString pub_key;
    QString pri_key;
};

#endif // RSAMODULE_H
