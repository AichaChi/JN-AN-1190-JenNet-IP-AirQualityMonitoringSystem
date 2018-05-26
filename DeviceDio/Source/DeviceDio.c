/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         DeviceDio - Main Router Source File
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
/* Application includes */
#include "Config.h"
#include "DeviceDefs.h"
#include "Exception.h"
#include "Security.h"
#include "Node.h"
#include "MibAdcStatus.h"
#include "MibDio.h"
#include "MibDioStatus.h"
#include "MibDioConfig.h"
#include "MibDioControl.h"
//#include "Uart.h"

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
#define DEVICE_ADC_MASK 		(MIB_ADC_MASK_SRC_VOLT|MIB_ADC_MASK_SRC_TEMP) /* ADCs to be read */
#define DEVICE_ADC_SRC_BUS_VOLTS E_AHI_ADC_SRC_VOLT
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
/* MIB structures */
#if MK_BLD_MIB_DIO_STATUS
extern tsMibDioStatus	 sMibDioStatus;
extern thJIP_Mib		 hMibDioStatus;
#endif

#if MK_BLD_MIB_DIO_CONFIG
extern tsMibDioConfig	 sMibDioConfig;
extern thJIP_Mib		 hMibDioConfig;
#endif

#if MK_BLD_MIB_DIO_CONTROL
extern tsMibDioControl	 sMibDioControl;
extern thJIP_Mib		 hMibDioControl;
#endif

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

	/* Turn on application LED */
	//vAHI_DioSetDirection(0, DEVICE_OUTPUT_MASK_APP);
	//vAHI_DioSetPullup(0, DEVICE_OUTPUT_MASK_APP);
	//vAHI_DioSetOutput(DEVICE_OUTPUT_MASK_APP, 0);

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
	DBG_vPrintf(DEBUG_DEVICE_FUNC, "\n\nDEVICE DIO");
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
	Node_vPdmInit(DEVICE_ADC_MASK, DEVICE_ADC_PERIOD);

	/* Initialise Dio MIBs */
	#if MK_BLD_MIB_DIO_CONFIG
		MibDioConfig_vInit(hMibDioConfig, &sMibDioConfig);
	#endif
	#if MK_BLD_MIB_DIO_STATUS
		MibDioStatus_vInit(hMibDioStatus, &sMibDioStatus);
	#endif
	#if MK_BLD_MIB_DIO_CONTROL
		MibDioControl_vInit(hMibDioControl, &sMibDioControl);
	#endif

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

	/* FactoryReset ? */
	if (bFactoryReset)
	{
		#if MK_BLD_MIB_DIO_CONFIG
			PDM_vDeleteRecord(&sMibDioConfig.sDesc);
		#endif
		#if MK_BLD_MIB_DIO_CONTROL
			PDM_vDeleteRecord(&sMibDioControl.sDesc);
		#endif
	}

	/* Perform node level reset */
	Node_vReset(bFactoryReset);

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

	/* Register Dio MIBs */
	#if MK_REG_MIB_DIO_STATUS
		MibDioStatus_vRegister();
	#endif
	#if MK_REG_MIB_DIO_CONFIG
		MibDioConfig_vRegister();
	#endif
	#if MK_REG_MIB_DIO_CONTROL
		MibDioControl_vRegister();
	#endif

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
			/* Pass on to DioStatus module */
			MibDioStatus_vSysCtrlEvent(u32Device, u32ItemBitmap);
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
			/* Perform node handling (which includes obtaining the ADC source that completed) */
			(void) Node_u8AnalogueEvent();
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
	/* Pass tick timer on to application MIBs here */
	MibDioStatus_vTick();
	MibDioConfig_vTick();
	MibDioControl_vTick();
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
	/* Pass 100ms application timer on to application MIBs here */
	MibDioStatus_vAppTimer100ms();
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
	;
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
