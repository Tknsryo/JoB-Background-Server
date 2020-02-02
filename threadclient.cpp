#include "threadclient.h"

ThreadClient::ThreadClient(QObject *parent):QThread(parent)
{
    server = nullptr;
    client = nullptr;
}
ThreadClient::~ThreadClient()
{
    if(server!=nullptr)
        delete server;
    if(client!=nullptr)
        delete client;
}
void ThreadClient::run()
{
    server = new QTcpServer;
    server->setMaxPendingConnections(1);
    if(!server->listen(QHostAddress::Any,9999))
        server->listen();
    qDebug() << "listening on tcp://"<<
                server->serverAddress().toString().remove(" ")
             <<":"<<server->serverPort()<<",for the client";
    qDebug() << server->thread()->currentThreadId();

    client = new manClient(server);
    emit buildDone();
    exec();
}
