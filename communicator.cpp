#include "communicator.h"
#include "global.h"

Communicator::Communicator(QTcpServer *new_server,QObject *parent) :
    QObject(parent)
{
    server = new_server;
    socket = nullptr;
    toClient = false;
    autoRestart = false;
    dispData = false;
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
        connect(socket,&QTcpSocket::readyRead,this,&Communicator::mapper);

        gameOn = true;
    }
    else {
        qDebug()<<"failed to connecte to FG";
    }
}

void Communicator::openServer()
{
    save();
    timer->stop();
    timer->deleteLater();
    socket->deleteLater();
    socket = nullptr;
    gameOn = false;
    if(!server->isListening())
    {
        if(!server->listen(QHostAddress::Any,8888))
            server->listen();
        qDebug() << "listening on tcp://"<<server->serverAddress().toString()
                 <<":"<<server->serverPort()<<",for the game.";
    }
}

void Communicator::mapper()
/**
 * 映射接受到的指令
 */
{
    char data;
    if(socket->read(&data,1)>0){
        if(autoRestart && data =='r')
        {
            socket->write(QByteArray("~").append('k').append(1).append(1).append('r'));
            socket->flush();
            qDebug()<<"restart";
        }
        else if(data == '~') {
            qDebug() << "recving from game";
            QByteArray Header;
            QByteArray Data;
            Header = socket->readAll();
            if(Header.at(0) == 'd')
            {
                readStatus(Data,static_cast<quint8>(Header.at(1)));
            }
        }
        else {
            socket->readAll();
        }
    }
}

void Communicator::readStatus(QByteArray &statusData,quint8 len)
{
    do{
        socket->waitForReadyRead();
        statusData.append(socket->readAll());
    }while(statusData.isEmpty() || statusData.back() != '!');
    status.time = (double*)(statusData.data());
    status.stat = (quint8*)(statusData.data()+8);
    status.jump = (quint8*)(statusData.data()+8+1);
    status.squat = (quint8*)(statusData.data()+8+2);
    status.barDis = (double*)(statusData.data()+8+3);
    status.barLen = (double*)(statusData.data()+8+3+3*8);
    //接收数据写入全局列表
    if(statusData.size() != len){
        qDebug()<<"Fail to recv status data";
        return;
    }
    if(toClient){
        mutex1->lock();
        dataList->append(statusData);
        mutex1->unlock();
        emit receDone(); //完成接收
    }
    if(recvData.isOpen()){ //将接受到的数据写入文件
        qDebug("writing data to disk");
        int i;
        recvData.write(QString::number(*status.time).toUtf8()+',');
        recvData.write(QString::number(*status.stat).toUtf8()+',');
        recvData.write(QString::number(*status.jump).toUtf8()+',');
        recvData.write(QString::number(*status.squat).toUtf8()+',');
        for(i=0;i<3;i++){
            recvData.write(QString::number(status.barDis[i]).toUtf8()+',');
        }
        for(i=0;i<2;i++){
            recvData.write(QString::number(status.barLen[i]).toUtf8()+',');
        }
        recvData.write(QString::number(status.barLen[2]).toUtf8()+'\n');
    }
    if(dispData){
        qDebug() << "recv status: ";
        qDebug() << "time" << *status.time;
        qDebug() << "stat" << *status.stat;
        qDebug() << "jump" << *status.jump;
        qDebug() << "squat" << *status.squat;
        qDebug() << "barDis" << status.barDis[0]<<' '<<status.barDis[1]<<' '<<status.barDis[2];
        qDebug() << "barLen" << status.barLen[0]<<' '<<status.barLen[1]<<' '<<status.barLen[2];
    }
}

void Communicator::toDisk(const QString & path, QIODevice::OpenMode flag)
{
    if(recvData.isOpen()){
        qDebug("toDisk: file already opened.");
        return;
    }
    recvData.setFileName(path);
    if(!recvData.open(flag)){
        qDebug() << "file doesn't exist";
    }
    if(recvData.size()==0){
        recvData.write("time,gameover,jump,squat,");
        recvData.write("dis1,dis2,dis3,");
        recvData.write("len1,len2,len3\n");
    }
}
void Communicator::save()
{
    if(recvData.isOpen()){
        recvData.close();
        qDebug() << "file saved.";
    }
    else {
        qDebug() << "file not opened!";
    }
}

void Communicator::fromClient()
{
    mutex2->lock();
    if(!dataForGame->isEmpty()){
        QByteArray data = dataForGame->takeFirst();
        if(socket!=nullptr){
            socket->write(data);
            socket->flush();
            qDebug("sent to game");
        }
    }
    mutex2->unlock();
}

void Communicator::settingFromClient(const char key, const QByteArray &value)
{
    if(key == 't'){ //是否将game的数据传给client
        quint8 val = static_cast<quint8>(value.at(0));
        toClient = val > 0;
    }
    else if (key == 'o') { //game的数据写入文件
        quint8 type = static_cast<quint8>(value.at(0));
        QIODevice::OpenMode flag = type>0?QIODevice::Append:QIODevice::WriteOnly;
        toDisk(value.mid(1),flag);
    }
    else if (key == 's') { //保存
        quint8 val = static_cast<quint8>(value.at(0));
        if(val > 0)
            save();
    }
    else if (key == 'r') {
        quint8 val = static_cast<quint8>(value.at(0));
        autoRestart = val > 0;
    }
    else if (key == 'd') {
        quint8 val = static_cast<quint8>(value.at(0));
        dispData = val > 0;
    }
    else {
        qDebug("fail to configure settings.");
        emit messageForClient("fail to configure settings.");
        return;
    }
    emit messageForClient("setting applied.");
}
