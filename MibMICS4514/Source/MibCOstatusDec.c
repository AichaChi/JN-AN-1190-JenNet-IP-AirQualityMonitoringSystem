/*
 * MibCOconfigDec.c
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
#include "MibCOstatus.h"
#include "MibSensors.h"
/****************************************************************************/
/***        MIB structure                                                 ***/
/****************************************************************************/
PUBLIC tsMibCOstatus sMibCOstatus;


/****************************************************************************/
/***        MIB declaration                                               ***/
/****************************************************************************/
/* Registering MIB ? */
#if MK_REG_MIB_CO_STATUS

#define DECLARE_MIB
#include "MibCOstatusDef.h"
JIP_START_DECLARE_MIB(COstatusDef, COstatus)
JIP_CALLBACK(COppm,         NULL,                 vGetUint32, &sMibCOstatus.sPerm.COppm)
JIP_CALLBACK(TargetStatus,  NULL,                 vGetUint8,  &sMibCOstatus.sPerm.TargetStatus)
JIP_CALLBACK(COMin,         NULL,                 vGetUint32, &sMibCOstatus.sTemp.COMin)
JIP_CALLBACK(COMax,         NULL,                 vGetUint32, &sMibCOstatus.sTemp.COMax)
JIP_CALLBACK(Ro,            NULL,                 vGetUint32, &sMibCOstatus.sPerm.Ro)
JIP_CALLBACK(COTarget,      NULL ,                vGetUint32, &sMibCOstatus.sTemp.COtarget)
JIP_CALLBACK(i16Volts,      NULL ,          vGetUint32, &sMibCOstatus.sPerm.i16Volts)
JIP_END_DECLARE_MIB(COstatus, hCOstatus)

/* Public MIB handle */
PUBLIC const thJIP_Mib hMibCOstatus = &sCOstatusMib.sMib;

#else

/* Public MIB handle */
PUBLIC const thJIP_Mib hMibCOstatus = NULL;

#endif

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/


