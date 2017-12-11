#ifndef SCOM_TCP_THREAD_H
#define SCOM_TCP_THREAD_H

#include <QThread>
#include <QTcpSocket>
#include <QDebug>
#include "CircBuff.h"
#include "SCOMProtocol.h"

class scom_tcp_thread : public QThread
{
    Q_OBJECT

	SCOMProtocol *scom_protocol;
	
public:
    explicit scom_tcp_thread(qintptr ID, SCOMProtocol *scom_protocol, QObject *parent = 0);
    
    void run();

signals:
    void error(QTcpSocket::SocketError socketerror);
    
public slots:
    void readyRead();
    void disconnected();

private:
    QTcpSocket *socket;
    qintptr socketDescriptor;
	CircBuff *scom_buff;
	QByteArray answer;
};

#endif // SCOM_TCP_THREAD_H
