#include "mainwindow.h"
#include <QApplication>

#include "openssl/sha.h"

#include <string>


std::string sha256(const std::string str)
{
    char buf[2];
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);
    std::string newString = "";
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        sprintf(buf,"%02x",hash[i]);
        newString = newString + buf;
    }
    return newString;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    char out[1024];
     std::string str = "Hello World";
    //qDebug() << QString::fromStdString(sha256(str));

    return a.exec();
}
