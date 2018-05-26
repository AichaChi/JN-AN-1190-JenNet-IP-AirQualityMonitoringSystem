/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         AdcStatus MIB - Implementation
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
#include <recal.h>
/* Stack includes */
#include <Api.h>
#include <AppApi.h>
#include <JIP.h>
#include <6LP.h>
/* JenOS includes */
#include <dbg.h>
#include <dbg_uart.h>
#include <os.h>
#include <pdm.h>
/* Application common includes */
#include "DeviceDefs.h"
#include "Table.h"
#include "Node.h"
#include "MibAdcStatus.h"
#include "MibCOstatus.h"
#include "MibNO2status.h"
#include "MibO3status.h"
#include "MibH2Sstatus.h"
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define MIB_ADC_BITS 	   		  		        (12)
#define MIB_ADC_READ_RANGE	       (1<<MIB_ADC_BITS)
#define MIB_ADC_READ_MAX	  (MIB_ADC_READ_RANGE-1)
#define MIB_ADC_TEMP_CALIBRATE   				200	/* 20C */
#define MIB_ADC_TEMP_OSC_PULL_LO 			    950 /*  95C */
#define MIB_ADC_TEMP_OSC_PUSH_LO			    930	/*  93C */
#define MIB_ADC_TEMP_OSC_PULL_HI 			   1100 /* 110C */
#define MIB_ADC_TEMP_OSC_PUSH_HI			   1080	/* 108C */

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
PRIVATE tsMibAdcStatus *psMibAdcStatus;				 /* Nwk Status Mib data */
/* Set up register pointer */
PRIVATE uint32 *pu32Test3V = ((uint32 *) 0x020000C8);

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: MibAdcStatus_vInit
 *
 * DESCRIPTION:
 * Initialises data
 *
 ****************************************************************************/
PUBLIC void MibAdcStatus_vInit( thJIP_Mib       hMibAdcStatusInit,
								tsMibAdcStatus *psMibAdcStatusInit,
								uint8 			u8AdcMask,
								uint8 			u8Period)
{
	/* Debug */
	DBG_vPrintf(DEBUG_MIB_ADC_STATUS, "\n%sMibAdcStatus_vInit() {%d}", acDebugIndent, sizeof(tsMibAdcStatus));

	/* Valid data pointer ? */
	if (psMibAdcStatusInit != (tsMibAdcStatus *) NULL)
	{
		/* Take copy of pointer to data */
		psMibAdcStatus = psMibAdcStatusInit;

		/* Take a copy of the MIB handle */
		psMibAdcStatus->hMib = hMibAdcStatusInit;

		/* Default temporary status data */
		psMibAdcStatus->sTemp.u8Mask = u8AdcMask;
		/* Begin with minimum possible values so oscillator is pulled is booting at high temperatures */
		psMibAdcStatusInit->i16ChipTemp = CONFIG_INT16_MIN;
		psMibAdcStatusInit->i16CalTemp  = CONFIG_INT16_MIN;

		/* Initialise other data */
		if (u8Period < MIB_ADC_ADCS) u8Period = MIB_ADC_ADCS;
		psMibAdcStatus->u8Period 	= u8Period;
		psMibAdcStatus->u8Interval  = u8Period / MIB_ADC_ADCS;

		/* Assume a starting temperature of 0 */
		psMibAdcStatus->i16CalTemp = 0;
		/* Fully push oscillator - clear bits 20 and 21 of TEST3V register */
		U32_CLR_BITS(pu32Test3V, (3<<20));

		/* Begin (resume) readings */
		//MibAdcStatus_vResume();
	}
}

/****************************************************************************
 *
 * NAME: MibAdcStatus_vRegister
 *
 * DESCRIPTION:
 * Registers MIB
 *
 ****************************************************************************/
PUBLIC void MibAdcStatus_vRegister(void)
{
	teJIP_Status eStatus;

	/* Debug */
	DBG_vPrintf(DEBUG_MIB_ADC_STATUS, "\n%sMibAdcStatus_vRegister()", acDebugIndent);
	Node_vDebugIndent(DEBUG_MIB_ADC_STATUS);
	/* Register MIB */
	eStatus = eJIP_RegisterMib(psMibAdcStatus->hMib);
	/* Debug */
	DBG_vPrintf(DEBUG_MIB_ADC_STATUS, "\n%seJIP_RegisterMib(AdcStatus)=%d", acDebugIndent, eStatus);

	/* Configure table structure */
	psMibAdcStatus->sRead.pvData		= (void *) psMibAdcStatus->sTemp.au16Read;
	psMibAdcStatus->sRead.u32Size		= sizeof(uint16);
	psMibAdcStatus->sRead.u16Entries 	= MIB_ADC_ADCS;

	/* Debug */
	Node_vDebugOutdent(DEBUG_MIB_ADC_STATUS);
}

/****************************************************************************
 *
 * NAME: MibAdcStatus_vTick
 *
 * DESCRIPTION:
 * Timing function
 *
 ****************************************************************************/
PUBLIC void MibAdcStatus_vTick(void)
{
	/* Increment tick counter */
	psMibAdcStatus->u32Tick++;
	/* Time to start a set of readings */
	if ((psMibAdcStatus->u32Tick % psMibAdcStatus->u8Period) == 0)
	{
		/* Start a set of ADC readings */
		MibAdcStatus_vStart();
	}
}

/****************************************************************************
 *
 * NAME: MibAdcStatus_vResume
 *
 * DESCRIPTION:
 * Timing function
 *
 ****************************************************************************/
PUBLIC void MibAdcStatus_vResume(void)
{
	/* Invalidate current reading value */
	psMibAdcStatus->u8Adc = 0xFF;
	/* Start a set of readings */
	MibAdcStatus_vStart();
}

/****************************************************************************
 *
 * NAME: MibAdcStatus_vStart
 *
 * DESCRIPTION:
 * Starts a full set of ADC readings
 *
 ****************************************************************************/
PUBLIC void MibAdcStatus_vStart(void)
{
	/* Mask includes at least one valid source and not already waiting for a reading */
	if (psMibAdcStatus->sTemp.u8Mask < (1<<(E_AHI_ADC_SRC_VOLT+1)) && psMibAdcStatus->u8Adc > E_AHI_ADC_SRC_VOLT)
	{
		/* Debug */
		DBG_vPrintf(DEBUG_MIB_ADC_STATUS, "\n%sMibAdcStatus_vStart()", acDebugIndent);
		Node_vDebugIndent(DEBUG_MIB_ADC_STATUS);

		/* Take a fresh copy of source mask */
		psMibAdcStatus->u8PendingMask = psMibAdcStatus->sTemp.u8Mask;
		/* Debug */
		DBG_vPrintf(DEBUG_MIB_ADC_STATUS, "\n%spsMibAdcStatus->u8PendingMask = 0x%x", acDebugIndent, psMibAdcStatus->u8PendingMask);
		/* Look for next enabled ADC source */
		for (psMibAdcStatus->u8Adc  = E_AHI_ADC_SRC_VOLT;
			 psMibAdcStatus->u8Adc <= E_AHI_ADC_SRC_VOLT && (psMibAdcStatus->u8PendingMask & (1<<psMibAdcStatus->u8Adc)) == 0;
			 psMibAdcStatus->u8Adc--);
		/* Found a valid ADC source ? */
		if (psMibAdcStatus->u8Adc <= E_AHI_ADC_SRC_VOLT)
		{
			/* Analogue peripherals not yet initialised ? */
			if (! bAHI_APRegulatorEnabled())
			{
				/* Initialise the Analogue Peripherals */
				vAHI_ApConfigure(E_AHI_AP_REGULATOR_ENABLE,
							 E_AHI_AP_INT_ENABLE,
							 E_AHI_AP_SAMPLE_8,
							 E_AHI_AP_CLOCKDIV_500KHZ,
							 E_AHI_AP_INTREF);
			}
			/* Enable reading */
			vAHI_AdcEnable(E_AHI_ADC_SINGLE_SHOT, E_AHI_AP_INPUT_RANGE_2, psMibAdcStatus->u8Adc);
			/* Debug */
			DBG_vPrintf(DEBUG_MIB_ADC_STATUS, "\n%svAHI_AdcEnable(%d)", acDebugIndent, psMibAdcStatus->u8Adc);
			DBG_vPrintf(DEBUG_MIB_ADC_STATUS, "\n%svAHI_AdcStartSample()", acDebugIndent);
			Node_vDebugOutdent(DEBUG_MIB_ADC_STATUS);
			/* Start sample */
			vAHI_AdcStartSample();
		}
		else
		{
			/* Debug */
			Node_vDebugOutdent(DEBUG_MIB_ADC_STATUS);
		}
	}
}

/****************************************************************************
 *
 * NAME: MibAdcStatus_u8Analogue
 *
 * DESCRIPTION:
 * Called when analogue reading is complete
 *
 ****************************************************************************/
PUBLIC uint8 MibAdcStatus_u8Analogue(void)
{
	/* Debug */
	DBG_vPrintf(DEBUG_MIB_ADC_STATUS, "\n%sMibAdcStatus_u8Analogue()", acDebugIndent);
	Node_vDebugIndent(DEBUG_MIB_ADC_STATUS);

	/* Note the reading */
	psMibAdcStatus->sTemp.au16Read[psMibAdcStatus->u8Adc] = u16AHI_AdcRead()  & (ADC_10_BIT_MASK);
	/* Left shift the conversion to scale from 10 to 12 bits (number of bits in original JN5148 ADC */
//	psMibAdcStatus->sTemp.au16Read[psMibAdcStatus->u8Adc] <<= 2;
    if (psMibAdcStatus->u8Adc == E_AHI_ADC_SRC_ADC_1) MibNO2status_vAnalogue(psMibAdcStatus->u8Adc);
    else if (psMibAdcStatus->u8Adc == E_AHI_ADC_SRC_ADC_2) MibCOstatus_vAnalogue(psMibAdcStatus->u8Adc);
	else if (psMibAdcStatus->u8Adc == E_AHI_ADC_SRC_ADC_3) MibO3status_vAnalogue(psMibAdcStatus->u8Adc);
	else if (psMibAdcStatus->u8Adc == E_AHI_ADC_SRC_ADC_4) MibH2Sstatus_vAnalogue(psMibAdcStatus->u8Adc);
	/* Debug */
	DBG_vPrintf(DEBUG_MIB_ADC_STATUS, "\n%sau16Read[%d] = %d", acDebugIndent, psMibAdcStatus->u8Adc, psMibAdcStatus->sTemp.au16Read[psMibAdcStatus->u8Adc]);

	/* Update the table hash value */
	psMibAdcStatus->sRead.u16Hash++;

	/* Remove bit from pending adc source mask */
	psMibAdcStatus->u8PendingMask &= ~(1<<psMibAdcStatus->u8Adc);
	/* Debug */
	DBG_vPrintf(DEBUG_MIB_ADC_STATUS, "\n%spsMibAdcStatus->u8PendingMask = 0x%x", acDebugIndent, psMibAdcStatus->u8PendingMask);
	/* Look for next enabled ADC source */
	for (;
		 psMibAdcStatus->u8Adc <= E_AHI_ADC_SRC_VOLT && (psMibAdcStatus->u8PendingMask & (1<<psMibAdcStatus->u8Adc)) == 0;
		 psMibAdcStatus->u8Adc--);
	/* Found a valid ADC source ? */
	if (psMibAdcStatus->u8Adc <= E_AHI_ADC_SRC_VOLT)
	{

//		else if (psMibAdcStatus->u8Adc == E_AHI_ADC_SRC_ADC_3) MibO3status_vAnalogue(psMibAdcStatus->u8Adc);
	//	else if (psMibAdcStatus->u8Adc == E_AHI_ADC_SRC_ADC_4) MibH2Sstatus_vAnalogue(psMibAdcStatus->u8Adc);
		/* Enable reading */
		vAHI_AdcEnable(E_AHI_ADC_SINGLE_SHOT, E_AHI_AP_INPUT_RANGE_2, psMibAdcStatus->u8Adc);
		/* Debug */
		DBG_vPrintf(DEBUG_MIB_ADC_STATUS, "\n%svAHI_AdcEnable(%d)", acDebugIndent, psMibAdcStatus->u8Adc);
		DBG_vPrintf(DEBUG_MIB_ADC_STATUS, "\n%svAHI_AdcStartSample", acDebugIndent);
		Node_vDebugOutdent(DEBUG_MIB_ADC_STATUS);
		/* Start sample */
		vAHI_AdcStartSample();
	}
	else
	{
		/* Debug */
		Node_vDebugOutdent(DEBUG_MIB_ADC_STATUS);
	}

	/* Return the source we just read */
	return psMibAdcStatus->u8Adc;
}

/****************************************************************************
 *
 * NAME: MibAdcStatus_u16Read
 *
 * DESCRIPTION:
 * Returns most recent specified analogue raw reading
 *
 ****************************************************************************/
PUBLIC uint16 MibAdcStatus_u16Read(uint8 u8Adc)
{
	return psMibAdcStatus->sTemp.au16Read[u8Adc];
}

/****************************************************************************
 *
 * NAME: MibAdcStatus_i32Convert
 *
 * DESCRIPTION:
 * Returns most recent specified analogue reading converting it to specified
 * range.
 *
 ****************************************************************************/
PUBLIC int32 MibAdcStatus_i32Convert(uint8 u8Adc, int32 i32Min, int32 i32Max)
{
	int32 i32Return;

	/* Inverted ? */
	if (i32Min > i32Max)
	{
		/* Calculate conversion */
		i32Return = (((MIB_ADC_READ_MAX-(int32)psMibAdcStatus->sTemp.au16Read[u8Adc])*((i32Min-i32Max)+1))/MIB_ADC_READ_RANGE)+i32Max;
	}
	/* Not inverted ? */
	else
	{
		/* Calculate conversion */
		i32Return = (((int32)psMibAdcStatus->sTemp.au16Read[u8Adc]*((i32Max-i32Min)+1))/MIB_ADC_READ_RANGE)+i32Min;
	}


	/* Return reading converted to millivolts */
	return i32Return;
}

/****************************************************************************
 *
 * NAME: MibAdcStatus_i16DeciCentigrade
 *
 * DESCRIPTION:
 * Returns most recent specified analogue reading converted to DeciCentigrade
 * for on chip analogue source.
 *
 ****************************************************************************/
PUBLIC int16 MibAdcStatus_i16DeciCentigrade(uint8 u8Adc)
{
	int16 i16DeciCentigrade;

	i16DeciCentigrade = (int16) ((int32) 250 - ((((int32) psMibAdcStatus->sTemp.au16Read[u8Adc] - (int32) 1244) * (int32) 353) / (int32) 100));

	return i16DeciCentigrade;
}
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
