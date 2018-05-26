/****************************************************************************/
#ifndef  MIBH2SSTATUS_H_INCLUDED
#define  MIBH2SSTATUS_H_INCLUDED

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
	float              	      H2Sppm;
	uint8           		  TargetStatus;
	float                     Ro;
	int16                     i16Volts;
}tsMibH2SstatusPerm;

typedef struct
{
	float                     H2Starget;
	float            	      H2SMin;
	float              	      H2SMax;
}tsMibH2SstatusTemp;

typedef struct
{
    /* MIB handles */
	thJIP_Mib 			   		hMib;

	/* PDM record descriptor */
	PDM_tsRecordDescriptor 		sDesc;

	/* Data pointers */
	tsMibH2SstatusPerm	sPerm;
	tsMibH2SstatusTemp	sTemp;
	/* Pointers to other MIBs */
     void                    *pvMibAdcStatus;

	/* Other data */
   	uint32					  u32NotifyChanged;		/* Trap notification flags */
   	uint8                     u8AdcSrcBusVolts;
} tsMibH2Sstatus;

/****************************************************************************/
/***        Public Data                                     			  ***/
/****************************************************************************/

/****************************************************************************/
/***        Public Function Prototypes                                    ***/
/****************************************************************************/
PUBLIC void MibH2Sstatus_vInit(thJIP_Mib         	  hMibH2SstatusInit,
                               		 tsMibH2Sstatus *psMibH2SstatusInit,
                               		 uint8  	   u8AdcSrcBusVoltsInit);
PUBLIC void MibH2Sstatus_vRegister(void);
PUBLIC void MibH2Sstatus_vAppTimer(uint32 u32TimerSeconds );
PUBLIC void MibH2Sstatus_vAnalogue(uint8 u8Adc);


#endif  /* MIBCH2SSTATUS_H_INCLUDED */
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
