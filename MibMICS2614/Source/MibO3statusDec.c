/*
 * MibO3statusDec.c
 *
 *  Created on: 5 août 2016
 *  Author: lenovo-pc
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
#include "MibO3status.h"
#include "MibSensors.h"
/****************************************************************************/
/***        MIB structure                                                 ***/
/****************************************************************************/
PUBLIC tsMibO3status sMibO3status;


/****************************************************************************/
/***        MIB declaration                                               ***/
/****************************************************************************/
/* Registering MIB ? */
#if MK_REG_MIB_O3_STATUS

#define DECLARE_MIB
#include "MibO3statusDef.h"
JIP_START_DECLARE_MIB(O3statusDef, O3status)
JIP_CALLBACK(O3ppb,          NULL,                 vGetUint32, &sMibO3status.sPerm.O3ppb)
JIP_CALLBACK(TargetStatus,   NULL,                 vGetUint8,  &sMibO3status.sPerm.TargetStatus)
JIP_CALLBACK(O3Min,          NULL,                 vGetUint32, &sMibO3status.sTemp.O3Min)
JIP_CALLBACK(O3Max,          NULL,                 vGetUint32, &sMibO3status.sTemp.O3Max)
JIP_CALLBACK(Ro,             NULL,                 vGetUint32, &sMibO3status.sPerm.Ro)
JIP_CALLBACK(O3target,       NULL,                 vGetUint32, &sMibO3status.sTemp.O3target)
JIP_CALLBACK(i16Volts,       NULL,                 vGetUint32, &sMibO3status.sPerm.i16Volts)
JIP_END_DECLARE_MIB(O3status, hO3status)

/* Public MIB handle */
PUBLIC const thJIP_Mib hMibO3status = &sO3statusMib.sMib;

#else

/* Public MIB handle */
PUBLIC const thJIP_Mib hMibO3status = NULL;

#endif

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/


