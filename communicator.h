#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#include <QObject>
#include <QTimer>
#include <QTcpSocket>
#include <QTcpServer>

class Communicator : public QObject
{
    Q_OBJECT
public:
    explicit Communicator(QTcpServer *new_server,QObject *parent = nullptr);
    void Connect();
private:
    QTcpServer *server;
    QTcpSocket *socket;
    QTimer *timer;
    void openServer();
    void mapper();
signals:

};

#endif // COMMUNICATOR_H
