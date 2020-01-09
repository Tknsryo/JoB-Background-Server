#include "communicator.h"

Communicator::Communicator(QTcpServer *new_server,QObject *parent) :
    QObject(parent)
{
    server = new_server;
    socket = nullptr;
    connect(server,&QTcpServer::newConnection,this,&Communicator::Connect);
}

void Communicator::Connect()
{
    if((socket=server->nextPendingConnection())){
        connect(socket,&QTcpSocket::disconnected,this,&Communicator::openServer);
        server->close();
        qDebug()<<"connected to FG";
        timer = new QTimer(this);
        timer->setInterval(2000);
        connect(timer,&QTimer::timeout,[=](){
            static int i=1;
            i==1?socket->write("j"):socket->write("n");
            socket->flush();
            qDebug()<<"send";
            i *= -1;});
        timer->start();
        connect(socket,&QTcpSocket::readyRead,this,&Communicator::mapper);
    }
    else {
        qDebug()<<"failed to connecte to FG";
    }
}

void Communicator::openServer()
{
    timer->stop();
    timer->deleteLater();
    socket->deleteLater();
    if(!server->isListening())
    {
        if(!server->listen(QHostAddress::Any,8888))
            server->listen();
        qDebug() << "listening on tcp://"<<server->serverAddress().toString()<<":"<<server->serverPort();
    }
}

void Communicator::mapper()
{
    char data;
    if(socket->read(&data,1)>0){
        qDebug()<<"recv: "<<data;
        if(data =='r')
        {
            socket->write("r");
            socket->flush();
            qDebug()<<"restart";
        }
    }
}
