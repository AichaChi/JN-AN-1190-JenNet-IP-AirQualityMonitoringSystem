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
#include "MibNO2status.h"
#include "MibSensors.h"
/****************************************************************************/
/***        MIB structure                                                 ***/
/****************************************************************************/
PUBLIC tsMibNO2status sMibNO2status;


/****************************************************************************/
/***        MIB declaration                                               ***/
/****************************************************************************/
/* Registering MIB ? */
#if MK_REG_MIB_NO2_STATUS

#define DECLARE_MIB
#include "MibNO2statusDef.h"
JIP_START_DECLARE_MIB(NO2statusDef, NO2status)
JIP_CALLBACK(NO2ppb,         NULL,                 vGetUint32, &sMibNO2status.sPerm.NO2ppb)
JIP_CALLBACK(TargetStatus,   NULL,                 vGetUint8,  &sMibNO2status.sPerm.TargetStatus)
JIP_CALLBACK(NO2Min,         NULL,                 vGetUint32, &sMibNO2status.sTemp.NO2Min)
JIP_CALLBACK(NO2Max,         NULL,                 vGetUint32, &sMibNO2status.sTemp.NO2Max)
JIP_CALLBACK(Ro,             NULL,                 vGetUint32, &sMibNO2status.sPerm.Ro)
JIP_CALLBACK(NO2target,      NULL ,                vGetUint32, &sMibNO2status.sTemp.NO2target)
JIP_CALLBACK(i16Volts,       NULL ,           vGetUint32, &sMibNO2status.sPerm.i16Volts)
JIP_END_DECLARE_MIB(NO2status, hNO2status)

/* Public MIB handle */
PUBLIC const thJIP_Mib hMibNO2status = &sNO2statusMib.sMib;

#else

/* Public MIB handle */
PUBLIC const thJIP_Mib hMibNO2status = NULL;

#endif

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/


