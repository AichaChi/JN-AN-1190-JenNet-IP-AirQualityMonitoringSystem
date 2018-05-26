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
#include "MibCOstatus.h"
#include "DriverREDsensor.h"
#include "DeviceDefs.h"
#include "MibSensors.h"
#include "MibAdcStatus.h"
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
/* Data access */
#define VAL_CAUTION_PPM           8
#define VAL_WARNING_PPM           35
#define VAL_THREAT0_PPM           400
#define VAL_THREAT1_PPM           800
#define VAL_THREAT2_PPM           1600
#define VAL_THREAT3_PPM           12,800

#define DEBUG_MIB_CO_STATUS       FALSE

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
PRIVATE tsMibCOstatus *psMibCOstatus;
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

PUBLIC void MibCOstatus_vInit(thJIP_Mib     hMibCOstatusInit,
                             tsMibCOstatus *psMibCOstatusInit,
                              uint8  	   u8AdcSrcBusVoltsInit)
{
    /* Debug */
    DBG_vPrintf(DEBUG_MIB_CO_STATUS, "\n%sMibCOstatus_vInit() {%d}", acDebugIndent, sizeof(tsMibCOstatus));

    /* Valid data pointer ? */
    if (psMibCOstatusInit != (tsMibCOstatus *) NULL)
    {
        /* Take copy of pointer to data */
        psMibCOstatus = psMibCOstatusInit;
        /* Take a copy of the MIB handle */
        psMibCOstatus->hMib = hMibCOstatusInit;
        /* Load Dio mib data */
         (void) PDM_eLoadRecord(&psMibCOstatus->sDesc,
     							   (uint16)(MIB_ID_CO_STATUS & 0xFFFF),
                                    (void *) &psMibCOstatus->sPerm,
                                    sizeof(psMibCOstatus->sPerm),
                                    FALSE);
        psMibCOstatus->sTemp.COtarget  = VAL_CO_TARGET;
        psMibCOstatus->sTemp.COMin 	  = RED_SENSOR_MAX_CO;
        psMibCOstatus->sTemp.COMax 	  = RED_SENSOR_MAX_CO;
		psMibCOstatus->u8AdcSrcBusVolts = u8AdcSrcBusVoltsInit;
		psMibCOstatus->sPerm.Ro = RED_SENSOR_DEFAULT_RO;

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
PUBLIC void MibCOstatus_vRegister(void)
{
    teJIP_Status eStatus;
    /* Debug */
    DBG_vPrintf(DEBUG_MIB_CO_STATUS, "\n%sMibCOstatus_vRegister()", acDebugIndent);
    Node_vDebugIndent(DEBUG_MIB_CO_STATUS);

    /* Register MIB */
    eStatus = eJIP_RegisterMib(psMibCOstatus->hMib);
    /* Debug */
    DBG_vPrintf(DEBUG_MIB_CO_STATUS, "\n%seJIP_RegisterMib(COstatus)=%d", acDebugIndent, eStatus);
    Node_vDebugOutdent(DEBUG_MIB_CO_STATUS);
}

/****************************************************************************
 *
 * NAME: MibCOStatus_vTick
 *
 * DESCRIPTION:
 * Timer function
 *
 ****************************************************************************/
//PUBLIC void MibCOstatus_vTick(void)
//{
//	/* Are there any variable notifications pending ? */
//	 if ((psMibCOstatus->u32NotifyChanged & VAR_MASK_CO_STATUS) != 0)
//	{
//		/* Use common function to output notifications */
//		Node_vJipNotifyChanged(psMibCOstatus->hMib, &psMibCOstatus->u32NotifyChanged, VAR_MASK_CO_STATUS, VAR_COUNT_CO_STATUS);
//	}
//}

PUBLIC void MibCOstatus_vAnalogue(uint8 u8Adc)
{
	/* ADC 2 */
	if (u8Adc == psMibCOstatus->u8AdcSrcBusVolts)
	{
		/* Pass on to driver and note the returned voltage */
		psMibCOstatus->sPerm.i16Volts = DriverREDsensor_i16Analogue(u8Adc, MibAdcStatus_u16Read(u8Adc));
		PDM_vSaveRecord(&psMibCOstatus->sDesc);
		//DBG_vPrintf(TRUE,"\nVread=%d",psMibCOstatus->sPerm.i16Volts);
	}
}

PUBLIC teJIP_Status MibCOstatus_eSetMode(uint8 u8Val, void *pvCbData)
{
	teJIP_Status eReturn;

	/* Call standard function */
	eReturn = eSetUint8(u8Val, pvCbData);
    /* Make sure permament data is saved */
    PDM_vSaveRecord(&psMibCOstatus->sDesc);
    /* Debug */
  //  DBG_vPrintf(DEBUG_MIB_CO_STATUS, "\n%sMibNode_eSetMode(%d) = %d { PDM_vSaveRecord(COControl) = %d }", acDebugIndent, u8Val, eReturn, psMibCOstatus->sDesc.eState);

	return eReturn;
}
/****************************************************************************
 *
 * NAME: MibCOStatus_vAppTimer5min
 *
 * DESCRIPTION:
 * Timer function
 *
 ****************************************************************************/
PUBLIC void MibCOstatus_vAppTimer(uint32 u32TimerSeconds)
{
	if ((Node_bCalibrate() == 1) && (u32TimerSeconds == 1) )
	{
		float resvalue = (float)(((VAL_SENSOR_ALIM * VAL_RED_SENSOR_RGND) / (float)psMibCOstatus->sPerm.i16Volts) - RED_RL_VALUE - VAL_RED_SENSOR_RGND);
	    DBG_vPrintf(TRUE,"\nresvalue_CO=%d",(uint32)resvalue);
	//	DBG_vPrintf(TRUE,"\nVread_CO=%d",psMibCOstatus->sPerm.i16Volts);
		psMibCOstatus->sPerm.Ro = DriverREDsensor_getro(resvalue);
		DBG_vPrintf(TRUE,"\nRo_CO=%d",(uint32)psMibCOstatus->sPerm.Ro);
		PDM_vSaveRecord(&psMibCOstatus->sDesc);

	}
	else
	{
		float resvalue = (float)(((VAL_SENSOR_ALIM * VAL_RED_SENSOR_RGND) / (float)psMibCOstatus->sPerm.i16Volts) - RED_RL_VALUE - VAL_RED_SENSOR_RGND);
	//	DBG_vPrintf(TRUE,"\nVread_CO=%d",psMibCOstatus->sPerm.i16Volts);
		DBG_vPrintf(TRUE,"\nresvalue_CO=%d",(uint32)resvalue);
		psMibCOstatus->sPerm.COppm = DriverREDsensor_getppm(resvalue,psMibCOstatus->sPerm.Ro);
		float ppm = psMibCOstatus->sPerm.COppm * 1000;
		DBG_vPrintf(TRUE,"\nCOppm=%d",(uint32)ppm);
//		psMibCOstatus->u32NotifyChanged |= (1 << VAR_IX_CO_STATUS_PPM_CURRENT);
//		Node_vJipNotifyChanged(psMibCOstatus->hMib, &psMibCOstatus->u32NotifyChanged, VAR_MASK_CO_STATUS, VAR_COUNT_CO_STATUS);
		PDM_vSaveRecord(&psMibCOstatus->sDesc);
		/* Valid result ? */
//		if ((psMibCOstatus->sPerm.COppm >=psMibCOstatus->sTemp.COMin) && (psMibCOstatus->sPerm.COppm <= psMibCOstatus->sTemp.COMax))
//		{
//			if (psMibCOstatus->sPerm.COppm < RED_SENSOR_DEFAULT_PPM)
//			psMibCOstatus->sPerm.TargetStatus = VAR_VAL_PPM_STATUS_TARGET_STATUS_OK;
//			else if (psMibCOstatus->sPerm.COppm >VAL_CAUTION_PPM)
//			psMibCOstatus->sPerm.TargetStatus = VAR_VAL_PPM_STATUS_TARGET_STATUS_CAUTION;
//			else if (psMibCOstatus->sPerm.COppm >VAL_CAUTION_PPM)
//			psMibCOstatus->sPerm.TargetStatus = VAR_VAL_PPM_STATUS_TARGET_STATUS_WARNING;
//			else if (psMibCOstatus->sPerm.COppm >VAL_CAUTION_PPM)
//			psMibCOstatus->sPerm.TargetStatus = VAR_VAL_PPM_STATUS_TARGET_STATUS_THREAT0;
//			else if (psMibCOstatus->sPerm.COppm >VAL_CAUTION_PPM)
//			psMibCOstatus->sPerm.TargetStatus = VAR_VAL_PPM_STATUS_TARGET_STATUS_THREAT1;
//			else if (psMibCOstatus->sPerm.COppm >VAL_CAUTION_PPM)
//			psMibCOstatus->sPerm.TargetStatus = VAR_VAL_PPM_STATUS_TARGET_STATUS_THREAT2;
//		    else if (psMibCOstatus->sPerm.COppm >VAL_CAUTION_PPM)
//			 psMibCOstatus->sPerm.TargetStatus = VAR_VAL_PPM_STATUS_TARGET_STATUS_THREAT3;
//	      }
	   }
	}
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/


