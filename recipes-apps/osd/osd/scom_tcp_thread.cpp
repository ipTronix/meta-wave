#include "scom_tcp_thread.h"
#include "CircBuff.h"

scom_tcp_thread::scom_tcp_thread(qintptr ID, SCOMProtocol *scom_protocol, QObject *parent) :
    QThread(parent)
{
    socketDescriptor = ID;
	scom_buff = new CircBuff(128);
	this->scom_protocol = scom_protocol;
}

void scom_tcp_thread::run()
{
    // thread starts here
    qDebug() << " Thread started";

    socket = new QTcpSocket();

    // set the ID
    if(!socket->setSocketDescriptor(socketDescriptor))
    {
        // something's wrong, we just emit a signal
        emit error(socket->error());
        return;
    }

    // connect socket and signal
    // note - Qt::DirectConnection is used because it's multithreaded
    //        This makes the slot to be invoked immediately, when the signal is emitted.

    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));

    // We'll have multiple clients, we want to know which is which
    qDebug() << socketDescriptor << " Client connected";

    // make this thread a loop,
    // thread will stay alive so that signal/slot to function properly
    // not dropped out in the middle when thread dies

    exec();
}

void scom_tcp_thread::readyRead()
{
	int data_count;
	int space_in_queue;
	int data_2_copy;
	int pos;
	unsigned char parse_status;
	
    // get the information
    QByteArray Data = socket->readAll();
	
	//for (int j=0; j<Data.size(); j++)
	//	qDebug("%d", Data.at(j));
	
	data_count = Data.size();

	pos = 0;
	
	while(data_count>0)
	{
		space_in_queue = scom_buff->count_free();
		
		if (data_count <= space_in_queue)
		{
			data_2_copy = data_count;
		}
		else
		{
			data_2_copy = space_in_queue;
		}	
		
		scom_buff->putString(Data.mid(pos, data_2_copy));
		
		do
		{
			parse_status = scom_protocol->Parse(scom_buff, &answer);
			if (parse_status & SEND_ANSWER_FLAG)
			{
				socket->write(answer);
			}
		}while(parse_status & KEEP_PARSE_FLAG);
		
		pos += data_2_copy;
		data_count -= data_2_copy;
	}

}

void scom_tcp_thread::disconnected()
{
    qDebug() << socketDescriptor << " Disconnected";


    socket->deleteLater();
	scom_buff->deleteLater();
    exit(0);
}
