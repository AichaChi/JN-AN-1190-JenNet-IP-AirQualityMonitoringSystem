/****************************************************************************/
#ifndef  MIBNO2STATUS_H_INCLUDED
#define  MIBNO2STATUS_H_INCLUDED

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/
/* SDK includes */
#include <jendefs.h>
/* JenOS includes */
#include <os.h>
#include <pdm.h>
#include <JIP.h>
#include <6LP.h>
/* Application common includes */
#include "Table.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
typedef struct
{
	//float                     resvalue;
	float              	      NO2ppb;
	uint8           		  TargetStatus;
	float                     Ro;
	int16                     i16Volts;
}tsMibNO2statusPerm;

typedef struct
{
	float                     NO2target;
	float            	      NO2Min;
	float              	      NO2Max;
}tsMibNO2statusTemp;

typedef struct
{
    /* MIB handles */
	thJIP_Mib 			   		hMib;

	/* PDM record descriptor */
	PDM_tsRecordDescriptor 		sDesc;

	/* Data pointers */
	tsMibNO2statusPerm	sPerm;
	tsMibNO2statusTemp	sTemp;
	/* Pointers to other MIBs */
     void                    *pvMibAdcStatus;

	/* Other data */
   	uint32					  u32NotifyChanged;		/* Trap notification flags */
   	uint8                     u8AdcSrcBusVolts;
} tsMibNO2status;

/****************************************************************************/
/***        Public Data                                     			  ***/
/****************************************************************************/

/****************************************************************************/
/***        Public Function Prototypes                                    ***/
/****************************************************************************/
PUBLIC void MibNO2status_vInit(thJIP_Mib         	  hMibNO2statusInit,
                               		 tsMibNO2status *psMibNO2statusInit,
                               		 uint8  	   u8AdcSrcBusVoltsInit);
PUBLIC void MibNO2status_vRegister(void);
PUBLIC void MibNO2status_vAppTimer(uint32 u32TimerSeconds );
PUBLIC void MibNO2status_vAnalogue(uint8 u8Adc);


#endif  /* MIBNO2STATUS_H_INCLUDED */
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
