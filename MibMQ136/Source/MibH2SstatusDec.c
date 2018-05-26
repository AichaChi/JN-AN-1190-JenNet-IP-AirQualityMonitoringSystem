/*
 * MibH2SconfigDec.c
 *
 *  Created on: 5 août 2016
 *      Author: lenovo-pc
 */
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
/* SDK includes */
#include <jendefs.h>
/* Stack includes */
#include <JIP.h>
#include <AccessFunctions.h>
/* Application common includes */
#include "MibH2Sstatus.h"
#include "MibSensors.h"
/****************************************************************************/
/***        MIB structure                                                 ***/
/****************************************************************************/
PUBLIC tsMibH2Sstatus sMibH2Sstatus;


/****************************************************************************/
/***        MIB declaration                                               ***/
/****************************************************************************/
/* Registering MIB ? */
#if MK_REG_MIB_H2S_STATUS

#define DECLARE_MIB
#include "MibH2SstatusDef.h"
JIP_START_DECLARE_MIB(H2SstatusDef, H2Sstatus)
JIP_CALLBACK(H2Sppm,         NULL,                 vGetUint32, &sMibH2Sstatus.sPerm.H2Sppm)
JIP_CALLBACK(TargetStatus,   NULL,                 vGetUint8,  &sMibH2Sstatus.sPerm.TargetStatus)
JIP_CALLBACK(H2SMin,         NULL,                 vGetUint32, &sMibH2Sstatus.sTemp.H2SMin)
JIP_CALLBACK(H2SMax,         NULL,                 vGetUint32, &sMibH2Sstatus.sTemp.H2SMax)
JIP_CALLBACK(Ro,             NULL,                 vGetUint32, &sMibH2Sstatus.sPerm.Ro)
JIP_CALLBACK(H2STarget,      NULL ,                vGetUint32, &sMibH2Sstatus.sTemp.H2Starget)
JIP_CALLBACK(i16Volts,       NULL ,                vGetUint32, &sMibH2Sstatus.sPerm.i16Volts)
JIP_END_DECLARE_MIB(H2Sstatus, hH2Sstatus)

/* Public MIB handle */
PUBLIC const thJIP_Mib hMibH2Sstatus = &sH2SstatusMib.sMib;

#else

/* Public MIB handle */
PUBLIC const thJIP_Mib hMibH2Sstatus = NULL;

#endif

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/


