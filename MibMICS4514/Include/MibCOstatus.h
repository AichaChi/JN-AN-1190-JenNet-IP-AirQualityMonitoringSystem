/****************************************************************************/
#ifndef  MIBCOSTATUS_H_INCLUDED
#define  MIBCOSTATUS_H_INCLUDED

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
/* CO status temporary data */
typedef struct
{
	//float                     resvalue;
	float              	      COppm;
	uint8           		  TargetStatus;
	float                     Ro;
	int16                     i16Volts;
}tsMibCOstatusPerm;

/* Network control mib */
typedef struct
{
	float                     COtarget;
	float            	      COMin;
	float              	      COMax;
}tsMibCOstatusTemp;

typedef struct
{
    /* MIB handles */
	thJIP_Mib 			   		hMib;

	/* PDM record descriptor */
	PDM_tsRecordDescriptor 		sDesc;

	/* Data pointers */
	tsMibCOstatusPerm	sPerm;
	tsMibCOstatusTemp	sTemp;
	/* Pointers to other MIBs */
     void                    *pvMibAdcStatus;

	/* Other data */
   	uint32					  u32NotifyChanged;		/* Trap notification flags */
   	uint8                     u8AdcSrcBusVolts;
} tsMibCOstatus;

/****************************************************************************/
/***        Public Data                                     			  ***/
/****************************************************************************/

/****************************************************************************/
/***        Public Function Prototypes                                    ***/
/****************************************************************************/
PUBLIC void MibCOstatus_vInit(thJIP_Mib         	  hMibCOstatusInit,
                               		 tsMibCOstatus *psMibCOstatusInit,
                               		 uint8  	   u8AdcSrcBusVoltsInit);
PUBLIC void MibCOstatus_vRegister(void);
PUBLIC void MibCOstatus_vAppTimer(uint32 u32TimerSeconds );
PUBLIC void MibCOstatus_vAnalogue(uint8 u8Adc);


#endif  /* MIBCOSTATUS_H_INCLUDED */
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
