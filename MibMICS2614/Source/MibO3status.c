/*
 * MibO3status.c

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
#include "MibO3status.h"
#include "DriverMICS2614.h"
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

#define DEBUG_MIB_O3_STATUS      FALSE

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
PRIVATE tsMibO3status *psMibO3status;
/****************************************************************************/
/***        External variables                                            ***/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: MibO3Status_vInit
 *
 * DESCRIPTION:
 * Initialises data
 *
 ****************************************************************************/

PUBLIC void MibO3status_vInit(thJIP_Mib     hMibO3statusInit,
                             tsMibO3status *psMibO3statusInit,
                              uint8  	   u8AdcSrcBusVoltsInit)
{
    /* Debug */
    DBG_vPrintf(DEBUG_MIB_O3_STATUS, "\n%sMibO3status_vInit() {%d}", acDebugIndent, sizeof(tsMibO3status));

    /* Valid data pointer ? */
    if (psMibO3statusInit != (tsMibO3status *) NULL)
    {
        /* Take copy of pointer to data */
        psMibO3status = psMibO3statusInit;
        /* Take a copy of the MIB handle */
        psMibO3status->hMib = hMibO3statusInit;
        /* Load Dio mib data */
         (void) PDM_eLoadRecord(&psMibO3status->sDesc,
     							   (uint16)(MIB_ID_O3_STATUS & 0xFFFF),
                                    (void *) &psMibO3status->sPerm,
                                    sizeof(psMibO3status->sPerm),
                                    FALSE);

        psMibO3status->sTemp.O3target  = VAL_O3_TARGET;
        psMibO3status->sTemp.O3Min     = MICS2614_SENSOR_MIN_PPB;
        psMibO3status->sTemp.O3Max     = MICS2614_SENSOR_MAX_PPB ;
		psMibO3status->u8AdcSrcBusVolts = u8AdcSrcBusVoltsInit;
		psMibO3status->sPerm.Ro     = MICS2614_SENSOR_DEFAULT_RO ;

    }
}

/****************************************************************************
 *
 * NAME: MibO3Status_vRegister
 *
 * DESCRIPTION:
 * Registers MIB
 *
 ****************************************************************************/
PUBLIC void MibO3status_vRegister(void)
{
    teJIP_Status eStatus;
    /* Debug */
    DBG_vPrintf(DEBUG_MIB_O3_STATUS, "\n%sMibO3status_vRegister()", acDebugIndent);
    Node_vDebugIndent(DEBUG_MIB_O3_STATUS);

    /* Register MIB */
    eStatus = eJIP_RegisterMib(psMibO3status->hMib);
    /* Debug */
    DBG_vPrintf(DEBUG_MIB_O3_STATUS, "\n%seJIP_RegisterMib(O3status)=%d", acDebugIndent, eStatus);
    Node_vDebugOutdent(DEBUG_MIB_O3_STATUS);
}

/****************************************************************************
 *
 * NAME: MibCOStatus_vTick
 *
 * DESCRIPTION:
 * Timer function
 *
 ****************************************************************************/
PUBLIC void MibO3status_vAnalogue(uint8 u8Adc)
{
	/* ADC 2 */
	if (u8Adc == psMibO3status->u8AdcSrcBusVolts)
	{
		/* Pass on to driver and note the returned voltage */
		psMibO3status->sPerm.i16Volts = DriverMICS2614_i16Analogue(u8Adc, MibAdcStatus_u16Read(u8Adc));
		PDM_vSaveRecord(&psMibO3status->sDesc);
		//DBG_vPrintf(TRUE,"\nVread_O3=%d",psMibO3status->sPerm.i16Volts);
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
PUBLIC void MibO3status_vAppTimer(uint32 u32TimerSeconds)
{
	if ((Node_bCalibrate() == 1) && (u32TimerSeconds == 1) )
	{
		float resvalue = (float)(((VAL_SENSOR_ALIM * VAL_MICS2614_SENSOR_RGND) / (float)psMibO3status->sPerm.i16Volts) - MICS2614_RL_VALUE - VAL_MICS2614_SENSOR_RGND);
		DBG_vPrintf(TRUE,"\nVread_O3=%d",(uint32)psMibO3status->sPerm.i16Volts);
	//	DBG_vPrintf(TRUE,"\nresvalue_O3=%d",(uint32)resvalue);
		psMibO3status->sPerm.Ro = DriverMICS2614_getro(resvalue);
		DBG_vPrintf(TRUE,"\nRo_O3=%d",(uint32)psMibO3status->sPerm.Ro);
		PDM_vSaveRecord(&psMibO3status->sDesc);
	}
	else
	{
		float resvalue = (float)(((VAL_SENSOR_ALIM * VAL_MICS2614_SENSOR_RGND) / (float)psMibO3status->sPerm.i16Volts) - MICS2614_RL_VALUE - VAL_MICS2614_SENSOR_RGND);
		DBG_vPrintf(TRUE,"\nresvalue_O3=%d",(uint32)resvalue);
	//	DBG_vPrintf(TRUE,"\nVread_O3=%d",(uint32)psMibO3status->sPerm.i16Volts);
	//	DBG_vPrintf(TRUE,"\nRo_O3=%d",(uint32)psMibO3status->sPerm.Ro);
		psMibO3status->sPerm.O3ppb = DriverMICS2614_getppb(resvalue,psMibO3status->sPerm.Ro);
		float ppb = psMibO3status->sPerm.O3ppb*1000;
        DBG_vPrintf(TRUE,"\nO3ppb=%d",(uint32)ppb);
		PDM_vSaveRecord(&psMibO3status->sDesc);
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


