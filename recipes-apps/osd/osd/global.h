/*****************************************************************************
*   Fie:		global.h
*   Project:	OSD North Invent
*   Decription:
******************************************************************************/
#ifndef GLOBAL_H
#define GLOBAL_H

#define SW_REL_MAJ          "1"
#define SW_REL_MIN          "6"

//#define DEBUG_WINDOWS
//#define DEBUG_IMX_LORENZO
//#define DEBUG_AVVIA_SUBITO

#define _IMG_PATH_WIN   "file:///C:/LAVORO/ipTRONIX/OSD_SAM_Scaler_software/SW/OSD_Desktop_SCOM/proj/img/"

#define NUM_INPUTS              4
#define OSD_TIMEOUT             20//sec
#define TEMPER_UPDATE_TIME      3 //sec

#define MAX_OSD_BRIGHTNESS		100
#define MAX_OSD_CONTRAST		100
#define MAX_OSD_BUZZER			100

#define MAX_SCOM_BRIGHTNESS		100
#define MAX_SCOM_BUZZER			255

#define ANSI_COLOURS

#ifdef ANSI_COLOURS
# define COL_RED	"\e[1;31m"	//Red
# define COL_GREEN	"\e[1;32m"	//Green
# define COL_YELLOW	"\e[1;33m"	//Yellow
# define COL_BLUE	"\e[1;34m"	//Blue (scuro)
# define COL_PURP	"\e[1;35m"	//Purple
# define COL_CYAN	"\e[1;36m"	//Cyan
# define COL_NORMAL	"\e[0m"     //Reset color
#else
# define COL_RED
# define COL_GREEN
# define COL_YELLOW
# define COL_BLUE
# define COL_PURP
# define COL_CYAN
# define COL_NORMAL
#endif

#define PRINT_FUNC_NAME  { qDebug() << COL_YELLOW "@" <<  __FUNCTION__ << COL_NORMAL; }

#define HI(x) (unsigned char)((x) / 256)
#define LO(x) (unsigned char)((x) % 256)

#endif // GLOBAL_H
