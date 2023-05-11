#include "rsamodule.h"
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <qDebug>
#include <string>
#include <cstring>

#define DEBUG_MODE
#define PRI_LEN 887
#define PUB_LEN 272

void rsaModule::generateKey()
{
    size_t pri_len = 0; // 私钥长度
    size_t pub_len = 0; // 公钥长度
    pub_key.clear();
    pri_key.clear();

    RSA *keypair = RSA_generate_key(KEY_LENGTH, RSA_3, NULL, NULL);

    BIO *pri = BIO_new(BIO_s_mem());
    BIO *pub = BIO_new(BIO_s_mem());

    PEM_write_bio_RSAPrivateKey(pri, keypair, NULL, NULL, 0, NULL, NULL);
    PEM_write_bio_RSA_PUBKEY(pub, keypair);

    pri_len = BIO_pending(pri);
    pub_len = BIO_pending(pub);

    char* tmp_pri_key = (char *)calloc(pri_len + 1, sizeof(char));
    char* tmp_pub_key = (char *)calloc(pub_len + 1, sizeof(char));

    BIO_read(pri, tmp_pri_key, pri_len);
    BIO_read(pub, tmp_pub_key, pub_len);

    pri_key = QString(tmp_pri_key);
    pub_key = QString(tmp_pub_key);

    qDebug()<<pri_key<<"\n";
    qDebug()<<pub_key<<"\n";

    RSA_free(keypair);
    BIO_free_all(pub);
    BIO_free_all(pri);
}

QByteArray rsaModule::rsaPubEncrypt(const QByteArray clear_text)
{
    QByteArray crypt_text;

    char* tmp = (char*)calloc(PUB_LEN + 1,sizeof(char));
    std::string str_tmp = pub_key.toStdString();
    strcpy(tmp,str_tmp.data());
    BIO* keybio = BIO_new_mem_buf(tmp, -1);
    RSA* rsa = RSA_new();

    rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa, NULL, NULL);

    int key_len = RSA_size(rsa);
    int block_len = key_len - 11;

    char* buffer = (char*) calloc(key_len + 1,sizeof(char));
    int cipher_length = RSA_public_encrypt(clear_text.length(),
                                           (unsigned char*)clear_text.toStdString().data(),
                                           (unsigned char*)buffer,
                                           rsa,
                                           RSA_PKCS1_PADDING);

    QByteArray cipher_text = QByteArray(buffer,key_len);

    BIO_free_all(keybio);
    RSA_free(rsa);
    free(buffer);
    free(tmp);

    return cipher_text;
}

QByteArray rsaModule::rsaPriDecrypt(const QByteArray cipher_text)
{
    char* tmp = (char*)calloc(PRI_LEN + 1,sizeof(char));
    std::string str_tmp = pri_key.toStdString();
    strcpy(tmp,str_tmp.data());
    qDebug()<<tmp;

    RSA *rsa = RSA_new();
    BIO *keybio;
    keybio = BIO_new_mem_buf(tmp, -1);

    rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);
    if(rsa == nullptr){
        return "";
    }

    int key_len = RSA_size(rsa);
    char *buffer = (char*)calloc(key_len + 1,sizeof(char));

    int clear_length = RSA_private_decrypt(cipher_text.length(),
                                           (unsigned char*)cipher_text.data(),
                                           (unsigned char*)buffer,
                                           rsa,
                                           RSA_PKCS1_PADDING);
    if (clear_length==-1) {
        unsigned long ulErr = ERR_get_error(); // 获取错误号

        char szErrMsg[1024] = {0};
        char *pTmp = NULL;
        pTmp = ERR_error_string(ulErr,szErrMsg);
        qDebug()<< szErrMsg << "\n";
    }
    QByteArray clear_text = QByteArray(buffer, clear_length);

    BIO_free_all(keybio);
    RSA_free(rsa);
    free(buffer);

    return clear_text;
}

QString rsaModule::getPubKey()
{
    return this->pub_key;
}

void rsaModule::setPubKey(QString key)
{
    this->pub_key = key;
}

rsaModule::rsaModule()
{

}

rsaModule::~rsaModule()
{

}
