#include "scom_tcp_server.h"
#include "scom_tcp_thread.h"
#include "SCOMProtocol.h"

scom_tcp_server::scom_tcp_server(QObject *parent) :
    QTcpServer(parent)
{
}

void scom_tcp_server::startServer()
{
    int port = 10001;

    if(!this->listen(QHostAddress::Any,port))
    {
        qDebug() << "Could not start server";
    }
    else
    {
        qDebug() << "Listening to port " << port << "...";
    }
}

void scom_tcp_server::incomingConnection(qintptr socketDescriptor)
{
    // We have a new connection
    qDebug() << socketDescriptor << " Connecting...";

    scom_tcp_thread *thread = new scom_tcp_thread(socketDescriptor, m_scom_protocol, this);

    // connect signal/slot
    // once a thread is not needed, it will be beleted later
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();
}
