/*****************************************************************************
*   Fie:		debug.h
*   Project:	OSD North Invent
*   Decrizione:
******************************************************************************/
#ifndef DEBUG_H
#define DEBUG_H

#define TEMP_REVISION "0.1"

//#define DEBUG_WIN
//#define DEBUG_TTY_LORENZO
#define DEBUG_NO_TEMP_MONITOR

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


#endif // DEBUG_H
