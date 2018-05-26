/****************************************************************************/
#ifndef  MIBO3STATUS_H_INCLUDED
#define  MIBO3STATUS_H_INCLUDED

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
	float              	      O3ppb;
	uint8           		  TargetStatus;
	float                     Ro;
	int16                     i16Volts;
}tsMibO3statusPerm;

typedef struct
{
	float                     O3target;
	float            	      O3Min;
	float              	      O3Max;
}tsMibO3statusTemp;

typedef struct
{
    /* MIB handles */
	thJIP_Mib 			   		hMib;

	/* PDM record descriptor */
	PDM_tsRecordDescriptor 		sDesc;

	/* Data pointers */
	tsMibO3statusPerm	sPerm;
	tsMibO3statusTemp	sTemp;
	/* Pointers to other MIBs */
     void                    *pvMibAdcStatus;

	/* Other data */
   	uint32					  u32NotifyChanged;		/* Trap notification flags */
   	uint8                     u8AdcSrcBusVolts;
} tsMibO3status;

/****************************************************************************/
/***        Public Data                                     			  ***/
/****************************************************************************/

/****************************************************************************/
/***        Public Function Prototypes                                    ***/
/****************************************************************************/
PUBLIC void MibO3status_vInit(thJIP_Mib         	  hMibO3statusInit,
                               		 tsMibO3status *psMibO3statusInit,
                               		 uint8  	   u8AdcSrcBusVoltsInit);
PUBLIC void MibO3status_vRegister(void);
PUBLIC void MibO3status_vAppTimer(uint32 u32TimerSeconds );
PUBLIC void MibO3status_vAnalogue(uint8 u8Adc);


#endif  /* MIBO3STATUS_H_INCLUDED */
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
