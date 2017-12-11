#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


#include <QtWebSockets/QtWebSockets>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QWebSocket * _wsBTC;

private:
    Ui::MainWindow *ui;

private slots:
    void onWsBtcConnected();
    void onWsBtcDisconnected();
    void onWsBtcBinaryMessageReceived(const QByteArray &message);
    void onTextMessageReceived( QString message);

    void  onSslErrors( QList<QSslError> errors);


    void on_pushButton_clicked();

    void  onError(QAbstractSocket::SocketError error);
    void  onPong(quint64 elapsedTime, const QByteArray &payload);
};

#endif // MAINWINDOW_H
