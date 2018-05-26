/*
 * MibH2Sstatus.c

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
#include "MibH2Sstatus.h"
#include "DriverMQ136.h"
#include "DeviceDefs.h"
#include "MibSensors.h"
#include "MibAdcStatus.h"
#include "DriverREDsensor.h"
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
/* Data access */
#define VAL_CAUTION_PPM           0
#define VAL_WARNING_PPM           0
#define VAL_THREAT0_PPM           0
#define VAL_THREAT1_PPM           0
#define VAL_THREAT2_PPM           0
#define VAL_THREAT3_PPM           0

#define DEBUG_MIB_H2S_STATUS     FALSE

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
PRIVATE tsMibH2Sstatus *psMibH2Sstatus;
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

PUBLIC void MibH2Sstatus_vInit(thJIP_Mib     hMibH2SstatusInit,
                             tsMibH2Sstatus *psMibH2SstatusInit,
                              uint8  	   u8AdcSrcBusVoltsInit)
{
    /* Debug */
    DBG_vPrintf(DEBUG_MIB_H2S_STATUS, "\n%sMibH2Sstatus_vInit() {%d}", acDebugIndent, sizeof(tsMibH2Sstatus));

    /* Valid data pointer ? */
    if (psMibH2SstatusInit != (tsMibH2Sstatus *) NULL)
    {
        /* Take copy of pointer to data */
        psMibH2Sstatus = psMibH2SstatusInit;
        /* Take a copy of the MIB handle */
        psMibH2Sstatus->hMib = hMibH2SstatusInit;
        /* Load Dio mib data */
         (void) PDM_eLoadRecord(&psMibH2Sstatus->sDesc,
     							   (uint16)(MIB_ID_H2S_STATUS & 0xFFFF),
                                    (void *) &psMibH2Sstatus->sPerm,
                                    sizeof(psMibH2Sstatus->sPerm),
                                    FALSE);

        psMibH2Sstatus->sTemp.H2Starget  = VAL_H2S_TARGET;
        psMibH2Sstatus->sTemp.H2SMin 	  = MQ136_SENSOR_MAX_PPM;
        psMibH2Sstatus->sTemp.H2SMax 	  = MQ136_SENSOR_MIN_PPM;
		psMibH2Sstatus->u8AdcSrcBusVolts = u8AdcSrcBusVoltsInit;
		psMibH2Sstatus->sPerm.Ro  = MQ136_SENSOR_DEFAULT_RO;

    }
}

/****************************************************************************
 *
 * NAME: MibH2SStatus_vRegister
 *
 * DESCRIPTION:
 * Registers MIB
 *
 ****************************************************************************/
PUBLIC void MibH2Sstatus_vRegister(void)
{
    teJIP_Status eStatus;
    /* Debug */
    DBG_vPrintf(DEBUG_MIB_H2S_STATUS, "\n%sMibH2Sstatus_vRegister()", acDebugIndent);
    Node_vDebugIndent(DEBUG_MIB_H2S_STATUS);

    /* Register MIB */
    eStatus = eJIP_RegisterMib(psMibH2Sstatus->hMib);
    /* Debug */
    DBG_vPrintf(DEBUG_MIB_H2S_STATUS, "\n%seJIP_RegisterMib(H2Sstatus)=%d", acDebugIndent, eStatus);
    Node_vDebugOutdent(DEBUG_MIB_H2S_STATUS);
}

/****************************************************************************
 *
 * NAME: MibCOStatus_vTick
 *
 * DESCRIPTION:
 * Timer function
 *
 ****************************************************************************/
PUBLIC void MibH2Sstatus_vAnalogue(uint8 u8Adc)
{
	/* ADC 2 */
	if (u8Adc == psMibH2Sstatus->u8AdcSrcBusVolts)
	{
		/* Pass on to driver and note the returned voltage */
		psMibH2Sstatus->sPerm.i16Volts = DriverMQ136_i16Analogue(u8Adc, MibAdcStatus_u16Read(u8Adc));
		PDM_vSaveRecord(&psMibH2Sstatus->sDesc);
	}
}

/****************************************************************************
 *
 * NAME: MibH2SStatus_vAppTimer
 *
 * DESCRIPTION:
 * Timer function
 *
 ****************************************************************************/
PUBLIC void MibH2Sstatus_vAppTimer(uint32 u32TimerSeconds)
{
	if ((Node_u8SleepPeriods() == 192) && (Node_bCalibrate() == 1))
	{
		float resvalue = (float)((( VAL_MQ136_SENSOR_RGND * VAL_SENSOR_ALIM  ) / (float)psMibH2Sstatus->sPerm.i16Volts) - VAL_MQ136_SENSOR_RGND );
		//DBG_vPrintf(TRUE,"\nVread_H2S=%d",psMibH2Sstatus->sPerm.i16Volts);
		DBG_vPrintf(TRUE,"\nresvalue_H2S=%d",(uint32)resvalue);
		psMibH2Sstatus->sPerm.Ro = DriverMQ136_getro(resvalue);
		DBG_vPrintf(TRUE,"\nRo_H2S=%d",(uint32)psMibH2Sstatus->sPerm.H2Sppm);
		PDM_vSaveRecord(&psMibH2Sstatus->sDesc);
	}
	else  if (Node_u8SleepPeriods() > 192)
	{
		float resvalue = (float)(((VAL_SENSOR_ALIM * VAL_MQ136_SENSOR_RGND) / (float)psMibH2Sstatus->sPerm.i16Volts) - VAL_MQ136_SENSOR_RGND );
		DBG_vPrintf(TRUE,"\nresvalue_H2S=%d",(uint32)resvalue);
	//  DBG_vPrintf(TRUE,"\nVread_H2S=%d",psMibH2Sstatus->sPerm.i16Volts);
		//DBG_vPrintf(TRUE,"\nRo_H2S=%d",(uint32)psMibH2Sstatus->sPerm.Ro);
		psMibH2Sstatus->sPerm.H2Sppm =  DriverMQ136_getppm(resvalue,psMibH2Sstatus->sPerm.Ro);
		float ppm = psMibH2Sstatus->sPerm.H2Sppm *1000;
		DBG_vPrintf(TRUE,"\nH2Sppm=%d",(uint32)ppm);
		PDM_vSaveRecord(&psMibH2Sstatus->sDesc);
		/* Valid result ? */
//		if ((psMibH2Sstatus->sPerm.H2Sppm >=psMibH2Sstatus->sTemp.H2SMin) && (psMibH2Sstatus->sPerm.H2Sppm <= psMibH2Sstatus->sTemp.H2SMax))
//		{
//		/*add trap notification for H2Sppm if needed*/
//	    }
	}
}
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/


