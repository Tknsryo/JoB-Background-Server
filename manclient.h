#ifndef MANCLIENT_H
#define MANCLIENT_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class manClient : public QObject
{
    Q_OBJECT
public:
    explicit manClient(QTcpServer *new_server,QObject *parent = nullptr);
    void onGameRecv(); //处理从game接收到的数据
    void sendMessage(const QByteArray&);
private:
    void Connect();
    void mapper();
    void openServer();
    void settings(const char key, const QByteArray& value);
    QTcpServer *server;
    QTcpSocket *socket;
signals:
    void newDataForGame(); //从client接收到传给game的数据
    void settingRecvd(const char key, const QByteArray& value); //从client接收到设置主线程中属性的命令
};

#endif // MANCLIENT_H
