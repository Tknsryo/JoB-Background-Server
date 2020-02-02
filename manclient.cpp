#include <QThread>
#include "manclient.h"
#include "global.h"

manClient::manClient(QTcpServer *new_server,QObject *parent) : QObject(parent)
{
    server = new_server;
    socket = nullptr;
    connect(server,&QTcpServer::newConnection,this,&manClient::Connect);
}
void manClient::Connect()
{
    if((socket=server->nextPendingConnection())){
        connect(socket,&QTcpSocket::disconnected,this,&manClient::openServer);
        server->close();
        qDebug() << "connected to client";
        connect(socket,&QTcpSocket::readyRead,this,&manClient::mapper);

        //echo
        sendMessage("You have successfully connected to the server.");
    }
    else {
        qDebug()<<"failed to connecte to client";
    }
}

void manClient::openServer()
{
    socket->deleteLater();
    socket = nullptr;
    if(!server->isListening())
    {
        if(!server->listen(QHostAddress::Any,9999))
            server->listen();
        qDebug() << "listening on tcp://"<<
                    server->serverAddress().toString().remove(" ")
                 <<":"<<server->serverPort()<<",for the client";
    }
}

void manClient::mapper()
{
    char data;
    if(socket->read(&data,1)>0){
        qDebug()<<"recv from client: "<<data;
        if(data == '~'){
            //recv data
            QByteArray Header;
            QByteArray Data;
            quint8 batchs;
            qDebug() << "recving from client";
            Header = socket->readAll();
            batchs = static_cast<quint8>(Header.at(2));
            qDebug() << "batchs:"<<batchs;
            for(int i=0;i<batchs;i++){
                socket->waitForReadyRead();
                Data.append(socket->readAll());
            }
            if(Data.size() != static_cast<quint8>(Header.at(1))){
                qDebug("Fail to recv data from client");
                return;
            }
            qDebug("recv done!");
            //analyze data
            if(Header.at(0) == 'r'){ //redirect
                mutex2->lock();
                dataForGame->append(Data);
                mutex2->unlock();
                emit newDataForGame();
                qDebug("sending to game");
            }
            else if (Header.at(0) == 's') { //setting
                settings(Data.at(0),Data.mid(1));
            }
            else if (Header.at(0) == 'q') { //query
                if(Data.front() == 'g'){
                    if(gameOn){
                        sendMessage("Game is running.");
                    }
                    else {
                        sendMessage("Game is not running");
                    }
                }
            }
        }
        else {
            socket->readAll();
        }
    }
}

void manClient::onGameRecv()
{
    mutex1->lock();
    if(!dataList->isEmpty()){
        QByteArray data = dataList->takeFirst();
        if(socket!=nullptr){
            qDebug("sending to client");
            QByteArray header("~");
            header.append('d').append(static_cast<quint8>(data.size()));
            header.append(quint8(1));
            socket->write(header);
            socket->write(data);
            socket->flush();
        }
    }
    mutex1->unlock();
}

void manClient::settings(const char key, const QByteArray &value)
{
    if(key == 't' || key == 'o' || key == 's' || key == 'r' || key == 'd'){
        emit settingRecvd(key,value);
    }
}

void manClient::sendMessage(const QByteArray & message)
{
    QByteArray header("~");
    header.append('m').append(static_cast<quint8>(message.size())).append(quint8(1));
    socket->write(header);
    socket->write(message);
}
