#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "defines.h"

#include <QSslSocket>

#include <iostream>
#include <sstream>

#include "boost/iostreams/filtering_stream.hpp"
#include "boost/iostreams/filter/gzip.hpp"
#include "boost/iostreams/device/array.hpp"
#include "boost/iostreams/copy.hpp"


bool gzipDecompress(const QByteArray & in , QByteArray& out) {
    std::stringstream ss_decomp;
    boost::iostreams::filtering_istream stream;
    stream.push(boost::iostreams::gzip_decompressor());
    //in.push(ss_comp);     //从字符流中解压
    stream.push(boost::iostreams::array_source(in.constData(), in.count()));     //从文件中解压
    boost::iostreams::copy(stream, ss_decomp);
    out.append(ss_decomp.str().c_str() , (ss_decomp.str().size()));
    return true;
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qDebug() << "MainWindow  " << QSslSocket::supportsSsl();

    _wsBTC = new QWebSocket();
    connect(_wsBTC, SIGNAL(connected()), this, SLOT(onWsBtcConnected()));
    connect(_wsBTC,SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));
    connect(_wsBTC, SIGNAL(pong(quint64,QByteArray)), this, SLOT(onPong(quint64,QByteArray)));
    connect(_wsBTC, SIGNAL(sslErrors(QList<QSslError>)),this,SLOT(onSslErrors(QList<QSslError>)));
    connect(_wsBTC, SIGNAL(disconnected()), this, SLOT(onWsBtcDisconnected()));
    connect(_wsBTC,SIGNAL(binaryMessageReceived(QByteArray)), this, SLOT(onWsBtcBinaryMessageReceived(QByteArray)));
    connect(_wsBTC,SIGNAL(textMessageReceived(QString)),this, SLOT(onTextMessageReceived(QString)));
}

void MainWindow::onSslErrors(QList<QSslError> e)
{
    qDebug()  << "onSslErrors";
    for(auto each : e )
    {
        qDebug() << each.errorString() ;
    }
}

void MainWindow::onTextMessageReceived(QString message)
{
    qDebug() << "onTextMessageReceived" << message;
}

void  MainWindow::onError(QAbstractSocket::SocketError error)
{
    qDebug() << "onError" << error;
}

void  MainWindow::onPong(quint64 elapsedTime, const QByteArray &payload)
{
    qDebug() << "onPong" << elapsedTime;
}

MainWindow::~MainWindow()
{
    qDebug() << "~MainWindow";
    delete ui;
    delete _wsBTC;
}

void MainWindow::onWsBtcConnected()
{
    qDebug() << "onWsBtcConnected\n";

    QJsonObject json;
    json.insert("sub", "market.btcusdt.kline.1min");
    json.insert("id", "id1");

    QJsonDocument jsonDoc;
    jsonDoc.setObject(json);

    QByteArray ba = jsonDoc.toJson(QJsonDocument::Compact);
    QString jsonStr(ba);
    qDebug() << jsonStr;

    _wsBTC->sendTextMessage(jsonStr);
}

void MainWindow::onWsBtcDisconnected()
{
    qDebug() << "onWsBtcDisconnected\n";
}




void MainWindow::onWsBtcBinaryMessageReceived(const QByteArray &message)
{
    qDebug() << "onWsBtcBinaryMessageReceived\n";

    QByteArray baUnpack;
    //取回来的内容要解压
    bool bResult = gzipDecompress(message, baUnpack);
    if (bResult) {
        //解析Json
        qDebug() << baUnpack;
        QJsonParseError jsonError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(baUnpack, &jsonError);
        if (jsonError.error == QJsonParseError::NoError) {
            if (jsonDoc.isObject()) {
                QJsonObject jsonObj = jsonDoc.object();
                if (jsonObj.contains("ping")) {
                    //是心跳包
                    QJsonObject jsonPong;
                    jsonPong.insert("pong", jsonObj.value("ping"));
                    QJsonDocument docPong(jsonPong);
                    _wsBTC->sendTextMessage(QString(docPong.toJson(QJsonDocument::Compact)));
                }
                else if (jsonObj.contains("ch")) {
                    //订阅的数据包
                    QString symbol = jsonObj.value("ch").toString();
                    if (symbol == "market.btccny.kline.1min") {
                        //自己的处理代码
                    } else if (symbol == "market.ltccny.kline.1min"){
                        //自己的处理代码
                    }
                }
                else if (jsonObj.contains("rep")) {
                    //请求返回的数据包
                    QString sSymbol = jsonObj.value("rep").toString();
                    if (sSymbol == "market.btccny.kline.1min") {
                        //自己的处理代码
                    }
                    else if (sSymbol == "market.btccny.kline.5min") {
                        //自己的处理代码
                    }
                }
            }
        }
    } else {
        qDebug() << "Failure";
    }
}

void MainWindow::on_pushButton_clicked()
{
    qDebug() << "on_pushButton_clicked  _wsBTC connect";
    QUrl url("wss://api.huobi.pro/ws");
    _wsBTC->open(url);
}
