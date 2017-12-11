#include "CircBuff.h"
#include <QDebug>

CircBuff::CircBuff(int size, QObject *parent) :
    QObject(parent)
{
	read_ptr = 0;
	write_ptr = 0;
	buffer.resize(size);
}

int CircBuff::count(void)
{
	int retVal;
	
	if (write_ptr >= read_ptr)
	{
		retVal = write_ptr - read_ptr;
	}
	else
	{
		retVal = buffer.size() + write_ptr - read_ptr;
	}
	
	return retVal;
}

int CircBuff::count_free(void)
{
	return (buffer.size() - count());
}

unsigned char CircBuff::testCharAtIdx(int idx)
{
	idx += read_ptr;
	
	if (idx >= buffer.size())
	{
		idx -= buffer.size();
	}
	
	return (unsigned char)buffer.at(idx);
}

unsigned char CircBuff::getChar(void)
{
	int retVal = buffer.at(read_ptr);
	
	read_ptr++;
	
	if (read_ptr >= buffer.size())
	{
		read_ptr = 0;
	}
	
	return retVal;
}

void CircBuff::putChar(unsigned char ch)
{
	buffer[write_ptr] = ch;
	
	write_ptr++;
	
	if (write_ptr >= buffer.size())
	{
		write_ptr = 0;
	}
}

void CircBuff::putString(QByteArray Data)
{
	int i;
	
	for (i=0; i<Data.size(); i++)
	{
		putChar(Data.at(i));
	}
	
	//qDebug() << " CircBuff: " << buffer;
}

void CircBuff::getString(unsigned char* pacData, int len)
{
	int i;
	
	for (i=0; i<len; i++)
	{
		if (pacData != 0)
		{
			pacData[i] = buffer.at(read_ptr);
		}
		
		read_ptr++;
	
		if (read_ptr >= buffer.size())
		{
			read_ptr = 0;
		}
	}
}

int CircBuff::getBuffSize(void)
{
	return buffer.size();
}

