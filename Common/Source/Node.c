/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         Node - common functions
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
#include <jip_data.h>
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
#include "MibCommon.h"
#include "MibNode.h"
#include "MibGroup.h"
#include "MibAdcStatus.h"
#include "MibNodeStatus.h"
#include "MibNodeControl.h"
#include "MibNwkStatus.h"
#include "MibNwkSecurity.h"
#include "MibNwkTest.h"
#include "MibNodeStatus.h"
#include "MibCOstatus.h"
#include "MibSensors.h"
#include "DriverREDsensor.h"
#include "DeviceDefs.h"
#include <math.h>
/* Optional Application Includes */
#ifdef  JENNIC_CHIP_FAMILY_JN516x
#include "AHI_EEPROM.h" /* Enables EEPROM factory reset detection, for 6x use only */
#endif
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

/* Factory reset magic number */
#ifdef MK_FACTORY_RESET_MAGIC
#define FACTORY_RESET_MAGIC 		MK_FACTORY_RESET_MAGIC
#else
#define FACTORY_RESET_MAGIC 		0xFA5E13CB
#endif
#define FACTORY_RESET_TICK_TIMER	32000000	/* 16Mhz - 2 seconds */

/* End Device build ? */
#ifdef MK_BLD_NODE_TYPE_END_DEVICE
/* Have to use wake timer 1 to drive 100ms timer */
#define NODE_WAKE_TIMER_100MS TRUE
/* Coordinator or router build ? */
#else
/* Use tick timer to drive 100ms timer (can also use wake timer if preferred) */
#define NODE_WAKE_TIMER_100MS FALSE
#endif
/* Optionally define a module type */
//#define NODE_MODULE_TYPE E_MODULE_HPM05

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/
PUBLIC int32  i32DebugIndent;
PUBLIC char    acDebugIndent[65];

/****************************************************************************/
/***        External Function Prototypes                                  ***/
/****************************************************************************/
extern PUBLIC void Device_vTick(void);
extern PUBLIC void Device_vAppTimer100ms(void);
extern PUBLIC void Device_vPreSleepCallback(void);
extern PUBLIC void Device_vSecond(uint32);
/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/
/* MIB structures */
#if MK_BLD_MIB_NODE
extern tsMibNode		 sMibNode;
#endif

#if MK_BLD_MIB_GROUP
extern tsMibGroup		 sMibGroup;
#endif

#if MK_BLD_MIB_NODE_STATUS
extern tsMibNodeStatus	 sMibNodeStatus;
extern thJIP_Mib		 hMibNodeStatus;
#endif

#if MK_BLD_MIB_NODE_CONTROL
extern tsMibNodeControl sMibNodeControl;
extern thJIP_Mib		hMibNodeControl;
#endif

#if MK_BLD_MIB_ADC_STATUS
extern tsMibAdcStatus	 sMibAdcStatus;
extern thJIP_Mib		 hMibAdcStatus;
#endif

#if MK_BLD_MIB_NWK_STATUS
extern tsMibNwkStatus	 sMibNwkStatus;
extern thJIP_Mib		 hMibNwkStatus;
#endif

#if MK_BLD_MIB_NWK_SECURITY
extern tsMibNwkSecurity	 sMibNwkSecurity;
extern thJIP_Mib		 hMibNwkSecurity;
#endif

#if MK_BLD_MIB_NWK_TEST
extern tsMibNwkTest	 	 sMibNwkTest;
extern thJIP_Mib		 hMibNwkTest;
#endif

#if DEBUG_NODE_LQI
extern PUBLIC uint8  	u8LastPktLqi;
uint8     				u8NodePktLqi;
#endif

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/* Flash/EEPROM data */
PRIVATE	uint8 					 u8PdmStartSector	= 0xff;
PRIVATE	uint8					 u8PdmNumSectors	= 0;
PRIVATE	uint32 					u32PdmSectorSize 	= 0;
PRIVATE	uint8					 u8RstSector		= 0xff;
PRIVATE uint8                    u8PrehH2S          = 0 ;
/* PDM device data */
PRIVATE PDM_tsRecordDescriptor   sDeviceDesc;
PRIVATE tsDevicePdm			     sDevicePdm;

/* Application flags */
PRIVATE volatile uint32 u32StackState;

/* Other data */
PRIVATE bool_t   bJoined;
PRIVATE bool_t	 bUp;
PRIVATE uint32 u32TxPending;
PRIVATE bool_t bCalibrate = FALSE;

/* Main timer */
PRIVATE uint32 u32TimerSeconds;
PRIVATE uint32 u32Hours;
PRIVATE uint32 u32TimerTicks;


/* Wake timer data */
#if NODE_WAKE_TIMER_100MS
PRIVATE uint64 u64WakeTimer1Period = (320*DEVICE_WAKE_TIMER_PERIOD_10MS);
PRIVATE uint32 u32WakeTimer1Recoveries;
#endif
#ifdef MK_BLD_NODE_TYPE_END_DEVICE
PRIVATE uint32 u32WakeTimer0Recoveries;
PRIVATE uint32 u32AppTimer100msResume;	/* Forced resume on 100ms timer counter */
PRIVATE bool_t   bAppTimer100msResume;	/* Forced resume on 100ms timer counter */
#endif

/* Timer fired flags */
PRIVATE volatile bool_t   bTickTimerFired;   	/* 10ms only when stack is running */
PRIVATE volatile bool_t   bWakeTimer1Fired;
PRIVATE volatile bool_t   bAppTimer100msFired; 	/* 100ms always running */
PRIVATE volatile bool_t   bSecondTimerFired; 	/* 1s always running */
PRIVATE uint32 u32AppTimer100ms;				/* 100ms timer counter */

/* Joining timeout enabled */
#if DEVICE_JOIN_TIMEOUT
PRIVATE bool_t bJoinTimeout;
#endif

/* Coordinator only data */
#ifdef MK_BLD_NODE_TYPE_COORDINATOR
MAC_ExtAddr_s 		sAuthoriseExtAddr;
tsSecurityKey   	sAuthoriseKey;
uint32			  u32AuthoriseTimer;
#endif

tsMibAdcStatusTemp  	sTemp;
tsMibAdcStatus       *psMibAdcStatus;
tsMibNodeStatusPerm *psMibNodeStatusPerm;
tsMibNodeStatus     *psMibNodeStatus;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: Node_vInit
 *
 * DESCRIPTION:
 * Initialises node hardware
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void Node_vInit(bool_t bWarmStart)
{
	/* Reset stack state */
	u32StackState = NODE_STACK_STATE_NONE;
	/* Clear any left over tick timer fired flag */
	bTickTimerFired = FALSE;
	/* Clear any left over pending transmissions */
	u32TxPending = 0;

	/* Debug ? */
	#ifdef DBG_ENABLE
	{
		/* Wait for clock to stablise */
		while(bAHI_Clock32MHzStable() == FALSE);
		/* Initialise debugging */
		DBG_vUartInit(DEBUG_UART, DEBUG_BAUD_RATE);
		/* Disable the debug port flow control lines to turn off LED2 */
		vAHI_UartSetRTSCTS(DEBUG_UART, FALSE);
		/* Bump baud rate up to 921600 */
		vAHI_UartSetBaudDivisor(DEBUG_UART, 2);
		vAHI_UartSetClocksPerBit(DEBUG_UART, 8);
	}
	#endif

	/* Initialise stack and hardware interfaces */
	v6LP_InitHardware();
	/***  Aicha  ***/
//	/* Initialize CALIBRATION button */
//	vAHI_DioSetDirection(DIO_BUTTON_MASK, 0);
//	vAHI_DioInterruptEnable(DIO_BUTTON_MASK,0);
	/**************/
	/* Cold start ? */
	if (FALSE == bWarmStart)
	{
		u8PrehH2S = 0;
		/* Initialise exception handler */
		Exception_vInit();

		/* Initialise all DIO as inputs, pullups enabled */
		vAHI_DioSetDirection(0xFFFFFFFF, 0);
		vAHI_DioSetPullup(0xFFFFFFFF, 0);
	}

	/* Enable for high temperature use */
	vAHI_ExtendedTemperatureOperation(TRUE);

	/* Using wake timer 1 for 100ms timings ? */
	#if NODE_WAKE_TIMER_100MS
	{
		/* Cold start ? */
		if (FALSE == bWarmStart)
		{
			uint32 u32WakeTimerCalibrate;

			/* Calibrate the wake timer */
			vAHI_WakeTimerStop(E_AHI_WAKE_TIMER_0);
			(void) u8AHI_WakeTimerFiredStatus();
			u32WakeTimerCalibrate = u32AHI_WakeTimerCalibrate();
			/* Calculate wake timer sleep periods */ //3200000*10=32000*10000*0.1
			u64WakeTimer1Period = (DEVICE_WAKE_TIMER_SCALE_10MS / u32WakeTimerCalibrate) * DEVICE_WAKE_TIMER_PERIOD_10MS ;			/* Enable wake timer 1 */
			vAHI_WakeTimerEnable(E_AHI_WAKE_TIMER_1, TRUE);
			/* Start running timer */
			vAHI_WakeTimerStartLarge(E_AHI_WAKE_TIMER_1, u64WakeTimer1Period);
		}
	}
	#endif

	/* Work out allocation of EEPROM sectors (varies per chip and flash device) */
	/* PDM configuration Assume 64*64byte sectors in EEPROM */
	u8PdmStartSector = 0;
	u32PdmSectorSize = 64;
	u8PdmNumSectors  = 62;
	/* Reset configuration */
	u8RstSector      = 62;
	/* Zero indentation */
	i32DebugIndent = 0;
	acDebugIndent[i32DebugIndent] = '\0';
}

/****************************************************************************
 *
 * NAME: Node_bTestFactoryResetEeprom (6x EEPROM version)
 *
 * DESCRIPTION:
 * Factory reset detection
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC bool_t Node_bTestFactoryResetEeprom(void)
{
	bool_t	 bReturn = FALSE;
	uint32 au32EepromA[3];
	uint32 au32EepromB[3];
	uint8    u8Eeprom = 0xFF;

	/* Debug */
	DBG_vPrintf(DEBUG_NODE_FUNC, "\n%sNode_bTestFactoryResetEeprom()", acDebugIndent);
	Node_vDebugIndent(DEBUG_NODE_FUNC);

	/* Read EEPROM data */
	bAHI_ReadEEPROM(u8RstSector, 0,                   sizeof(au32EepromA), (uint8 *) au32EepromA);
	bAHI_ReadEEPROM(u8RstSector, sizeof(au32EepromA), sizeof(au32EepromB), (uint8 *) au32EepromB);
	/* Is first EEPROM record valid ? */
	if (FACTORY_RESET_MAGIC == au32EepromA[1] &&
	    MK_JIP_DEVICE_ID    == au32EepromA[2])
	{
		/* Copy left shifted flags into second record */
		au32EepromB[0] = au32EepromA[0] << 1;
		/* Transfer other data */
		au32EepromB[1] = au32EepromA[1];
		au32EepromB[2] = au32EepromA[2];
		/* Invalidate first record */
		au32EepromA[1] = 0;
		/* Write second record to EEPROM */
		bAHI_WriteEEPROM(u8RstSector, sizeof(au32EepromA), sizeof(au32EepromB), (uint8 *) au32EepromB);
		/* Write invalidated first record to EEPROM */
		bAHI_WriteEEPROM(u8RstSector, 0,                   sizeof(au32EepromA), (uint8 *) au32EepromA);
		/* Note we are using second record */
		u8Eeprom = 1;
	}
	/* Is second EEPROM record valid ? */
	else if (FACTORY_RESET_MAGIC == au32EepromB[1] &&
	         MK_JIP_DEVICE_ID    == au32EepromB[2])
	{
		/* Copy left shifted flags into first record */
		au32EepromA[0] = au32EepromB[0] << 1;
		/* Transfer other data */
		au32EepromA[1] = au32EepromB[1];
		au32EepromA[2] = au32EepromB[2];
		/* Invalidate first record */
		au32EepromB[1] = 0;
		/* Write first record to EEPROM */
		bAHI_WriteEEPROM(u8RstSector, 0,                   sizeof(au32EepromA), (uint8 *) au32EepromA);
		/* Write invalidated second record to EEPROM */
		bAHI_WriteEEPROM(u8RstSector, sizeof(au32EepromA), sizeof(au32EepromB), (uint8 *) au32EepromB);
		/* Note we are using first record */
		u8Eeprom = 0;
	}
	/* No valid records ? */
	else
	{
		uint8  u8Sector;
		uint8 au8Junk[64];
		/* Zero junk data */
		memset(au8Junk, 0, sizeof(au8Junk));
		/* Loop through EEPROM sectors */
		for (u8Sector = 0; u8Sector < 63; u8Sector++)
		{
			/* Write junk into the sector to invalidate it */
			bAHI_WriteEEPROM(u8Sector, 0, 64, au8Junk);
		}
		/* Initialise history */
		au32EepromA[0] = 0xfffffffe;
		au32EepromA[1] = FACTORY_RESET_MAGIC;
		au32EepromA[2] = MK_JIP_DEVICE_ID;
		/* Write back to EEPROM */
		bAHI_WriteEEPROM(u8RstSector, 0, sizeof(au32EepromA), (uint8 *) au32EepromA);
		/* Note we are using first record */
		u8Eeprom = 0;
	}

	/* Start tick timer for 2 seconds */
	vAHI_TickTimerConfigure(E_AHI_TICK_TIMER_DISABLE);
	vAHI_TickTimerWrite(0);
	vAHI_TickTimerInterval(32000000);
	vAHI_TickTimerConfigure(E_AHI_TICK_TIMER_STOP);
	vAHI_TickTimerIntEnable(TRUE);
	/* Doze until tick timer fires */
	while (u32AHI_TickTimerRead() < 32000000)
	{
		/* Doze until tick timer fires */
		vAHI_CpuDoze();
	}
	/* Disable tick timer */
	vAHI_TickTimerConfigure(E_AHI_TICK_TIMER_DISABLE);

	/* Using first record ? */
	if (u8Eeprom == 0)
	{
		/* Update history in second record to flag we've run for a 2 seconds */
		au32EepromB[0] = (au32EepromA[0] | 0x1);
		/* Transfer other data */
		au32EepromB[1] = au32EepromA[1];
		au32EepromB[2] = au32EepromA[2];
		/* Invalidate first record */
		au32EepromA[1] = 0;
		/* Write second record to EEPROM */
		bAHI_WriteEEPROM(u8RstSector, sizeof(au32EepromA), sizeof(au32EepromB), (uint8 *) au32EepromB);
		/* Write invalidated first record to EEPROM */
		bAHI_WriteEEPROM(u8RstSector, 0,                   sizeof(au32EepromA), (uint8 *) au32EepromA);
		/* Has the device been on with the following history long, short, short, short, long ? */
		if ((au32EepromB[0] & 0x1F) == 0x11) bReturn = TRUE;
	}
	/* Using second record ? */
	else if (u8Eeprom == 1)
	{
		/* Update history in first record to flag we've run for a 2 seconds */
		au32EepromA[0] = (au32EepromB[0] | 0x1);
		/* Transfer other data */
		au32EepromA[1] = au32EepromB[1];
		au32EepromA[2] = au32EepromB[2];
		/* Invalidate first record */
		au32EepromB[1] = 0;
		/* Write first record to EEPROM */
		bAHI_WriteEEPROM(u8RstSector, 0,                   sizeof(au32EepromA), (uint8 *) au32EepromA);
		/* Write invalidated second record to EEPROM */
		bAHI_WriteEEPROM(u8RstSector, sizeof(au32EepromA), sizeof(au32EepromB), (uint8 *) au32EepromB);
		/* Has the device been on with the following history long, short, short, short, long ? */
		if ((au32EepromA[0] & 0x1F) == 0x11) bReturn = TRUE;
	}

	/* Debug */
	Node_vDebugOutdent(DEBUG_NODE_FUNC);
	DBG_vPrintf(DEBUG_NODE_FUNC, " = %d", bReturn);

	return bReturn;
}

/****************************************************************************
 *
 * NAME: Node_vPdmInit
 *
 * DESCRIPTION:
 * Initialises software
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void Node_vPdmInit(uint8 u8AdcMask,
							uint8 u8AdcPeriod)
{
	/* Debug */
	DBG_vPrintf(DEBUG_NODE_FUNC, "\n%sNode_vPdmInit(%02x, %d, %d)", acDebugIndent, u8AdcMask, u8AdcPeriod);
	Node_vDebugIndent(DEBUG_NODE_FUNC);

	/* Initialise PDM */
	PDM_vInit(u8PdmStartSector,
			  u8PdmNumSectors,
			  u32PdmSectorSize,
			  (OS_thMutex) 1,	/* Mutex */
			  NULL,
			  NULL,
			  NULL);

	/* Load Device PDM data */
	(void) PDM_eLoadRecord(&sDeviceDesc,
						   0xFFFF,
						   (void *) &sDevicePdm,
						   sizeof(sDevicePdm),
						   FALSE);

	/* Record was recovered from flash ? */
	if (PDM_RECOVERY_STATE_RECOVERED == sDeviceDesc.eState)
	{
		/* Is the JIP Device ID unexpected ? */
		if (MK_JIP_DEVICE_ID != sDevicePdm.u32JipDeviceId)
		{
			/* Delete the PDM data and reset to start again */
			PDM_vDelete();
			/* Reset */
			vAHI_SwReset();
		}
	}
	/* Record was not recovered from flash */
	else
	{
		/* Set correct JIP Device ID */
		sDevicePdm.u32JipDeviceId = MK_JIP_DEVICE_ID;
		sDevicePdm.u32Spare       = 0xffffffff;
		/* Make sure permament data is saved */
		PDM_vSaveRecord(&sDeviceDesc);
	}

	/* Initialise mibs (which reads PDM data) */
	#if MK_BLD_MIB_NODE
		MibNode_vInit       (&sMibNode);
	#endif
	#if MK_BLD_MIB_GROUP
		MibGroup_vInit      (&sMibGroup);
	#endif
	#if MK_BLD_MIB_NODE_STATUS
		MibNodeStatus_vInit (hMibNodeStatus, &sMibNodeStatus);
	#endif
	#if MK_BLD_MIB_NODE_CONTROL
		MibNodeControl_vInit(hMibNodeControl, &sMibNodeControl);
	#endif
	#if MK_BLD_MIB_ADC_STATUS
		MibAdcStatus_vInit  (hMibAdcStatus, &sMibAdcStatus, u8AdcMask, u8AdcPeriod);
	#endif
	#if MK_BLD_MIB_NWK_STATUS
		MibNwkStatus_vInit  (hMibNwkStatus, &sMibNwkStatus, MK_SECURITY);
	#endif
	#if MK_BLD_MIB_NWK_SECURITY
		MibNwkSecurity_vInit();
	#endif
	#if MK_BLD_MIB_NWK_TEST
		MibNwkTest_vInit(hMibNwkTest, &sMibNwkTest);
	#endif

	/* Debug */
	Node_vDebugOutdent(DEBUG_NODE_FUNC);
}

/****************************************************************************
 *
 * NAME: Node_vReset
 *
 * DESCRIPTION:
 * Reset device
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void Node_vReset(bool_t bFactoryReset)
{
	/* UART ? */
	#ifdef UART_H_INCLUDED
	{
		/* Initialise UART */
		UART_vChar('<');
	}
	#endif

	/* Debug */
	DBG_vPrintf(DEBUG_NODE_FUNC, "\n%sNode_vReset(%d)", acDebugIndent, bFactoryReset);
	Node_vDebugIndent(DEBUG_NODE_FUNC);

	/* Increment reset counter in node status mib */
	#if MK_BLD_MIB_NODE_STATUS
		MibNodeStatus_vIncrementResetCount();
	#endif

	/* FactoryReset ? */
	if (bFactoryReset)
	{
		/* Save or delete records as appropriate for a factory reset */
		#if MK_BLD_MIB_NODE
			/* Delete node record */
			PDM_vDeleteRecord(&sMibNode.sDesc);
		#endif
		#if MK_BLD_MIB_GROUP
			/* Delete group record */
			PDM_vDeleteRecord(&sMibGroup.sDesc);
		#endif
		#if MK_BLD_MIB_NODE_STATUS
			/* Ensure this record is saved */
			PDM_vSaveRecord(&sMibNodeStatus.sDesc);
		#endif
		#if MK_BLD_MIB_NWK_STATUS
			/* Ensure this record is saved */
			PDM_vSaveRecord(&sMibNwkStatus.sDesc);
		#endif
		#if MK_BLD_MIB_NWK_SECURITY
			/* Delete this record */
			PDM_vDeleteRecord(&sMibNwkSecurity.sDesc);
		#endif
	}
	/* Normal reset */
	else
	{
		/* Save all records to PDM */
		PDM_vSave();
	}

	/* Debug */
	DBG_vPrintf(DEBUG_NODE_FUNC, "\n%svAHI_SwReset(%d)!                ", acDebugIndent, bFactoryReset);
	/* UART ? */
	#ifdef UART_H_INCLUDED
	{
		/* Initialise UART */
		UART_vChar('>');
		UART_vChar(' ');
	}
	#endif
	/* Reset */
	vAHI_SwReset();

	/* Debug */
	Node_vDebugOutdent(DEBUG_NODE_FUNC);
}

/****************************************************************************
 *
 * NAME: Node_eJipInit
 *
 * DESCRIPTION:
 * Initialises software
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC teJIP_Status Node_eJipInit(void)
{
	teJIP_Status     eStatus;

	/* Debug */
	DBG_vPrintf((DEBUG_NODE_FUNC | DEBUG_NODE_LQI), "\n%sNode_eJipInit()", acDebugIndent);
	Node_vDebugIndent(DEBUG_NODE_FUNC);

	/* UART ? */
	#ifdef UART_H_INCLUDED
	{
		/* Initialise UART */
		UART_vChar(' ');
		UART_vChar('j');
		UART_vChar(' ');
	}
	#endif

	/* Initialise data from network security mib */
	#if MK_BLD_MIB_NWK_SECURITY
		eStatus = MibNwkSecurity_eJipInit();
	#endif

	/* Module type is set */
	#ifdef NODE_MODULE_TYPE
		#warning Setting high power mode
		vMiniMac_SetHighPowerMode(NODE_MODULE_TYPE);
	#endif
	/* Note stack is running */
	u32StackState = NODE_STACK_STATE_RUNNING;

	#ifdef MK_BLD_NODE_TYPE_END_DEVICE
	/* Register pre-sleep callback */
	vApi_RegPreSleepCallback(Node_vPreSleepCallback);
	#endif

	/* Register mib variables */
	#if MK_BLD_MIB_NODE
		/* Doesn't really register but does set default name */
		MibNode_vRegister();
	#endif
	#if MK_BLD_MIB_GROUP
		/* Restore groups */
		MibGroup_vRestore();
	#endif
	#if MK_REG_MIB_NODE_STATUS
		MibNodeStatus_vRegister();
	#endif
	#if MK_REG_MIB_NODE_CONTROL
		MibNodeControl_vRegister();
	#endif
	#if MK_REG_MIB_NWK_STATUS
		MibNwkStatus_vRegister();
	#endif
	#if MK_REG_MIB_NWK_SECURITY
		MibNwkSecurity_vRegister();
	#endif
	#if MK_REG_MIB_NWK_TEST
		MibNwkTest_vRegister();
	#endif
	#if MK_REG_MIB_ADC_STATUS
		MibAdcStatus_vRegister();
	#endif

	/* Debug */
	Node_vDebugOutdent(DEBUG_NODE_FUNC);

	return eStatus;
}

/****************************************************************************
 *
 * NAME: Node_v6lpConfigureNetwork
 *
 * DESCRIPTION:
 * Configures network
 *
 ****************************************************************************/
PUBLIC void Node_v6lpConfigureNetwork(tsNetworkConfigData *psNetworkConfigData)
{
	/* Debug */
	DBG_vPrintf(DEBUG_NODE_FUNC, "\n%sNode_v6lpConfigureNetwork()", acDebugIndent);
	Node_vDebugIndent(DEBUG_NODE_FUNC);

	/* Get network security set up correctly */
	MibNwkSecurity_v6lpConfigureNetwork(psNetworkConfigData);

	/* Debug */
	Node_vDebugOutdent(DEBUG_NODE_FUNC);
}

/****************************************************************************
 *
 * NAME: Node_vJipResume
 *
 * DESCRIPTION:
 * Flags that network should be resumed (end device only)
 *
 ****************************************************************************/
#ifdef MK_BLD_NODE_TYPE_END_DEVICE
PUBLIC void Node_v6lpResume(void)
{
	/* Debug */
	DBG_vPrintf(DEBUG_NODE_FUNC, "\n%sNode_v6lpResume()", acDebugIndent);
	/* Stack is not running ? */
	if (u32StackState == NODE_STACK_STATE_NONE)
	{
		/* Join timeout specified ? */
		#if DEVICE_JOIN_TIMEOUT
		/* Join timeout has not expired */
		if (FALSE == bJoinTimeout)
		#endif
		{
			/* Flag that we want to run the stack */
			u32StackState = NODE_STACK_STATE_RESUME;
		}
	}
}
#endif

/****************************************************************************
 *
 * NAME: Node_v6lpDataEvent
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
PUBLIC void Node_v6lpDataEvent(  int 			  iSocket,
							  	 te6LP_DataEvent  eEvent,
                              	 ts6LP_SockAddr *psAddr,
                              	 uint8           u8AddrLen)
{
	/* Which event ? */
	switch (eEvent)
	{
		/* Data received ? */
		/* IP data received ? */
		/* 6LP ICMP message ? */
		case E_DATA_RECEIVED:
		case E_IP_DATA_RECEIVED:
		case E_6LP_ICMP_MESSAGE:
		{

			/* Discard 6LP packets as only interested in JIP communication  */
			i6LP_RecvFrom(iSocket, NULL, 0, 0, NULL, NULL);
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
 * NAME: Node_vJipStackEvent
 *
 * DESCRIPTION:
 * Processes any incoming stack events.
 * Once a join indication has been received, we initialise JIP and register
 * the various MIBs.
 *
 * PARAMETERS: Name          RW Usage
 *             eEvent        R  Stack event
 *             pvData        R  Additional information associated with event
 *             u8DataLen     R  Length of additional information
 *
 ****************************************************************************/
PUBLIC bool_t Node_bJipStackEvent(te6LP_StackEvent eEvent, void *pvData, uint8 u8DataLen)
{
	bool_t bReturn   = FALSE;

	/* Debug */
	DBG_vPrintf(DEBUG_NODE_FUNC, "\n%sNode_bJipStackEvent(%d)", acDebugIndent, eEvent);
	Node_vDebugIndent(DEBUG_NODE_FUNC);
	/* Which event ? */
    switch (eEvent)
    {
		/* Started network ? */
    	/* Joined network ? */
		case E_STACK_STARTED:
		case E_STACK_JOINED:
		{
			tsNwkInfo *psNwkInfo;
			/* Cast data pointer to correct type */
			psNwkInfo = (tsNwkInfo *) pvData;
			/* Debug */
			DBG_vPrintf(DEBUG_NODE_LQI, "\n%sNode_bJipStackEvent(JOINED)", acDebugIndent);
			DBG_vPrintf(DEBUG_NODE_FUNC, " JOINED = {%x:%x 0x%x %d %d}",
				psNwkInfo->sParentAddr.u32H,
				psNwkInfo->sParentAddr.u32L,
				psNwkInfo->u16PanID,
				psNwkInfo->u16Depth,
				psNwkInfo->u8Channel);

			/* UART ? */
			#ifdef UART_H_INCLUDED
			{
				/* Initialise UART */
				UART_vChar(' ');
				UART_vChar('J');
				UART_vChar('-');
				UART_vNumber(psNwkInfo->u8Channel, 10);
				UART_vString("-0x");
				UART_vNumber(psNwkInfo->u16PanID, 16);
				UART_vChar(' ');
			}
			#endif

			#if MK_BLD_MIB_GROUP
				/* First join - restore groups */
				if (FALSE == bJoined) MibGroup_vRestore();
			#endif

			/* Network has been joined */
			bJoined = TRUE;
			/* Network is now up */
			bUp     = TRUE;

		    /* Commissioning timeout set ? */
		    #if (DEVICE_COMMISSION_TIMEOUT > 0)
			#if MK_BLD_MIB_NODE_CONTROL && MK_BLD_MIB_NWK_STATUS
		    {
				/* Have we joined in standalone mode (rather than resumed) ? */
				if ((u16Api_GetStackMode() & NONE_GATEWAY_MODE) != 0 &&
					sMibNwkStatus.sPerm.u8UpMode == MIB_NWK_STATUS_UP_MODE_NONE)
				{
					/* Start the factory reset timer (using dummy call to set data function) */
					MibNodeControl_eSetFactoryReset(DEVICE_COMMISSION_TIMEOUT, &sMibNodeControl.sTemp.u16FactoryReset);
				}
			}
			#endif
			#endif
		}
		break;

		/* Lost network ? */
		case E_STACK_RESET:
		{
			/* Debug */
			DBG_vPrintf(DEBUG_NODE_LQI, "\n%sNode_bJipStackEvent(RESET)", acDebugIndent);
			DBG_vPrintf(DEBUG_NODE_FUNC, " RESET");
			/* UART ? */
			#ifdef UART_H_INCLUDED
			{
				/* Initialise UART */
				UART_vChar(' ');
				UART_vChar('X');
				UART_vChar(' ');
			}
			#endif

			/* Network is now down */
			bUp = FALSE;

			/* Never joined a network ? */
			if (sMibNwkStatus.sPerm.u8UpMode == MIB_NWK_STATUS_UP_MODE_NONE)
			{
				/* Re-apply default join profile */
				(void) bJnc_SetJoinProfile(CONFIG_JOIN_PROFILE, NULL);
			}
		}
		break;

		/* Gateway present ? */
		case E_STACK_NETWORK_ANNOUNCE:
		{
			/* Debug */
			DBG_vPrintf(DEBUG_NODE_FUNC, " NETWORK_ANNOUNCE");
			DBG_vPrintf(TRUE, " NETWORK_ANNOUNCE");
		}
		break;

		case E_STACK_NODE_JOINED:
		{
			MAC_ExtAddr_s sExtAddr;
			/* Extract MAC address of device seeking authorisation */
			memcpy((uint8 *) &sExtAddr, (uint8 *) pvData, sizeof(MAC_ExtAddr_s));
			/* Debug */
			DBG_vPrintf(DEBUG_NODE_FUNC, " NODE_JOINED %x:%x", sExtAddr.u32H, sExtAddr.u32L);
			#ifdef MK_BLD_NODE_TYPE_COORDINATOR
			{
				/* Is this the node currently being authorised ? */
				if (memcmp(&sAuthoriseExtAddr, &sExtAddr, sizeof(MAC_ExtAddr_s)) == 0)
				{
					DBG_vPrintf(DEBUG_NODE_FUNC, " cleared");
					/* Clear the authorised node to allow another to join */
					sAuthoriseExtAddr.u32H = 0;
					sAuthoriseExtAddr.u32L = 0;
					u32AuthoriseTimer = 0;
				}
//				/* Build 6LP address from mac address of the node previously joined */
//						teJIP_Status eStatus;
//						ts6LP_SockAddr s6LP_SockAddr;
//						EUI64_s sIntAddr ;
//
//					    memset(&s6LP_SockAddr, 0, sizeof(ts6LP_SockAddr));
//
//						i6LP_CreateInterfaceIdFrom64(&sIntAddr, (EUI64_s *) &sExtAddr);
//						i6LP_CreateLinkLocalAddress (&s6LP_SockAddr.sin6_addr, &sIntAddr);
//						/* Complete full socket address */
//						s6LP_SockAddr.sin6_family = E_6LP_PF_INET6;
//						s6LP_SockAddr.sin6_flowinfo =0;
//						s6LP_SockAddr.sin6_port = JIP_DEFAULT_PORT;
//						s6LP_SockAddr.sin6_scope_id =0;
//						eStatus = eJIP_Remote_Mib_Get( &s6LP_SockAddr,
//													   psMibCOstatus->sPerm.COppm,
//													   MIB_ID_CO_STATUS,
//													   VAR_IX_CO_STATUS_PPM_CURRENT	);
//													   #if 0
//													   0xffffff00,	/* Node MIB */
//													   0);			/* MAC Address Variable */
//													   #endif
//						/* Successful request */
//						if (E_JIP_OK == eStatus)
//						/* Debug */
//						DBG_vPrintf(TRUE, "\nCOppm=%d", psMibCOstatus->sPerm.COppm);
			}
			#endif
		}
		break;

		case E_STACK_NODE_LEFT:
		{
			MAC_ExtAddr_s sExtAddr;
			/* Extract MAC address of device seeking authorisation */
			memcpy((uint8 *) &sExtAddr, (uint8 *) pvData, sizeof(MAC_ExtAddr_s));
			/* Debug */
			DBG_vPrintf(DEBUG_NODE_FUNC, " NODE_LEFT %x:%x", sExtAddr.u32H, sExtAddr.u32L);
		}
		break;

		/* Coordinator only events ? */
		#ifdef MK_BLD_NODE_TYPE_COORDINATOR
		/* Broadcast authorise ? */
		case E_STACK_NODE_AUTHORISE:
		{
			MAC_ExtAddr_s sExtAddr;
			MAC_ExtAddr_s sKeyAddr;

			/* Extract MAC address of device seeking authorisation */
			memcpy((uint8 *) &sExtAddr, (uint8 *) pvData, sizeof(MAC_ExtAddr_s));
			/* Debug */
			DBG_vPrintf(DEBUG_NODE_FUNC, " NODE_AUTHORISE %x:%x", sExtAddr.u32H, sExtAddr.u32L);

			/* Are we not currently authorising a device or this is a repeat from the device we are already trying to authorise ? */
			if ((sAuthoriseExtAddr.u32L == 0 && sAuthoriseExtAddr.u32H == 0) ||
				(memcmp(&sAuthoriseExtAddr, &sExtAddr, sizeof(MAC_ExtAddr_s)) == 0))
			{
				/* Note the MAC address of the device we are going to authorise */
				memcpy((uint8 *) &sAuthoriseExtAddr, (uint8 *) pvData, sizeof(MAC_ExtAddr_s));
				/* Authorisation timer not already running - start authorisation timer */
				if (u32AuthoriseTimer == 0) u32AuthoriseTimer = 1500;
				/* Reverse the word order for key building */
				sKeyAddr.u32H = sAuthoriseExtAddr.u32L;
				sKeyAddr.u32L = sAuthoriseExtAddr.u32H;
				/* Build commissioning key */
				Security_vBuildCommissioningKey((uint8 *) &sKeyAddr, (uint8 *) &sAuthoriseKey);
				/* Commission node to authorise it into the network */
				eApi_CommissionNode(&sAuthoriseExtAddr, &sAuthoriseKey);
				/* Debug */
				DBG_vPrintf(DEBUG_NODE_FUNC, " allowed");
			}
			else
			{
				/* Debug */
				DBG_vPrintf(DEBUG_NODE_FUNC, " denied");
			}
		}
		break;

		/* SDK version dependent events ? */
		#if (MK_JENNIC_SDK_VERSION >= 1107)
		/* Unicast authorise ? */
		case E_STACK_NODE_AUTH_UCAST:
		{
			tsNodeAuthoriseRequest sNodeAuthoriseRequest;
			MAC_ExtAddr_s          sKeyAddr;
			tsSecurityKey   	   sAuthoriseUnicastKey;

			/* Extract authorisation data */
			memcpy((uint8 *) &sNodeAuthoriseRequest, (uint8 *) pvData, sizeof(tsNodeAuthoriseRequest));
			/* Debug */
			DBG_vPrintf(DEBUG_NODE_FUNC, " NODE_AUTH_UCAST %x:%x", sNodeAuthoriseRequest.sJoinMacAddr.u32H, sNodeAuthoriseRequest.sJoinMacAddr.u32L);
			/* Reverse the word order for key building */
			sKeyAddr.u32H = sNodeAuthoriseRequest.sJoinMacAddr.u32L;
			sKeyAddr.u32L = sNodeAuthoriseRequest.sJoinMacAddr.u32H;
			/* Build commissioning key */
			Security_vBuildCommissioningKey((uint8 *) &sKeyAddr, (uint8 *) &sAuthoriseUnicastKey);
			/* Commission node to authorise it into the network */
			v6LP_SendNodeAuthorisationResponse(&sNodeAuthoriseRequest.sJoinMacAddr, (uint8 *) &sAuthoriseUnicastKey, &sNodeAuthoriseRequest.sParentSockAddr);
			/* Debug */
			DBG_vPrintf(DEBUG_NODE_FUNC, " allowed");
		}
		break;
		#endif

		/* Node joined network ? */
		case E_STACK_NODE_JOINED_NWK:
		{
			MAC_ExtAddr_s sExtAddr;
			/* Extract MAC address of device seeking authorisation */
			memcpy((uint8 *) &sExtAddr, (uint8 *) pvData, sizeof(MAC_ExtAddr_s));
			/* Debug */
			DBG_vPrintf(DEBUG_NODE_FUNC, " NODE_JOINED_NWK %x:%x", sExtAddr.u32H, sExtAddr.u32L);
			/* Is this the node currently being authorised ? */
			if (memcmp(&sAuthoriseExtAddr, &sExtAddr, sizeof(MAC_ExtAddr_s)) == 0)
			{
				DBG_vPrintf(DEBUG_NODE_FUNC, " cleared");
				/* Clear the authorised node to allow another to join */
				sAuthoriseExtAddr.u32H = 0;
				sAuthoriseExtAddr.u32L = 0;
				u32AuthoriseTimer = 0;
			}
		}
		break;

		/* Node left network ? */
		case E_STACK_NODE_LEFT_NWK:
		{
			MAC_ExtAddr_s sExtAddr;
			/* Extract MAC address of device seeking authorisation */
			memcpy((uint8 *) &sExtAddr, (uint8 *) pvData, sizeof(MAC_ExtAddr_s));
			/* Debug */
			DBG_vPrintf(DEBUG_NODE_FUNC, " NODE_LEFT_NWK %x:%x", sExtAddr.u32H, sExtAddr.u32L);
		}
		break;
		#endif

		/* Allowing sleep ? */
		#ifdef MK_BLD_NODE_TYPE_END_DEVICE
		/* Poll event ? */
		case E_STACK_POLL:
		{
			te6LP_PollResponse ePollResponse;

			/* Cast response */
			ePollResponse = *((te6LP_PollResponse *) pvData);
			/* Debug */
			DBG_vPrintf(DEBUG_DEVICE_FUNC, " POLL %d", ePollResponse);

			/* Which response ? */
			switch (ePollResponse)
			{
				/* Got some data ? */
				case E_6LP_POLL_DATA_READY:
				{
					/* UART ? */
					#ifdef UART_H_INCLUDED
					{
						/* Initialise UART */
						UART_vChar('D');
					}
					#endif
					/* Debug */
					DBG_vPrintf(DEBUG_DEVICE_FUNC, " DATA_READY");
					/* Poll again in case there is more */
					ePollResponse = e6LP_Poll();
					/* Debug */
					DBG_vPrintf(DEBUG_DEVICE_FUNC, "\n%se6LP_Poll() = %d", acDebugIndent, ePollResponse);
				}
				break;

				/* Others ? */
				default:
				{
					/* UART ? */
					#ifdef UART_H_INCLUDED
					{
						/* Initialise UART */
						UART_vChar('E');
					}
					#endif
					/* Network up - set flag to indicate polled but no data */
					if (bUp) bReturn = TRUE;
				}
				break;
			}
		}
		break;
		#endif

		/* Others ? */
	    default:
		{
			/* Do nothing */
			;
		}
        break;
    }

	/* Pass on to mibs */
	#if MK_BLD_MIB_NWK_STATUS
		/* Pass on to MIB */
		MibNwkStatus_vStackEvent(eEvent);
	#endif
	#if MK_BLD_MIB_NWK_SECURITY
		/* Pass on to MIB */
		MibNwkSecurity_vStackEvent(eEvent, pvData, u8DataLen);
	#endif
	#if MK_BLD_MIB_NWK_TEST
		/* Pass on to MIB */
		MibNwkTest_vStackEvent(eEvent, pvData, u8DataLen);
	#endif

	/* Debug */
	Node_vDebugOutdent(DEBUG_NODE_FUNC);
	DBG_vPrintf(DEBUG_NODE_FUNC, " = %d", bReturn);

	return bReturn;
}

/****************************************************************************
 *
 * NAME: Node_vMain
 *
 * DESCRIPTION:
 * Called each timne around the main loop
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void Node_vMain(void)
{

	/* Pass on to NwkSecurity MIB */
	MibNwkSecurity_vMain();
	/* End Device ? */
	#ifdef MK_BLD_NODE_TYPE_END_DEVICE
	{
		/* Should we wake the stack ? */
		if (u32StackState == NODE_STACK_STATE_RESUME)
		{
			u32StackState = NODE_STACK_STATE_RUNNING;
			/* Debug */
			DBG_vPrintf(DEBUG_NODE_LQI, "\n%si6LP_ResumeStack()", acDebugIndent);
			DBG_vPrintf(DEBUG_DEVICE_FUNC, "\n%sNode_vMain { i6LP_ResumeStack() }", acDebugIndent);
			/* Resume 6LoWPAN */
			i6LP_ResumeStack();
			/* Create a dummy timers events */
			bTickTimerFired = TRUE;
			bSecondTimerFired = TRUE;
			/* Module type is set */
			#ifdef NODE_MODULE_TYPE
				#warning Setting high power mode
				vMiniMac_SetHighPowerMode(NODE_MODULE_TYPE);
			#endif
		}
	}
	#endif
	/* Has the application second timer fired ? */
	if (bSecondTimerFired && bWakeTimer1Fired)
	{
		bSecondTimerFired = FALSE;
		Node_vSecond();
	}
	/* Has the 10ms stack tick timer fired ? */
	if (bTickTimerFired)
	{
		/* Clear tick timer fired flag */
		bTickTimerFired = FALSE;
	//	DBG_vPrintf(TRUE,"\nbTickTimerFired");
		/* Handle tick timer firing */
		Node_vTick();
	}
	/* Allow stack processing */
	if (u32StackState == NODE_STACK_STATE_RUNNING) vJIP_Tick();
}

/****************************************************************************
 *
 * NAME: Node_vSysCtrlEvent
 *
 * DESCRIPTION:
 * Call when system control interrupts are raised
 *
 ****************************************************************************/
PUBLIC void Node_vSysCtrlEvent(uint32 u32Device, uint32 u32ItemBitmap)
{
	/* Double check its from system control ? */
	if (E_AHI_DEVICE_SYSCTRL == u32Device)
	{
		/* End Device ? */
		#ifdef MK_BLD_NODE_TYPE_END_DEVICE
		{
			/* Is this interrupt from wake timer 0 (stack timing) ? */
			if (u32ItemBitmap & E_AHI_SYSCTRL_WK0_MASK)
			{
				/* Stack is not running ? */
				if (Node_u32StackState() == NODE_STACK_STATE_NONE)
				{
					/* Flag that we want to run the stack */
					Node_v6lpResume();
					//DBG_vPrintf(TRUE,"\nE_AHI_SYSCTRL_WK0_MASK");
				}
				/* Is the wake timer running (sometimes the stack generates a fake interrupt) ? */
				if (u8AHI_WakeTimerStatus() & E_AHI_WAKE_TIMER_MASK_0)
				{
					/* Toggle LED WK0 */
					//vREG_GpioWrite(REG_GPIO_DOUT, u32REG_GpioRead(REG_GPIO_DOUT) ^ DEVICE_OUTPUT_MASK_WK0);
				}
			}
			/***  Aicha  ***/
			/* Is this interrupt from DIO11(calibration  button) ? */
//			if (u32ItemBitmap & E_AHI_DIO11_INT)
//			{}
			/***************/


		}
		#endif

		/* Using wake timer 1 for 100ms timings ? */
		#if NODE_WAKE_TIMER_100MS
		{
			/* Is this interrupt from wake timer 1 (application timing) ? */
			if (u32ItemBitmap & E_AHI_SYSCTRL_WK1_MASK)
			{
				DBG_vPrintf(TRUE,"\nE_AHI_SYSCTRL_WK1_MASK");
				/* Restart running timer */
				vAHI_WakeTimerStartLarge(E_AHI_WAKE_TIMER_1, u64WakeTimer1Period - (DEVICE_WAKE_TIMER_MAX - u64AHI_WakeTimerReadLarge(E_AHI_WAKE_TIMER_1)));
				/***  Aicha ***/
				bWakeTimer1Fired = TRUE;
				u32TimerSeconds=1;
				 vAHI_WakeTimerStop(E_AHI_WAKE_TIMER_1);

				/* End device ? */
				#ifdef MK_BLD_NODE_TYPE_END_DEVICE
				{
					/* Stack is not running and network is up ? */
					if (Node_u32StackState() == NODE_STACK_STATE_NONE && bUp)
					{
						/* Is a resume is being forced ? */
						if (bAppTimer100msResume == TRUE && u32AppTimer100ms == u32AppTimer100msResume)
						{
							/* Clear the forced resume flag */
							bAppTimer100msResume = FALSE;
							/* Flag that we want to run the stack */
							Node_v6lpResume();
						}
						/* Is a resume scheduled ? */
						else if ((u32AppTimer100ms % DEVICE_ED_STACK_RESUME) == 0)
						{
							/* Flag that we want to run the stack */
							Node_v6lpResume();
						}
					}
				}
				#endif

				/* Flag 100ms timer has fired */
				bAppTimer100msFired = TRUE;
				/* Increment counter */
				u32AppTimer100ms++;
			}
		}
		#endif
	}
}

/****************************************************************************
 *
 * NAME: Node_vTickTimerEvent
 *
 * DESCRIPTION:
 * Called when tick timer interrupts (every 10ms when stack is running)
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void Node_vTickTimerEvent(void)
{
	/* Note tick timer has fired */
	bTickTimerFired = TRUE;

	/* Not using wake timer 1 for 100ms timings ? */
	#if (NODE_WAKE_TIMER_100MS == FALSE)
	{
		/* Increment tick counter */
		u32TimerTicks++;
		/* Reached required interval ? */
		if (u32TimerTicks % DEVICE_WAKE_TIMER_PERIOD_10MS == 0)
		{
			/* Tick timer has gone over 1 second */
			if (u32TimerTicks >= 100)
			{
				/* Increment seconds */
				u32TimerSeconds++;
				/* Update ticks */
				u32TimerTicks -= 100;
				/* Flag seconds have changed */
				bSecondTimerFired = TRUE;
			}

			/* Flag 100ms timer has fired */
			bAppTimer100msFired = TRUE;
			/* Increment counter */
			u32AppTimer100ms++;
			/* Toggle LED WK1 */
		//	vREG_GpioWrite(REG_GPIO_DOUT, u32REG_GpioRead(REG_GPIO_DOUT) ^ DEVICE_OUTPUT_MASK_WK1);
		}
	}
	#endif
}

/****************************************************************************
 *
 * NAME: Node_u8AnalogueEvent
 *
 * DESCRIPTION:
 * Called when an ADC conversion is completed
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC uint8 Node_u8AnalogueEvent(void)
{
	/* AdcStatus MIB has been built ? */
	#if MK_BLD_MIB_ADC_STATUS
		/* Just return value from ADC MIB */
		return MibAdcStatus_u8Analogue();
	#else
		/* Return invalid value */
		return 0xFF;
	#endif
}

/****************************************************************************
 *
 * NAME: Node_vTick
 *
 * DESCRIPTION:
 * Called every 10ms
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void Node_vTick(void)
{
	/* Not an end device ? */
	#ifndef MK_BLD_NODE_TYPE_END_DEVICE
	{
		/* Using wake timer 1 for 100ms timings ? */
		#if NODE_WAKE_TIMER_100MS
		{
			/* Check wake timer 1 for stalls and recover */
			Node_vCheckWakeTimer1();
		}
		#endif
	}

	#endif

	/* Building for coordinator ? */
	#ifdef MK_BLD_NODE_TYPE_COORDINATOR
	{
		/* Is the authorisation timer running ? */
		if (u32AuthoriseTimer > 0)
		{
			/* Decrement */
			u32AuthoriseTimer--;
			/* Timer expired ? */
			if (u32AuthoriseTimer == 0)
			{
				/* Invalidate the authorisation address (so another device can join) */
				sAuthoriseExtAddr.u32H = 0;
				sAuthoriseExtAddr.u32L = 0;
			}
		}
	}
	#endif

	/* Pass up to device */
	Device_vTick();

	/* Pass tick on to mibs CHECKED */
	#if MK_BLD_MIB_NWK_STATUS
		MibNwkStatus_vTick();
	#endif
	#if MK_BLD_MIB_NWK_TEST
		MibNwkTest_vTick();
	#endif
	/* Pass tick on to mibs CHECK THESE */
	#if MK_BLD_MIB_ADC_STATUS
		#ifndef MK_BLD_NODE_TYPE_END_DEVICE
			MibAdcStatus_vTick();
		#endif
	#endif

	/* Debug LQI ? */
	#if DEBUG_NODE_LQI
	{
		/* Different LQI ? */
		if (u8NodePktLqi != u8LastPktLqi)
		{
			/* Note new value */
			u8NodePktLqi = u8LastPktLqi;
			/* Debug */
			DBG_vPrintf(DEBUG_NODE_LQI, "\n%sNode_vTickLqi(%d, %d)", acDebugIndent, u32TimerSeconds, u8NodePktLqi);
		}
	}
	#endif
}

/****************************************************************************
 *
 * NAME: Node_vAppTimer100ms
 *
 * DESCRIPTION:
 * Called every 100ms always run by application
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void Node_vAppTimer100ms(void)
{
}
/****************************************************************************
 *
 * NAME: Node_vSecond
 *
 * DESCRIPTION:
 * Called whenever the timer seconds value is updated
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void Node_vSecond(void)
{
    if (u32TimerSeconds == 1)
	{
       #if MK_BLD_PREH_STATUS
    	{
           vAHI_DioSetOutput(0,MICS2614_OUTPUT_MASK);     //VO3_EN=0
	       vAHI_DioSetOutput(0,MICS4514_OUTPUT_MASK);     //PREH=0
	       DBG_vPrintf(TRUE,"\n preheating MICS was done");
    	}
       #endif
	}
#if MK_BLD_MIB_H2S_STATUS
{
	 u8PrehH2S++;
    #if MK_BLD_PREH_STATUS
	{

       if (u8PrehH2S == 192)
       {
    	   vAHI_DioSetOutput(0,MQ136_OUTPUT_MASK);        //MQ_EN=0
           bCalibrate = TRUE;
           DBG_vPrintf(TRUE,"\nPreheating MQ136 was done");
       }
       else DBG_vPrintf(TRUE,"\nperiod=%d",u8PrehH2S);
	}
    #endif
}
#endif

       #if MK_BLD_MIB_ADC_STATUS
       /* Start a set of ADC readings */
       MibAdcStatus_vResume();
       #endif
    /***************/
	/* Join timeout configured ? */
	#if DEVICE_JOIN_TIMEOUT
	{
		/* Have we not already timed out and we've not already joined since being powered on ? */
		if (FALSE == bJoinTimeout && bJoined == FALSE)
		{
			/* Should we stop trying to join now and the stack is running ? */
			if (u32TimerSeconds >= DEVICE_JOIN_TIMEOUT && u32StackState == NODE_STACK_STATE_RUNNING)
			{
				/* Debug */
				DBG_vPrintf(DEBUG_NODE_FUNC, "\n%sNode_vSecond { bApi_ResetStack(IDLE) } *****************************************************************", acDebugIndent);
				/* Reset to idle */
				while(FALSE == bApi_ResetStack(FALSE, 0x00));

				+/* Note we've stopped trying to join */
				bJoinTimeout = TRUE;
				/* Return to none stack state (which for an end device will allow sleeping at the AHI level) */
				u32StackState = NODE_STACK_STATE_NONE;
			}
		}
	}
	#endif

	/* Building for coordinator ? */
	#ifdef MK_BLD_NODE_TYPE_COORDINATOR
	{
		/* A minute has passed and the network is up and running as a coordinator ? */
		if (u32TimerSeconds % 30 == 0 && bUp && sMibNwkSecurity.sJipInitData.eDeviceType == E_JIP_DEVICE_COORDINATOR)
		{
			/* Announce the presence of the network to any device that might be in standalone mode */
			eApi_SendNetworkAnnounce();
		}
	}
	#endif

	/* Update stack min address */
	Exception_vUpdateStackMin();

     /* Pass up to device */
	Device_vSecond(u32TimerSeconds);
    bCalibrate = FALSE;
    u32TimerSeconds = 0;
	/* Pass seconds onto mib */
	#if MK_BLD_MIB_NODE_CONTROL
		MibNodeControl_vSecond(u8PrehH2S*60);
	#endif
	#if MK_BLD_MIB_NWK_STATUS
		MibNwkStatus_vSecond(u8PrehH2S*60);
	#endif
	#if MK_BLD_MIB_NWK_SECURITY
		MibNwkSecurity_vSecond(u8PrehH2S*60);
	#endif
}
/****************************************************************************
 *
 * NAME: Node_vJipNotifyChanged
 *
 * DESCRIPTION:
 * Called tp issue trap notifications for MIB variables
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void	Node_vJipNotifyChanged(thJIP_Mib hMib, uint32 *pu32VarFlags, uint32 u32VarMask, uint8 u8VarCount)
{
	/* Are there any variable notifications pending ? */
	if ((*pu32VarFlags & u32VarMask) != 0)
	{
		/* Is the stack running ? */
		if (u32StackState == NODE_STACK_STATE_RUNNING)
		{
			/* Is the network up ? */
			if (bUp)
			{
				uint8 u8VarIdx;

				/* Debug */
				DBG_vPrintf(DEBUG_NODE_FUNC, "\n%sNode_vJipNotifyChanged(%s, 0x%x, 0x%x, %d)", acDebugIndent, ((tsJIP_MibInst *)hMib)->pcName, *pu32VarFlags, u32VarMask, u8VarCount);
				Node_vDebugIndent(DEBUG_NODE_FUNC);

				/* Loop through variables */
				for (u8VarIdx = 0; u8VarIdx < u8VarCount; u8VarIdx++)
				{
					/* Is the flag for this variable set ? */
					if ((*pu32VarFlags & (1 << u8VarIdx)) != 0)
					{
						/* Clear flag */
						*pu32VarFlags &= ~(1 << u8VarIdx);
						/* Debug */
						DBG_vPrintf(DEBUG_NODE_FUNC, "\n%svJIP_NotifyChanged(%s, %d)", acDebugIndent, ((tsJIP_MibInst *)hMib)->pcName, u8VarIdx);
						/* Output notification */
						vJIP_NotifyChanged(hMib, u8VarIdx);
						/* Break loop */
						break;
					}
				}

				/* Debug */
				Node_vDebugOutdent(DEBUG_NODE_FUNC);
			}
		}
	}
}

/****************************************************************************
 *
 * NAME: Node_vCheckWakeTimer1
 *
 * DESCRIPTION:
 * Checks for overflow of wake timer 1 and restarts
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void	Node_vCheckWakeTimer1(void)
{
	/* Using wake timer 1 for 100ms timings ? */
	#if NODE_WAKE_TIMER_100MS
	{
		/* Wake timer 1 is running ? */
		if (u8AHI_WakeTimerStatus() & E_AHI_WAKE_TIMER_MASK_1)
		{
			/* Read current value */
			uint64 u64WakeTimerRead = u64AHI_WakeTimerReadLarge(E_AHI_WAKE_TIMER_1);
			/* Has value wrapped around ? */
			if (u64WakeTimerRead >= DEVICE_WAKE_TIMER_ROLLOVER)
			{
				uint32 u32WakeTimerOverrunTicks;
				/* Restart timer as it probably stalled */
				vAHI_WakeTimerStartLarge(E_AHI_WAKE_TIMER_1, Node_u64WakeTimer1Period());
				/* Calculate the time the wake timer ran for without generating an interrupt (in 32kHz intervals) */
				u64WakeTimerRead = (DEVICE_WAKE_TIMER_MAX - u64WakeTimerRead) + Node_u64WakeTimer1Period();
				/* Calculate number of ticks missed by wake timer */
				u32WakeTimerOverrunTicks = (u64WakeTimerRead * DEVICE_WAKE_TIMER_PERIOD_10MS) / Node_u64WakeTimer1Period();
				/* Update the number of ticks */
				u32TimerTicks += u32WakeTimerOverrunTicks;
				/* Update seconds */
				u32TimerSeconds += u32TimerTicks / 100;
				/* Update count of recoveries */
				u32WakeTimer1Recoveries++;
			}
		}
	}
	#endif
}

PUBLIC void Node_vCheckCalib(uint8 u8Calib)
{
	if (u8Calib == 1)
		bCalibrate = TRUE;
}
/****************************************************************************
 *
 * NAME: Data access functions
 *
 ****************************************************************************/

PUBLIC uint32 Node_u32StackState(void)
{
	return u32StackState;
}

PUBLIC uint32 Node_u32TimerSeconds(void)
{
	return u32TimerSeconds;
}

PUBLIC bool_t Node_bCalibrate(void)
{
	return bCalibrate ;
}

PUBLIC uint8 Node_u8SleepPeriods(void)
{
	return u8PrehH2S ;
}


PUBLIC bool_t Node_bJoined(void)
{
	return bJoined;
}

PUBLIC bool_t Node_bUp(void)
{
	return bUp;
}

PUBLIC uint64 Node_u64WakeTimer1Period(void)
{
	/* Using wake timer 1 for 100ms timings ? */
	#if NODE_WAKE_TIMER_100MS
	{
		return u64WakeTimer1Period;
	}
	#else
	{
		return 0ULL;
	}
	#endif
}

/****************************************************************************
 *
 * NAME: vJIP_StayAwakeRequest()
 *
 * DESCRIPTION:
 * Request from JIP to stay awake a bit longer
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
#ifdef MK_BLD_NODE_TYPE_END_DEVICE
PUBLIC WEAK void vJIP_StayAwakeRequest(void)
{
	#if (DEVICE_ED_REQ_AWAKE != 0)
	{
		/* Set output line */
		//vAHI_DioSetOutput(DEVICE_OUTPUT_MASK_STAY_AWAKE, 0);
		/* Force resume on a future wake (-1 cos u32AppTimer100ms should already have been incremented) */
		bAppTimer100msResume   = TRUE;
		u32AppTimer100msResume = u32AppTimer100ms + DEVICE_ED_REQ_AWAKE - 1;
	}
	#endif
}
#endif

/****************************************************************************
 *
 * NAME: Node_vSleep
 *
 * DESCRIPTION:
 * Called prior to sleeping
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
#ifdef MK_BLD_NODE_TYPE_END_DEVICE
PUBLIC void Node_vSleep(void)
{
	/* Debug */
	DBG_vPrintf(DEBUG_NODE_FUNC, "\n%sNode_vSleep()", acDebugIndent);
	Node_vDebugIndent(DEBUG_NODE_FUNC);

	/* Set TX LED off */
	//vAHI_DioSetOutput(0, DEVICE_OUTPUT_MASK_TX);

	/* Stack is running ? */
	if (Node_u32StackState() == NODE_STACK_STATE_RUNNING)
	{
		/* UART ? */
		#ifdef UART_H_INCLUDED
		{
			/* Close UART */
			UART_vChar('s');
		}
		#endif
		/* Debug */
		DBG_vPrintf(DEBUG_DEVICE_FUNC, "\n%sv6LP_Sleep(TRUE, %d)        ", acDebugIndent, DEVICE_ED_APP_SLEEP_MS);
		/* Turn off application LED */
		//vAHI_DioSetOutput(0, DEVICE_OUTPUT_MASK_APP);
		/* Request stack to put us to sleep */
		v6LP_Sleep(TRUE, DEVICE_ED_APP_SLEEP_MS);
	}
	/* Stack is not running ? */
	else
	{
		/* Wake timer 0 is running ? */
		if (u8AHI_WakeTimerStatus() & E_AHI_WAKE_TIMER_MASK_0)
		{
			/* Read current value */
			uint64 u64WakeTimerRead = u64AHI_WakeTimerReadLarge(E_AHI_WAKE_TIMER_0);
			/* Has value wrapped around ? */
			if (u64WakeTimerRead >= DEVICE_WAKE_TIMER_ROLLOVER)
			{
				/* Restart timer as it probably stalled */
				vAHI_WakeTimerStartLarge(E_AHI_WAKE_TIMER_0, DEVICE_ED_APP_SLEEP_MS << 6);
				/* Update count of recoveries */
				u32WakeTimer0Recoveries++;
			}
		}
		/* Debug */
		DBG_vPrintf(DEBUG_DEVICE_FUNC, "\n%svAHI_Sleep(OSCON_RAMON)        ", acDebugIndent);
		/* UART ? */
		#ifdef UART_H_INCLUDED
		{
			/* Initialise UART */
			UART_vChar('z');
			UART_vClose();
		}
		#endif

		/* Turn off application LED */
		//vREG_GpioWrite(REG_GPIO_DOUT, u32REG_GpioRead(REG_GPIO_DOUT) | DEVICE_OUTPUT_MASK_APP);
		//vAHI_DioSetOutput(0, DEVICE_OUTPUT_MASK_APP);
		/* Go to sleep */
		vAHI_Sleep(E_AHI_SLEEP_OSCON_RAMON);
	}

	/* Debug */
	Node_vDebugOutdent(DEBUG_NODE_FUNC);
}
#endif

/****************************************************************************
 *
 * NAME: Node_vPreSleepCallback()
 *
 * DESCRIPTION:
 * Called by JIP to indicate about to enter sleep mode
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
#ifdef MK_BLD_NODE_TYPE_END_DEVICE
PUBLIC void Node_vPreSleepCallback(void)
{
	/* Call up to device layer */
	Device_vPreSleepCallback();

	/* Check wake timer 1 for stalls and recover */
	Node_vCheckWakeTimer1();

	/* UART ? */
	#ifdef UART_H_INCLUDED
	{
		/* Close UART */
		UART_vChar('P');
		UART_vClose();
	}
	#endif

	/* Turn off JIP LED */
	//vAHI_DioSetOutput(0, DEVICE_OUTPUT_MASK_JIP);
	/* Clear output line */
	//vAHI_DioSetOutput(0, DEVICE_OUTPUT_MASK_STAY_AWAKE);
}
#endif

/****************************************************************************
 *
 * NAME: Node_vDebugIndent
 *
 * DESCRIPTION:
 *
 ****************************************************************************/
PUBLIC void Node_vDebugIndent(bool_t bStream)
{
	if (bStream)
	{
		/* Output brace with existing indent */
		DBG_vPrintf(bStream, "\n%s{", acDebugIndent);
		/* Increase indentation */
		i32DebugIndent += 4;
		/* Got room to increase string ? */
		if (i32DebugIndent < sizeof(acDebugIndent))
		{
			/* Increase indentation */
			acDebugIndent[i32DebugIndent-4] = ' ';
			acDebugIndent[i32DebugIndent-3] = ' ';
			acDebugIndent[i32DebugIndent-2] = ' ';
			acDebugIndent[i32DebugIndent-1] = ' ';
			acDebugIndent[i32DebugIndent] 	= '\0';
		}
	}
}

/****************************************************************************
 *
 * NAME: Node_vDebugOutdent
 *
 * DESCRIPTION:
 *
 ****************************************************************************/
PUBLIC void Node_vDebugOutdent(bool_t bStream)
{
	if (bStream)
	{
		/* Decrease indentation */
		i32DebugIndent -= 4;
		/* OK to decrease indentation ? */
		if (i32DebugIndent >= 0)
		{
			acDebugIndent[i32DebugIndent] = '\0';
		}
		/* Output brace with new indent */
		DBG_vPrintf(bStream, "\n%s}", acDebugIndent);
	}
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
