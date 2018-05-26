/*
 * MibCOstatus.c

 *
 *  Created on: 5 août 2016
 *      Author: lenovo-pc
 */
/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
/* Standard includes */
#include <string.h>
/* SDK includes */
#include <jendefs.h>
/* Hardware includes */
#include <AppHardwareApi.h>
#include <PeripheralRegs.h>
/* Stack includes */
#include <Api.h>
#include <AppApi.h>
#include <JIP.h>
#include <6LP.h>
#include <AccessFunctions.h>
/* JenOS includes */
#include <dbg.h>
#include <dbg_uart.h>
#include <os.h>
#include <pdm.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
/* Application device includes */
#include "Node.h"
#include "Address.h"
#include "MibNO2status.h"
#include "DriverOXsensor.h"
#include "DeviceDefs.h"
#include "MibSensors.h"
#include "MibAdcStatus.h"
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
/* Data access */
#define VAL_CAUTION_PPB          0
#define VAL_WARNING_PPB           0
#define VAL_THREAT0_PPB           0
#define VAL_THREAT1_PPB           0
#define VAL_THREAT2_PPB           0
#define VAL_THREAT3_PPB           0

#define DEBUG_MIB_NO2_STATUS      FALSE

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
PRIVATE tsMibNO2status *psMibNO2status;

/****************************************************************************/
/***        External variables                                            ***/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: MibNO2Status_vInit
 *
 * DESCRIPTION:
 * Initialises data
 *
 ****************************************************************************/

PUBLIC void MibNO2status_vInit(thJIP_Mib     hMibNO2statusInit,
                             tsMibNO2status *psMibNO2statusInit,
                              uint8  	   u8AdcSrcBusVoltsInit)
{
    /* Debug */
    DBG_vPrintf(DEBUG_MIB_NO2_STATUS, "\n%sMibCOstatus_vInit() {%d}", acDebugIndent, sizeof(tsMibNO2status));

    /* Valid data pointer ? */
    if (psMibNO2statusInit != (tsMibNO2status *) NULL)
    {
        /* Take copy of pointer to data */
        psMibNO2status = psMibNO2statusInit;
        /* Take a copy of the MIB handle */
        psMibNO2status->hMib = hMibNO2statusInit;
        /* Load Dio mib data */
         (void) PDM_eLoadRecord(&psMibNO2status->sDesc,
     							   (uint16)(MIB_ID_NO2_STATUS & 0xFFFF),
                                    (void *) &psMibNO2status->sPerm,
                                    sizeof(psMibNO2status->sPerm),
                                    FALSE);

        psMibNO2status->sTemp.NO2target  = VAL_NO2_TARGET;
        psMibNO2status->sTemp.NO2Min     = OX_SENSOR_MIN_NO2;
        psMibNO2status->sTemp.NO2Max     = OX_SENSOR_MAX_NO2;
		psMibNO2status->u8AdcSrcBusVolts = u8AdcSrcBusVoltsInit;
		psMibNO2status->sPerm.Ro     = OX_SENSOR_DEFAULT_RO ;

    }
}

/****************************************************************************
 *
 * NAME: MibCOStatus_vRegister
 *
 * DESCRIPTION:
 * Registers MIB
 *
 ****************************************************************************/
PUBLIC void MibNO2status_vRegister(void)
{
    teJIP_Status eStatus;
    /* Debug */
    DBG_vPrintf(DEBUG_MIB_NO2_STATUS, "\n%sMibNO2status_vRegister()", acDebugIndent);
    Node_vDebugIndent(DEBUG_MIB_NO2_STATUS);

    /* Register MIB */
    eStatus = eJIP_RegisterMib(psMibNO2status->hMib);
    /* Debug */
    DBG_vPrintf(DEBUG_MIB_NO2_STATUS, "\n%seJIP_RegisterMib(NO2status)=%d", acDebugIndent, eStatus);
    Node_vDebugOutdent(DEBUG_MIB_NO2_STATUS);
}

/****************************************************************************
 *
 * NAME: MibCOStatus_vTick
 *
 * DESCRIPTION:
 * Timer function
 *
 ****************************************************************************/
PUBLIC void MibNO2status_vAnalogue(uint8 u8Adc)
{
	/* ADC 2 */
	if (u8Adc == psMibNO2status->u8AdcSrcBusVolts)
	{
		/* Pass on to driver and note the returned voltage */
		psMibNO2status->sPerm.i16Volts = DriverOXsensor_i16Analogue(u8Adc, MibAdcStatus_u16Read(u8Adc));
		PDM_vSaveRecord(&psMibNO2status->sDesc);
	}
	else {;}
}

/****************************************************************************
 *
 * NAME: MibCOStatus_vAppTimer5min
 *
 * DESCRIPTION:
 * Timer function
 *
 ****************************************************************************/
PUBLIC void MibNO2status_vAppTimer(uint32 u32TimerSeconds)
{
	if ((Node_bCalibrate() == 1) && (u32TimerSeconds == 1) )
	{
		float resvalue = (float)(((VAL_SENSOR_ALIM * VAL_OX_SENSOR_RGND) / (float)psMibNO2status->sPerm.i16Volts) - OX_RL_VALUE - VAL_OX_SENSOR_RGND);
		//DBG_vPrintf(TRUE,"\nVread_NO2=%d",psMibNO2status->sPerm.i16Volts);
	    DBG_vPrintf(TRUE,"\nresvalue_NO2=%d",(uint32)resvalue);
	    psMibNO2status->sPerm.Ro = DriverOXsensor_getro(resvalue);
		DBG_vPrintf(TRUE,"\nRo_NO2=%d",(uint32)psMibNO2status->sPerm.Ro);
		PDM_vSaveRecord(&psMibNO2status->sDesc);
	}
	else
	{
		float resvalue = (float)(((VAL_SENSOR_ALIM * VAL_OX_SENSOR_RGND) / (float)psMibNO2status->sPerm.i16Volts) - OX_RL_VALUE - VAL_OX_SENSOR_RGND);
		DBG_vPrintf(TRUE,"\nresvalue_NO2=%d",(uint32)resvalue);
		//DBG_vPrintf(TRUE,"\nVread_NO2=%d",psMibNO2status->sPerm.i16Volts);
		//DBG_vPrintf(TRUE,"\nRo_NO2=%d",(uint32)psMibNO2status->sPerm.Ro);
		psMibNO2status->sPerm.NO2ppb = DriverOXsensor_getppb(resvalue,psMibNO2status->sPerm.Ro);
		float ppb = psMibNO2status->sPerm.NO2ppb *1000;
		DBG_vPrintf(TRUE,"\nNO2ppb=%d",(uint32)ppb);
		PDM_vSaveRecord(&psMibNO2status->sDesc);
		/* Valid result ? */
//		if ((psMibNO2status->sPerm.NO2ppb >=psMibCOstatus->sTemp.COMin) && (psMibNO2status->sPerm.NO2ppb <= psMibNO2status->sTemp.NO2Max))
//		{
//			if (psMibNO2status->sPerm.NO2ppb < OX_SENSOR_DEFAULT_PPM)
//			psMibCOstatus->sPerm.TargetStatus = VAR_VAL_PPB_STATUS_TARGET_STATUS_OK;
//			else if (psMibNO2status->sPerm.NO2ppb>VAL_CAUTION_PPM)
//			psMibCOstatus->sPerm.TargetStatus = VAR_VAL_PPB_STATUS_TARGET_STATUS_CAUTION;
//			else if (psMibNO2status->sPerm.NO2ppb>VAL_CAUTION_PPM)
//			psMibCOstatus->sPerm.TargetStatus = VAR_VAL_PPB_STATUS_TARGET_STATUS_WARNING;
//			else if (psMibNO2status->sPerm.NO2ppb>VAL_CAUTION_PPM)
//			psMibCOstatus->sPerm.TargetStatus = VAR_VAL_PPB_STATUS_TARGET_STATUS_THREAT0;
//			else if (psMibNO2status->sPerm.NO2ppb>VAL_CAUTION_PPM)
//			psMibCOstatus->sPerm.TargetStatus = VAR_VAL_PPB_STATUS_TARGET_STATUS_THREAT1;
//			else if (psMibNO2status->sPerm.NO2ppb>VAL_CAUTION_PPM)
//			psMibCOstatus->sPerm.TargetStatus = VAR_VAL_PPB_STATUS_TARGET_STATUS_THREAT2;
//		    else if (psMibNO2status->sPerm.NO2ppb>VAL_CAUTION_PPM)
//			 psMibCOstatus->sPerm.TargetStatus = VAR_VAL_PPB_STATUS_TARGET_STATUS_THREAT3;
//	      }
	   }
	}
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/


