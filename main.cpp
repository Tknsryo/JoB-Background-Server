#include <QCoreApplication>
#include <QTcpServer>
#include "communicator.h"
#include "threadclient.h"
#include "global.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    dataList = new QByteArrayList;
    dataForGame = new QByteArrayList;
    mutex1 = new QMutex;
    mutex2 = new QMutex;

    QTcpServer server(&a);
    server.setMaxPendingConnections(1);
    if(!server.listen(QHostAddress::Any,8888))
        server.listen();
    qDebug() << "listening on tcp://"<<
                server.serverAddress().toString().remove(" ")
             <<":"<<server.serverPort()<<",for the game.";
    qDebug() << server.thread()->currentThreadId();

    Communicator *c =  new Communicator(&server,&a);

    ThreadClient client(&a);

    QObject::connect(&client,&ThreadClient::buildDone,[&](){
        qDebug()<<"build done";
        QObject::connect(c,&Communicator::receDone,client.client,&manClient::onGameRecv,Qt::QueuedConnection);
        QObject::connect(client.client,&manClient::newDataForGame,c,&Communicator::fromClient,Qt::QueuedConnection);
        QObject::connect(client.client,&manClient::settingRecvd,c,&Communicator::settingFromClient,Qt::QueuedConnection);
        QObject::connect(c,&Communicator::messageForClient,client.client,&manClient::sendMessage,Qt::QueuedConnection);
    });

    client.start();
    return a.exec();
}
