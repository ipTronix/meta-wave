#ifndef SCOM_TCP_SERVER_H
#define SCOM_TCP_SERVER_H

#include <QTcpServer>
#include "scom_tcp_thread.h"
#include "SCOMProtocol.h"

class scom_tcp_server : public QTcpServer
{
    Q_OBJECT

	SCOMProtocol *m_scom_protocol;
	
public:
    explicit scom_tcp_server(QObject *parent = 0);
    void startServer();
	void RegisterScomProtocol(SCOMProtocol *protocol) { m_scom_protocol = protocol; }

signals:
    
public slots:

protected:
    void incomingConnection(qintptr socketDescriptor);

};

#endif // SCOM_TCP_SERVER_H
