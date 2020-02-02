#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#include <QObject>
#include <QTimer>
#include <QTcpSocket>
#include <QTcpServer>
#include <QFile>

class Communicator : public QObject
{
    Q_OBJECT
public:
    explicit Communicator(QTcpServer *new_server,QObject *parent = nullptr);
    void Connect();
    void fromClient(); //处理从client接收到传给game的数据
    void settingFromClient(const char key, const QByteArray &value); //处理从client接收到的设置
private:
    QTcpServer *server;
    QTcpSocket *socket;
    QTimer *timer;
    QFile recvData;
    bool toClient;
    bool autoRestart;
    bool dispData;
    void openServer();
    void mapper();
    struct {
        double *time;
        quint8 *stat;
        quint8 *jump;
        quint8 *squat;
        double *barDis;
        double *barLen;
    }status;
    void readStatus(QByteArray &data,quint8 len);
    void toDisk(const QString&,QIODevice::OpenMode = QIODevice::WriteOnly);
    void save();
signals:
    void receDone(); //数据已写入全局数组dataList
    void messageForClient(const QByteArray&); //有信息要发给client
};

#endif // COMMUNICATOR_H
