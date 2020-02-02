#ifndef THREADCLIENT_H
#define THREADCLIENT_H

#include <QThread>
#include <QTcpServer>
#include "manclient.h"
class ThreadClient : public QThread
{
    Q_OBJECT
public:
    explicit ThreadClient(QObject *parent=nullptr);
    virtual ~ThreadClient() override;
    manClient *client;
private:
    QTcpServer *server;
    void run() override;
signals:
    void buildDone();
};

#endif // THREADCLIENT_H
