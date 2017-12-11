/*****************************************************************************
*   Fie:		serialprotocol.cpp
*   Project:	OSD North Invent
*   Decription:
******************************************************************************/
#include "global.h"
#ifndef DEBUG_WINDOWS
#include <termios.h>
#include <sys/poll.h>
#endif
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <QEventLoop>
#include <QKeyEvent>
#include <QCoreApplication>
#include "parameters.h"
#include "SCOMProtocol.h"

#define MIN_MESSAGE_LEN		7

#define ADDR_OFFS			1
#define CMD_OFFS			2
#define LEN_OFFS			5
#define IHCHK_OFFS			6
#define DATA_OFFS			7

const SCOM_STRUCT m_asSCOMData[scomINVALID] =
{
	{{'T','M','P'}, 1, 2},			//scomTMP,
	{{'M','A','N'}, 0, 0xFF},		//scomMAN,
	{{'T','Y','P'}, 0, 0xFF},		//scomTYP,
	{{'B','R','T'}, 1, 1},			//scomBRT,
	{{'S','L','I'}, 1, 1},			//scomSLI,
	{{'S','W','I'}, 0, 4},			//scomSWI,
	{{'E','T','C'}, 0, 4},			//scomETC,
	{{'S','N','B'}, 0, 4},			//scomSNB,
	{{'L','F','D'}, 0, 0},			//scomLFD,
	{{'B','Z','Z'}, 1, 1},			//scomBZZ,
	{{'M','C','C'}, 1, 0},			//scomMCC,
};

//-----------------------------------------------------------------------
SCOMProtocol::SCOMProtocol(QObject *parent) :
    QObject(parent)
{
    PRINT_FUNC_NAME
    m_dm = 0;
	m_ql = 0;
}

eSCOM_CMD SCOMProtocol::FindScomCmd(char* pacCmd, unsigned char *pcLen)
{
	int i;

	*pcLen = 0;
	for (i=0; i<scomINVALID; i++)
	{
		if (memcmp(m_asSCOMData[i].acCmd, pacCmd, 3) == 0)
		{
			break;
		}
	}

	if (i < scomINVALID)
	{
		*pcLen = m_asSCOMData[i].len;
	}

	return (eSCOM_CMD)i;
}

void SCOMProtocol::GetScomLabel(eSCOM_CMD eCmd, char* pLabel)
{
	if (eCmd < scomINVALID)
	{
		memcpy(pLabel, m_asSCOMData[eCmd].acCmd, 3);
	}
}

unsigned char SCOMProtocol::GetScomAnswerLen(eSCOM_CMD eCmd)
{
	unsigned char ret = 0;

	if (eCmd < scomINVALID)
	{
		ret = m_asSCOMData[eCmd].answer_len;
	}

	return ret;
}

unsigned char SCOMProtocol::GetCHK(unsigned char* pacData, unsigned char len){
	unsigned char chk = 0;

	for (int i=0; i<len; i++)
	{
		chk += pacData[i];
	}

	return ~chk;
}

void SCOMProtocol::GetAnswer(bool bACK, eSCOM_CMD eCmd, QByteArray* pAnswer)
{
	unsigned char acAnswer[30];
	unsigned int dwTempData;
	unsigned short int wTempData;
	unsigned char cTempData;
	unsigned char msg_len;
	int i;

	acAnswer[0] = (bACK)? ACK_VAL : NAK_VAL;
	acAnswer[1] = (unsigned char)((m_dm->property("monSerialAdr")).toInt());
	GetScomLabel(eCmd, (char*)&acAnswer[2]);
	acAnswer[5] = GetScomAnswerLen(eCmd);
	acAnswer[6] = GetCHK(&acAnswer[0], 6);

	switch(eCmd)
	{
		case scomTMP:
			wTempData = (unsigned short int)(m_dm->Curr_TemperatureC & 0xFFFF);
			acAnswer[7] = (unsigned char)(wTempData >> 8);
			acAnswer[8] = (unsigned char)(wTempData);
			acAnswer[9] = GetCHK(&acAnswer[7], 2);
			msg_len = 10;
			break;

		case scomMAN:
			for (i=0; i<m_dm->array_manufacturer_ID.size(); i++)
			{
				if (m_dm->array_manufacturer_ID.at(i) == 0)
				{
					break;
				}
				acAnswer[7+i] = m_dm->array_manufacturer_ID.at(i);
			}
			acAnswer[5] = i;
			acAnswer[6] = GetCHK(&acAnswer[0], 6);
			if (i>0)
			{
				acAnswer[7+i] = GetCHK(&acAnswer[7], i);
				msg_len = 7+i+1;
			}
			else
			{
				msg_len = 7;
			}
			break;

		case scomTYP:
			for (i=0; i<m_dm->array_monitor_type.size(); i++)
			{
				if (m_dm->array_monitor_type.at(i) == 0)
				{
					break;
				}
				acAnswer[7+i] = m_dm->array_monitor_type.at(i);
			}
			acAnswer[5] = i;
			acAnswer[6] = GetCHK(&acAnswer[0], 6);
			if (i>0)
			{
				acAnswer[7+i] = GetCHK(&acAnswer[7], i);
				msg_len = 7+i+1;
			}
			else
			{
				msg_len = 7;
			}
			break;

		case scomBRT:
			dwTempData = (m_dm->property("dimLevel")).toInt();
			dwTempData = (dwTempData*MAX_SCOM_BRIGHTNESS + (MAX_OSD_BRIGHTNESS/2)) / MAX_OSD_BRIGHTNESS;
			acAnswer[7] = (unsigned char)dwTempData;
			acAnswer[8] = GetCHK(&acAnswer[7], 1);
			msg_len = 9;
			break;

		case scomSLI:
			switch((m_dm->property("currentIN")).toInt())
			{
				case 0:
					cTempData = 0x03;
					break;
				case 1:
					cTempData = 0x0F;
					break;
				case 2:
					cTempData = 0x04;
					break;
				case 3:
					cTempData = 0x10;
					break;
				default:
					cTempData = 0x03;
					break;
			}
			acAnswer[7] = cTempData;
			acAnswer[8] = GetCHK(&acAnswer[7], 1);
			msg_len = 9;
			break;

		case scomSWI:
			acAnswer[7] = m_dm->array_SoftwareID.at(0);
			acAnswer[8] = m_dm->array_SoftwareID.at(1);
			acAnswer[9] = m_dm->array_SoftwareID.at(2);
			acAnswer[10] = m_dm->array_SoftwareID.at(3);
			acAnswer[11] = GetCHK(&acAnswer[7], 4);
			msg_len = 12;
			break;

		case scomETC:
			dwTempData = (m_dm->property("totalRunTime")).toInt();
			acAnswer[7] = (dwTempData>>24);
			acAnswer[8] = (dwTempData>>16);
			acAnswer[9] = (dwTempData>>8);
			acAnswer[10] = dwTempData;
			acAnswer[11] = GetCHK(&acAnswer[7], 4);
			msg_len = 12;
			break;

		case scomSNB:
			acAnswer[7] = m_dm->array_MonitorSerial.at(0);
			acAnswer[8] = m_dm->array_MonitorSerial.at(1);
			acAnswer[9] = m_dm->array_MonitorSerial.at(2);
			acAnswer[10] = m_dm->array_MonitorSerial.at(3);
			acAnswer[11] = GetCHK(&acAnswer[7], 4);
			msg_len = 12;
			break;

		case scomLFD:
			msg_len = 7;
			break;

		case scomBZZ:
			dwTempData = (m_dm->property("buzzerVol")).toInt();
			dwTempData = (dwTempData*MAX_SCOM_BUZZER + (MAX_OSD_BUZZER/2)) / MAX_OSD_BUZZER;
			acAnswer[7] = (unsigned char)dwTempData;
			acAnswer[8] = GetCHK(&acAnswer[7], 1);
			msg_len = 9;
			break;

		case scomMCC:
			msg_len = 7;
			break;

		default:
			return;
	}

	pAnswer->resize(msg_len);
	for (i=0; i<msg_len; i++)
	{
		(*pAnswer)[i] = acAnswer[i];
	}
}

unsigned char SCOMProtocol::Parse(CircBuff* scom_buff, QByteArray* pAnswer)
{
	int count;
	unsigned char checksum;
	unsigned char address;
	char acCmd[3];
	int i;
	eSCOM_CMD eScomCmd;
	unsigned char cmd_len;
	unsigned char current_len;
	unsigned char msg_chk;
	unsigned char packet_len;
	bool chk_ok;
	bool bACK;
	bool bOsdLock;
	unsigned char status = 0;
	unsigned char scom_addr = (unsigned char)((m_dm->property("monSerialAdr")).toInt());
	
	while(true)
	{
		count = scom_buff->count();
		
		if (count < MIN_MESSAGE_LEN)
		{
			status = 0;
			break;
		}

		//Header has been received
		if (scom_buff->testCharAtIdx(0) != SCOM_ATT)
		{
			scom_buff->getChar();
			//qDebug("ATT not ok");
			continue;
		}

		checksum = SCOM_ATT;
		//qDebug("checksum = %d", checksum);
		address = scom_buff->testCharAtIdx(ADDR_OFFS);
		//qDebug("address = %d", address);
		
		checksum += address;
		//qDebug("checksum = %d", checksum);

		for (i=0; i<3; i++)
		{
			acCmd[i] = scom_buff->testCharAtIdx(CMD_OFFS + i);
			checksum += (unsigned char)acCmd[i];
			//qDebug("checksum = %d", checksum);
		}
		
		eScomCmd = FindScomCmd(acCmd, &cmd_len);
		current_len = scom_buff->testCharAtIdx(LEN_OFFS);
		checksum += current_len;
		//qDebug("checksum = %d", checksum);

		msg_chk = scom_buff->testCharAtIdx(IHCHK_OFFS);
		//qDebug("message checksum = %d", msg_chk);

		chk_ok = ((unsigned char)(checksum + msg_chk) == 0xFF)? true : false;

		if (!chk_ok && eScomCmd == scomINVALID)
		{
			scom_buff->getChar();
			//qDebug("CHK not ok");
			continue;
		}

		if (chk_ok)
		{
			packet_len = MIN_MESSAGE_LEN + ((current_len == 0)? 0 : current_len+1);
		}
		else
		{
			packet_len = MIN_MESSAGE_LEN + ((cmd_len == 0)? 0 : cmd_len+1);
		}

		if (count < packet_len)
		{
			//packet not completed
			status = 0;
			break;
		}
		
		//qDebug("Packet Complete");
		
		//Packet has been received completely
		if ((current_len != cmd_len && eScomCmd != scomINVALID) ||
			(address != BROADCAST_ADDR && address != scom_addr) ||
			((address == BROADCAST_ADDR) && ((eScomCmd == scomINVALID) || !chk_ok)))
		{
			//Packet is not for us or is in broadcast but with errors. Just discard it.
			scom_buff->getString(0, packet_len);
			//qDebug("Broadcast with errors");
			continue;
		}

		if (eScomCmd == scomINVALID)
		{
			//Answer NAK SWI
			scom_buff->getString(0, packet_len);
			GetAnswer(false, scomSWI, pAnswer);
			status = (KEEP_PARSE_FLAG | SEND_ANSWER_FLAG);
			//qDebug("Invalid command");
			break;
		}

		if (!chk_ok)
		{
			//Answer NAK Command
			scom_buff->getString(0, packet_len);
			GetAnswer(false, eScomCmd, pAnswer);
			status = (KEEP_PARSE_FLAG | SEND_ANSWER_FLAG);
			break;
		}

		//Check data checksum
		if (cmd_len > 0)
		{
			checksum = 0;
			for (i=0; i<=cmd_len; i++)
			{
				checksum += scom_buff->testCharAtIdx(DATA_OFFS + i);
			}

			if (checksum != 0xFF)
			{
				//qDebug("Invalid data checksum");
				scom_buff->getString(0, packet_len);

				if (address == scom_addr)
				{
					GetAnswer(false, eScomCmd, pAnswer);
					status = (KEEP_PARSE_FLAG | SEND_ANSWER_FLAG);
					break;
				}
				continue;
			}
		}

		bOsdLock = true;
		
		//Correct packet
		//qDebug("Correct Packet");
		
		switch(eScomCmd)
		{
			case scomTMP:
				bACK = (scom_buff->testCharAtIdx(DATA_OFFS) == 0x52)? true : false;
				break;

			case scomMAN:
				bACK = true;
				break;

			case scomTYP:
				bACK = true;
				break;

			case scomBRT:
				if (scom_buff->testCharAtIdx(DATA_OFFS) <= MAX_SCOM_BRIGHTNESS)
				{
					int val2set = (scom_buff->testCharAtIdx(DATA_OFFS)*MAX_OSD_BRIGHTNESS + (MAX_SCOM_BRIGHTNESS/2)) / MAX_SCOM_BRIGHTNESS;
					
					m_dm->setProperty("dimLevel", val2set);
					m_dm->updateSetting(osdBRIGHTNESS, val2set);
					bACK = true;
				}
				else
				{
					bACK = false;
				}
				break;

			case scomSLI:
				{
					//Set input in pipe 0
					int src_idx;
					bool bValid = true;

					bACK = false;
					switch(scom_buff->testCharAtIdx(DATA_OFFS))
					{
						case 0x03:
							src_idx = 0;//HwVideoSystem::srcDVI1;
							if ((m_dm->property("inputs1Disable")).toBool())
							{
								bValid = false;
							}
							break;

						case 0x04:
							src_idx = 2;//HwVideoSystem::srcDVI2;
							break;

						case 0x0F:
							src_idx = 1;//HwVideoSystem::srcDP1;
							if ((m_dm->property("inputs1Disable")).toBool())
							{
								bValid = false;
							}
							break;

						case 0x10:
							src_idx = 3;//HwVideoSystem::srcDP2;
							break;

						default:
							bValid = false;
							break;
					}

					if (bValid && (m_dm->property("inputMode")).toInt() == 0)
					{
						m_dm->setProperty("currentIN", src_idx);
						m_dm->updateSourceSettings();
						bACK = true;
					}
				}
				break;

			case scomSWI:
				bACK = true;
				break;

			case scomETC:
				bACK = true;
				break;

			case scomSNB:
				bACK = true;
				break;

			case scomLFD:
				m_dm->doFactoryReset();
				bACK = true;
				break;

			case scomBZZ:
				if (scom_buff->testCharAtIdx(DATA_OFFS) <= MAX_SCOM_BUZZER)
				{
					int val2set = (scom_buff->testCharAtIdx(DATA_OFFS)*MAX_OSD_BUZZER + (MAX_SCOM_BUZZER/2)) / MAX_SCOM_BUZZER;
					m_dm->setProperty("buzzerVol", val2set);
					m_dm->updateSetting(osdBUZZER, val2set);
					bACK = true;
				}
				else
				{
					bACK = false;
				}
				break;

			case scomMCC:
				QKeyEvent *event = 0;
				QKeyEvent *event_release = 0;
				bACK = true;
				switch(scom_buff->testCharAtIdx(DATA_OFFS))
				{
					case 0xF7:	/* Menu */
						qDebug("enter key");
						event = new QKeyEvent ( QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier);
						break;
						
					case 0xFB:	/* Esc */
						qDebug("escape key");
						event = new QKeyEvent ( QEvent::KeyPress, Qt::Key_Escape, Qt::NoModifier);
						event_release = new QKeyEvent ( QEvent::KeyRelease, Qt::Key_Escape, Qt::NoModifier);
						break;
						
					case 0xFC:	/* Plus */
						qDebug("plus key");
						event = new QKeyEvent ( QEvent::KeyPress, Qt::Key_Plus, Qt::NoModifier);
						break;
						
					case 0xFD:	/* Minus */
						qDebug("minus key");
						event = new QKeyEvent ( QEvent::KeyPress, Qt::Key_Minus, Qt::NoModifier);
						break;
					
					default:
						bACK = false;
						break;
				}
				
				if (event != 0)
				{
					QCoreApplication::postEvent(m_ql->getEngine()->rootObjects().first(), event);
					if (event_release != 0)
					{
						QCoreApplication::postEvent(m_ql->getEngine()->rootObjects().first(), event_release);
					}
				}
				
				bOsdLock = false;
				break;
		}

		if (bOsdLock && (m_dm->property("lockOnSerial")).toBool())
		{
			m_dm->setProperty("serLockActive", true);
			emit m_dm->closeOSD();				
		}
		
		scom_buff->getString(0, packet_len);
		
		if (address != BROADCAST_ADDR)
		{
			//Answer ACK Command
			GetAnswer(bACK, eScomCmd, pAnswer);
			status = (KEEP_PARSE_FLAG | SEND_ANSWER_FLAG);
			break;
		}

		continue;
	}
	
	return status;
}

/* EOF */

