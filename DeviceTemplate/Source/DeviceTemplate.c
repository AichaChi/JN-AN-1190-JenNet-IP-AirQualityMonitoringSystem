/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         DeviceTemplate - Main Template Source File
 */
/****************************************************************************/
/*
 * This software is owned by NXP B.V. and/or its supplier and is protected
 * under applicable copyright laws. All rights are reserved. We grant You,
 * and any third parties, a license to use this software solely and
 * exclusively on NXP products [NXP Microcontrollers such as JN5168, JN5164].
 * You, and any third parties must reproduce the copyright and warranty notice
 * and any other legend of ownership on each copy or partial copy of the
 * software.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Copyright NXP B.V. 2014. All rights reserved
 */
/****************************************************************************/

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
#include <OverNetworkDownload.h>
#include <sec2006.h>
/* JenOS includes */
#include <dbg.h>
#include <dbg_uart.h>
#include <os.h>
#include <pdm.h>
#include <math.h>
/* Application includes */
#include "Config.h"
#include "DeviceDefs.h"
#include "Exception.h"
#include "Security.h"
#include "Node.h"
#include "MibAdcStatus.h"
#include "MibCommon.h"
//#include "Uart.h"
/*** Aicha  ***/
#include "MibCOstatus.h"
#include "MibNO2status.h"
#include "MibO3status.h"
#include "MibH2Sstatus.h"
#include "DriverREDsensor.h"
/**************/

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
/* Check for unsupported chip type */
#ifdef  JENNIC_CHIP_FAMILY_JN514x
#error Unsupported chip!
#endif
#ifdef  JENNIC_CHIP_FAMILY_JN516x
#ifndef JENNIC_CHIP_JN5164
#ifndef JENNIC_CHIP_JN5168
#error Unsupported chip!
#endif
#endif
#endif

/* ADC control */
/***   Aicha   ***/
#if MK_BLD_MIB_H2S_STATUS
#define DEVICE_ADC_MASK    MIB_ADC_MASK_SRC_2|MIB_ADC_MASK_SRC_1|MIB_ADC_MASK_SRC_3|MIB_ADC_MASK_SRC_4/* ADCs to be read */
#else
#define DEVICE_ADC_MASK    MIB_ADC_MASK_SRC_2|MIB_ADC_MASK_SRC_1|MIB_ADC_MASK_SRC_3
#endif

#define DEVICE_ADC_SRC_RED_SENSOR  E_AHI_ADC_SRC_ADC_2
#define DEVICE_ADC_SRC_OX_SENSOR   E_AHI_ADC_SRC_ADC_1
#define DEVICE_ADC_SRC_O3_SENSOR   E_AHI_ADC_SRC_ADC_3
#define DEVICE_ADC_SRC_MQ_SENSOR   E_AHI_ADC_SRC_ADC_4
/******************/
#define DEVICE_ADC_PERIOD 		 25											  /* ADC sample period 0-100 in 10ms intervals */
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
/***        External Variables                                            ***/
/****************************************************************************/
/*** Aicha  ***/
#if MK_BLD_MIB_CO_STATUS
extern tsMibCOstatus	 	sMibCOstatus;
extern thJIP_Mib		 	hMibCOstatus;
#endif

#if MK_BLD_MIB_NO2_STATUS
extern tsMibNO2status	 	sMibNO2status;
extern thJIP_Mib		 	hMibNO2status;
#endif

#if MK_BLD_MIB_O3_STATUS
extern tsMibO3status	 	sMibO3status;
extern thJIP_Mib		 	hMibO3status;
#endif

#if MK_BLD_MIB_H2S_STATUS
extern tsMibH2Sstatus	 	sMibH2Sstatus;
extern thJIP_Mib		 	hMibH2Sstatus;
#endif
/*************/
/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
/* Other data */
PRIVATE bool_t   bSleep;
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
PUBLIC void 			Device_vInit(bool_t bWarmStart);
PUBLIC void 			Device_vPdmInit(void);
PUBLIC void 			Device_vReset(bool_t bFactoryReset);
PUBLIC teJIP_Status 	Device_eJipInit(void);
PUBLIC void 			Device_vMain(void);
PUBLIC void 			Device_vTick(void);
PUBLIC void 			Device_vAppTimer100ms(void);
PUBLIC void 			Device_vSecond(uint32 u32TimerSeconds);
#ifdef MK_BLD_NODE_TYPE_END_DEVICE
PUBLIC void 			Device_vSleep(void);
PUBLIC void 			Device_vPreSleepCallback(void);
#endif

/****************************************************************************
 *
 * NAME: AppColdStart
 *
 * DESCRIPTION:
 * Entry point for application from boot loader.
 *
 * RETURNS:
 * void, never returns
 *
 ****************************************************************************/
PUBLIC void AppColdStart(void)
{
	/* UART ? */
	#ifdef UART_H_INCLUDED
	{
		/* Wait for clock to stablise */
		while(bAHI_Clock32MHzStable() == FALSE);
		/* Initialise UART */
		UART_vInit();
		UART_vChar('c');
	}
	#endif
	/* Initialise device */
	Device_vInit(FALSE);
}

/****************************************************************************
 *
 * NAME: AppWarmStart
 *
 * DESCRIPTION:
 * Entry point for application from boot loader.
 *
 * RETURNS:
 * Never returns.
 *
 ****************************************************************************/
PUBLIC void AppWarmStart(void)
{
	/* UART ? */
	#ifdef UART_H_INCLUDED
	{
		/* Wait for clock to stablise */
		while(bAHI_Clock32MHzStable() == FALSE);
		/* Initialise UART */
		UART_vInit();
		UART_vChar('w');
	}
	#endif

	/* Turn on application LED */
	//vAHI_DioSetOutput(DEVICE_OUTPUT_MASK_APP, 0);

	/* Initialise device */
	Device_vInit(TRUE);
}

/****************************************************************************
 *
 * NAME: Device_vInit
 *
 * DESCRIPTION:
 * Entry point for application
 *
 * RETURNS:
 * void, never returns
 *
 ****************************************************************************/
PUBLIC void Device_vInit(bool_t bWarmStart)
{
	bool_t   				  bFactoryReset;

	/* UART ? */
	#ifdef UART_H_INCLUDED
	{
		UART_vChar('i');
	}
	#endif

	/* Node initialisation */
	Node_vInit(bWarmStart);

	/* Debug */
	DBG_vPrintf(DEBUG_DEVICE_FUNC, "                ");
	DBG_vPrintf(DEBUG_DEVICE_FUNC, "\n\nDEVICE TEMPLATE");
	DBG_vPrintf(DEBUG_DEVICE_FUNC, "\n%sDevice_vInit(%d)", acDebugIndent, bWarmStart);
	Node_vDebugIndent(DEBUG_DEVICE_FUNC);
	DBG_vPrintf(DEBUG_NODE_FUNC,   "\n%sNode_vInit(%d)", acDebugIndent, bWarmStart);

	/* UART ? */
	#ifdef UART_H_INCLUDED
	{
		/* Wait for clock to stablise */
		while(bAHI_Clock32MHzStable() == FALSE);
		/* Initialise UART */
		UART_vInit();
	}
	#endif

	/* Cold start ? */
	if (FALSE == bWarmStart)
	{
		/* Check for factory reset using flags from EEPROM */
		bFactoryReset = Node_bTestFactoryResetEeprom();

		/* Initialise PDM and MIB data */
		Device_vPdmInit();
		/***  Aicha  ***/
	    uint32 u32Buttons;
	    uint8 u8Calib = 0 ;
		/* Read the input */
        #if MK_BLD_PREH_STATUS
		{
	    	DBG_vPrintf(TRUE,"\nPREHEATING");
//	    	vAHI_DioSetDirection(0, MICS4514_OUTPUT_MASK);
//		    vAHI_DioSetOutput(MICS4514_OUTPUT_MASK,0);         //PREH=1
//		    vAHI_DioSetDirection(0, MICS2614_OUTPUT_MASK);
//		    vAHI_DioSetOutput(MICS2614_OUTPUT_MASK,0);         //VO3_EN=1
		    #if MK_BLD_MIB_H2S_STATUS
		       {
			      vAHI_DioSetDirection(0, MQ136_OUTPUT_MASK);
		          vAHI_DioSetOutput(MQ136_OUTPUT_MASK,0);      //MQ_EN=1
		       }
		     #endif
		}
       #endif
		u32Buttons = u32AHI_DioReadInput() & DIO_BUTTON_MASK;
		/* Button pressed ? */
	    if ((u32Buttons & DIO_BUTTON_MASK) == 0)
		  {
	    	 DBG_vPrintf(TRUE,"\n CALIBRATION");
		     u8Calib = 1;
		     Node_vCheckCalib(u8Calib);
		  }
	    /***************/
		/* Apply factory reset if required */
		if (bFactoryReset) Device_vReset(TRUE);

		/* Initialise JIP */
		(void) Device_eJipInit();
	}

	/* Enter main loop */
	Device_vMain();

	/* Allow sleeping ? */
	#ifdef MK_BLD_NODE_TYPE_END_DEVICE
	{
		/* Go to sleep if we exit main loop */
		Device_vSleep();
	}
	#endif

	/* Debug */
	Node_vDebugOutdent(DEBUG_DEVICE_FUNC);
}

/****************************************************************************
 *
 * NAME: Device_vPdmInit
 *
 * DESCRIPTION:
 * Initialise PDM MIB data
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void Device_vPdmInit(void)
{
	/* Debug */
	DBG_vPrintf(DEBUG_DEVICE_FUNC, "\n%sDevice_vPdmInit()", acDebugIndent);
	Node_vDebugIndent(DEBUG_DEVICE_FUNC);

	/* Initialise PDM and Node MIBs*/
	Node_vPdmInit(DEVICE_ADC_MASK,DEVICE_ADC_PERIOD);
    /***  Aicha  ***/
	/* Initialise Node MIBs */
		#if MK_BLD_MIB_CO_STATUS
		   MibCOstatus_vInit(hMibCOstatus, &sMibCOstatus,DEVICE_ADC_SRC_RED_SENSOR);
		#endif
        #if MK_BLD_MIB_NO2_STATUS
           MibNO2status_vInit(hMibNO2status, &sMibNO2status,DEVICE_ADC_SRC_OX_SENSOR);
        #endif
        #if MK_BLD_MIB_O3_STATUS
           MibO3status_vInit(hMibO3status, &sMibO3status,DEVICE_ADC_SRC_O3_SENSOR);
        #endif
        #if MK_BLD_MIB_H2S_STATUS
           MibH2Sstatus_vInit(hMibH2Sstatus, &sMibH2Sstatus,DEVICE_ADC_SRC_MQ_SENSOR);
        #endif
     /*************/
	/* Debug */
	Node_vDebugOutdent(DEBUG_DEVICE_FUNC);
}

/****************************************************************************
 *
 * NAME: Device_vReset
 *
 * DESCRIPTION:
 * Reset device
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void Device_vReset(bool_t bFactoryReset)
{
	/* Debug */
	DBG_vPrintf(DEBUG_DEVICE_FUNC, "\n%sDevice_vReset(%d)", acDebugIndent, bFactoryReset);
	Node_vDebugIndent(DEBUG_DEVICE_FUNC);
	/* Perform node level reset */
	Node_vReset(bFactoryReset);
	/* FactoryReset ? */
	if (bFactoryReset)
	{
		/***  Aicha  ***/
		#if MK_BLD_MIB_CO_CONTROL
			PDM_vDeleteRecord(&sMibCOstatus.sDesc);
		#endif
        #if MK_BLD_MIB_O3_CONTROL
	        PDM_vDeleteRecord(&sMibO3status.sDesc);
        #endif
        #if MK_BLD_MIB_NO2_CONTROL
            PDM_vDeleteRecord(&sMibNO2status.sDesc);
        #endif
        #if MK_BLD_MIB_H2S_CONTROL
            PDM_vDeleteRecord(&sMibH2Sstatus.sDesc);
        #endif
        /**************/
	}

	/* Debug */
	Node_vDebugOutdent(DEBUG_DEVICE_FUNC);
}

/****************************************************************************
 *
 * NAME: Device_eJipInit
 *
 * DESCRIPTION:
 * Initialise JIP layer stack
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC teJIP_Status Device_eJipInit(void)
{
	teJIP_Status     eStatus;

	/* Debug */
	DBG_vPrintf(DEBUG_DEVICE_FUNC, "\n%sDevice_eJipInit()", acDebugIndent);
	Node_vDebugIndent(DEBUG_DEVICE_FUNC);

	/* Node JIP initialisation */
	eStatus = Node_eJipInit();
	/***  Aicha   ***/
	/* Register Node MIBs */
	#if MK_REG_MIB_CO_STATUS
		MibCOstatus_vRegister();
	#endif
	#if MK_REG_MIB_NO2_STATUS
		MibNO2status_vRegister();
	#endif
    #if MK_REG_MIB_O3_STATUS
	    MibO3status_vRegister();
    #endif
    #if MK_REG_MIB_H2S_STATUS
        MibH2Sstatus_vRegister();
    #endif
    /***************/
	/* Debug */
	Node_vDebugOutdent(DEBUG_DEVICE_FUNC);
	DBG_vPrintf(DEBUG_DEVICE_FUNC, "=%d", eStatus);

	return eStatus;
}

/****************************************************************************
 *
 * NAME: v6LP_ConfigureNetwork
 *
 * DESCRIPTION:
 * Configures network
 *
 ****************************************************************************/
PUBLIC void v6LP_ConfigureNetwork(tsNetworkConfigData *psNetworkConfigData)
{
	/* Debug */
	DBG_vPrintf(DEBUG_DEVICE_FUNC, "\n%sv6LP_ConfigureNetwork()", acDebugIndent);
	Node_vDebugIndent(DEBUG_DEVICE_FUNC);

	/* Call node configure network */
	Node_v6lpConfigureNetwork(psNetworkConfigData);

	/* Debug */
	Node_vDebugOutdent(DEBUG_DEVICE_FUNC);
}

/****************************************************************************
 *
 * NAME: Device_vMain
 *
 * DESCRIPTION:
 * Main device loop
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void Device_vMain(void)
{
	/* UART ? */
	#ifdef UART_H_INCLUDED
	{
		/* Initialise UART */
		UART_vChar('m');
	}
	#endif
	/* Debug */
	DBG_vPrintf(DEBUG_DEVICE_FUNC, "\n%sDevice_vMain()", acDebugIndent);
	Node_vDebugIndent(DEBUG_DEVICE_FUNC);

	/* Make sure sleep flag is clear */
	bSleep = FALSE;
	/* Set sleep output */
	vAHI_DioSetOutput(DEVICE_OUTPUT_MASK_SLEEP, 0);
	/* Main loop */
	while(FALSE == bSleep)
	{
		/* Restart watchdog */
	   	vAHI_WatchdogRestart();
	   	/* Call node main function */
	   	Node_vMain();
	   	/* Stack is not running ? */
	   	if (Node_u32StackState() == NODE_STACK_STATE_NONE)
	   	{
			/* Allow sleep */
			bSleep = TRUE;
			/* Clear sleep output */
			vAHI_DioSetOutput(0, DEVICE_OUTPUT_MASK_SLEEP);

		}
		/* Not ready to sleep ? */
		else if (FALSE == bSleep)
		{
			/* Doze */
			vAHI_CpuDoze();
		}
	}
	/* UART ? */
	#ifdef UART_H_INCLUDED
	{
		/* Initialise UART */
		UART_vChar('n');
	}
	#endif

	/* Debug */
	Node_vDebugOutdent(DEBUG_DEVICE_FUNC);
}

/****************************************************************************
 *
 * NAME: v6LP_DataEvent
 *
 * DESCRIPTION:
 * As this app uses JIP for all communication we are not interested in the
 * 6LP data events. For any receive events we simply discard the packet to
 * free the packet buffer.
 *
 * PARAMETERS: Name        RW  Usage
 *             iSocket     R   Socket on which packet received
 *             eEvent      R   Data event
 *             psAddr      R   Source address (for RX) or destination (for TX)
 *             u8AddrLen   R   Length of address
 *
 ****************************************************************************/
PUBLIC void v6LP_DataEvent(int iSocket, te6LP_DataEvent eEvent,
                           ts6LP_SockAddr *psAddr, uint8 u8AddrLen)
{
	/* Call node data event handler */
	Node_v6lpDataEvent(iSocket, eEvent, psAddr, u8AddrLen);
}

/****************************************************************************
 *
 * NAME: vJIP_StackEvent
 *
 * DESCRIPTION:
 * Processes any incoming stack events.
 * Once a join indication has been received, we initialise JIP and register
 * the various MIBs.
 *
 * PARAMETERS: Name          RW Usage
 *             eEvent        R  Stack event
 *             pu8Data       R  Additional information associated with event
 *             u8DataLen     R  Length of additional information
 *
 ****************************************************************************/
PUBLIC void vJIP_StackEvent(te6LP_StackEvent eEvent, void *pvData, uint8 u8DataLen)
{
	/* Debug */
	DBG_vPrintf(DEBUG_DEVICE_FUNC, "\n%svJIP_StackEvent(%d)", acDebugIndent, eEvent);
	Node_vDebugIndent(DEBUG_DEVICE_FUNC);

	/* Not End Device ? */
	#ifndef MK_BLD_NODE_TYPE_END_DEVICE
	{
		/* Node handling */
		(void) Node_bJipStackEvent(eEvent, pvData, u8DataLen);
	}
	/* End device ? */
	#else
	{
		bool_t bPollNoData;

		/* Node handling */
		bPollNoData = Node_bJipStackEvent(eEvent, pvData, u8DataLen);

		/* Did we get a poll response but no data ? */
		if (bPollNoData)
		{
			/* UART ? */
			#ifdef UART_H_INCLUDED
			{
				/* Initialise UART */
				UART_vChar('=');
			}
			#endif
			/* Set flag for sleep */
			bSleep = TRUE;
			/* Clear sleep output */
			vAHI_DioSetOutput(0, DEVICE_OUTPUT_MASK_SLEEP);
		}
	}
	#endif

	/* Debug */
	Node_vDebugOutdent(DEBUG_DEVICE_FUNC);
}

/****************************************************************************
 *
 * NAME: v6LP_PeripheralEvent
 *
 * DESCRIPTION:
 * Processes any incoming peripheral events. The end device is completely
 * event driven (the idle loop does nothing) and events here are simply used
 * to update the appropriate JIP module.
 *
 * PARAMETERS: Name          RW Usage
 *             u32Device     R  Device that caused peripheral event
 *             u32ItemBitmap R  Events within that peripheral
 *
 ****************************************************************************/
PUBLIC void v6LP_PeripheralEvent(uint32 u32Device, uint32 u32ItemBitmap)
{
	/* Which device generated the interrupt ? */
	switch (u32Device)
	{
		/* System control ? */
		case E_AHI_DEVICE_SYSCTRL:
		{
			/* UART ? */
			#ifdef UART_H_INCLUDED
			{
				if (u32ItemBitmap & E_AHI_SYSCTRL_WK0_MASK) UART_vChar('.');
				if (u32ItemBitmap & E_AHI_SYSCTRL_WK1_MASK) UART_vChar(',');
			}
			#endif

			/* Pass on to node module */
			Node_vSysCtrlEvent(u32Device, u32ItemBitmap);
		}
		break;

		/* Tick timer (run by stack at 10ms intervals) ? */
		case E_AHI_DEVICE_TICK_TIMER:
		{
			/* Pass on to node module */
			Node_vTickTimerEvent();
		}
		break;

		/* Analogue devices ? */
		case E_AHI_DEVICE_ANALOGUE:
		{
		    Node_u8AnalogueEvent();
		}
		break;
		/* Others ? */
		default:
		{
			/* Do nothing */
			;
		}
		break;
	}
}

/****************************************************************************
 *
 * NAME: Device_vTick
 *
 * DESCRIPTION:
 * Reset device
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void Device_vTick(void)
{

}

/****************************************************************************
 *
 * NAME: Device_vAppTimer100ms
 *
 * DESCRIPTION:
 * Reset device
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void Device_vAppTimer100ms(void)
{
;
}

/****************************************************************************
 *
 * NAME: Device_vSecond
 *
 * DESCRIPTION:
 * Reset device
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void Device_vSecond(uint32 u32TimerSeconds)
{
	/* Pass second timer onto application MIBs here */
#if MK_BLD_MIB_CO_STATUS
MibCOstatus_vAppTimer(u32TimerSeconds);
#endif

#if MK_BLD_MIB_O3_STATUS
MibO3status_vAppTimer(u32TimerSeconds);
#endif

#if MK_BLD_MIB_NO2_STATUS
MibNO2status_vAppTimer(u32TimerSeconds);
#endif

#if MK_BLD_MIB_H2S_STATUS
MibH2Sstatus_vAppTimer(u32TimerSeconds);
#endif
}

/****************************************************************************
 *
 * NAME: Device_vException
 *
 * DESCRIPTION:
 *
 ****************************************************************************/
PUBLIC WEAK void Device_vException(uint32 u32HeapAddr, uint32 u32Vector, uint32 u32Code)
{
	/* Reset */
	vAHI_SwReset();
}

#ifdef MK_BLD_NODE_TYPE_END_DEVICE
/****************************************************************************
 *
 * NAME: Device_vSleep
 *
 * DESCRIPTION:
 * Go to sleep
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void Device_vSleep(void)
{
	/* Debug */
	DBG_vPrintf(DEBUG_DEVICE_FUNC, "\n%sDevice_vSleep()", acDebugIndent);
	Node_vDebugIndent(DEBUG_DEVICE_FUNC);

	/* Tidy any application data, MIBs or devices up here prior to sleeping */
	;

	/* Get node to do common tidy up then go to sleep */
	Node_vSleep();
	/* Sleep loop */
	while(1)
	{
		/* UART ? */
		#ifdef UART_H_INCLUDED
		{
			/* Initialise UART */
			UART_vChar('!');
		}
		#endif
		/* Deal with device tick timer events ? */
		Device_vTick();
		/* Doze */
		vAHI_CpuDoze();
	}
}

/****************************************************************************
 *
 * NAME: Device_vPreSleepCallback
 *
 * DESCRIPTION:
 * Go to sleep
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void Device_vPreSleepCallback(void)
{
	/* Do nothing */
	;
}
#endif

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
