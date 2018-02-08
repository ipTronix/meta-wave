/*****************************************************************************
*   Fie:		parameters.h
*   Project:	OSD North Invent
*   Decription:
******************************************************************************/
#ifndef PARAMETERS_H
#define PARAMETERS_H

//parametri singoli PARAM_SET
enum e_params {
    osdBRIGHTNESS    = 0,
    osdCONTRAST		 = 1,
    osdBUZZER		 = 2,
    osdPOWER_MODE	 = 3,
    osdSCALING_MODE	 = 4,
    osdNOSIG_TOUT	 = 5,
    osdSTDBY_TOUT	 = 6,
    osdTEMPERATURE	 = 7,
    osdMIN_TEMP		 = 8,
    osdMAX_TEMP		 = 9,        
    osdLOGO_TIMEOUT  = 10,
    osdMON_SER_ADR   = 11,
    osdJ24_CFG       = 12,
    osdMULTI_DIM     = 13,
    osdLOCK_ON_SER   = 14,
    osdLOCK_PIN      = 15,
	osdPOT_ENA_SER	 = 16,
    osdINVALID
};

#endif // PARAMETERS_H
