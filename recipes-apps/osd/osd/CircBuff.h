/*****************************************************************************
*   Fie:		CircBuff.h
*   Project:	OSD North Invent
*   Decription:
*   Note: 		This object is not thread safe
******************************************************************************/
#ifndef CIRCBUFF_H
#define CIRCBUFF_H

#include <QObject>
#include <QByteArray>

class CircBuff : public QObject
{
    Q_OBJECT

	unsigned int read_ptr;
	unsigned int write_ptr;
	QByteArray buffer;

public:
    explicit CircBuff(int buff_size, QObject *parent = 0);
	int count(void);
	int count_free(void);
	unsigned char testCharAtIdx(int idx);
	unsigned char getChar(void);
	void putChar(unsigned char ch);
	void putString(QByteArray Data);
	void getString(unsigned char* pacData, int len);
	int getBuffSize(void);
};

#endif // CIRCBUFF_H
