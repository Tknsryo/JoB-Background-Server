#include <QCoreApplication>
#include <QTcpServer>
#include "communicator.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QTcpServer server(&a);
    server.setMaxPendingConnections(1);
    if(!server.listen(QHostAddress::Any,8888))
        server.listen();
    qDebug() << "listening on tcp://"<<
                server.serverAddress().toString().remove(" ")
             <<":"<<server.serverPort();
    new Communicator(&server,&a);


    return a.exec();
}
