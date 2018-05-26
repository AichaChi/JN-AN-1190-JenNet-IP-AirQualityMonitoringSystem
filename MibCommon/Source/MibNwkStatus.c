/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         NwkStatus MIB - Implementation
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
/* JenOS includes */
#include <dbg.h>
#include <dbg_uart.h>
#include <os.h>
#include <pdm.h>
/* Application common includes */
#include "DeviceDefs.h"
#include "Node.h"
#include "MibCommon.h"
#include "MibNwkStatus.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define STACK_MODE_STANDALONE  0x0001
#define STACK_MODE_COMMISSION  0x0002

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
PRIVATE tsMibNwkStatus *psMibNwkStatus;			/* Nwk Status Mib data */

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: MibNwkStatus_vInit
 *
 * DESCRIPTION:
 * Initialises data
 *
 ****************************************************************************/
PUBLIC void MibNwkStatus_vInit(thJIP_Mib        hMibNwkStatusInit,
							   tsMibNwkStatus  *psMibNwkStatusInit,
								bool_t           bMibNwkStatusSecurity)
{
	/* Valid data pointer ? */
	if (psMibNwkStatusInit != (tsMibNwkStatus *) NULL)
	{
		/* Debug */
		DBG_vPrintf(DEBUG_MIB_NWK_STATUS, "\n%sMibNwkStatus_vInit(%d) {%d}",
			acDebugIndent,
			bMibNwkStatusSecurity,
			sizeof(tsMibNwkStatus));
		Node_vDebugIndent(DEBUG_MIB_NWK_STATUS);

		/* Take copy of pointer to data */
		psMibNwkStatus = psMibNwkStatusInit;

		/* Take a copy of the MIB handle */
		psMibNwkStatus->hMib = hMibNwkStatusInit;

		/* Note security setting */
		psMibNwkStatus->bSecurity = bMibNwkStatusSecurity;

		/* Load NwkStatus mib data */
		(void) PDM_eLoadRecord(&psMibNwkStatus->sDesc,
							   (uint16)(MIB_ID_NWK_STATUS & 0xFFFF),
							   (void *) &psMibNwkStatus->sPerm,
							   sizeof(psMibNwkStatus->sPerm),
							   FALSE);
		/* Debug */
		DBG_vPrintf(DEBUG_MIB_NWK_STATUS, "\n%sPDM_eLoadRecord(MibNwkStatus) = %d", acDebugIndent, psMibNwkStatus->sDesc.eState);

		/* Ensure runtime is set correctly */
		psMibNwkStatus->sTemp.u32RunTime = psMibNwkStatus->sPerm.u32DownTime + psMibNwkStatus->sPerm.u32UpTime;
		/* Note the value of the last saved run time */
		psMibNwkStatus->u32SavedRunTime = psMibNwkStatus->sTemp.u32RunTime;
		/* Note the value of the last saved frame counter */
		psMibNwkStatus->u32SavedFrameCounter = psMibNwkStatus->sPerm.u32FrameCounter;

		/* Advance our frame counter */
		psMibNwkStatus->sPerm.u32FrameCounter += CONFIG_FRAME_COUNTER_DELTA;
	}

	/* Debug */
	Node_vDebugOutdent(DEBUG_MIB_NWK_STATUS);
}

/****************************************************************************
 *
 * NAME: MibNwkStatus_vRegister
 *
 * DESCRIPTION:
 * Registers MIB
 *
 ****************************************************************************/
PUBLIC void MibNwkStatus_vRegister(void)
{
	teJIP_Status eStatus;

	/* Debug */
	DBG_vPrintf(DEBUG_MIB_NWK_STATUS, "\n%sMibNwkStatus_vRegister()", acDebugIndent);
	/* Register MIB */
	eStatus = eJIP_RegisterMib(psMibNwkStatus->hMib);
	DBG_vPrintf(DEBUG_MIB_NWK_STATUS, " { eJIP_RegisterMib(NwkStatus) = %d }", eStatus);

	/* Make sure permament data is saved */
	psMibNwkStatus->bSaveRecord = TRUE;
}

/****************************************************************************
 *
 * NAME: MibNwkStatus_vTick
 *
 * DESCRIPTION:
 * Tick function called every 10ms when stack is running
 *
 ****************************************************************************/
PUBLIC void MibNwkStatus_vTick(void)
{
	/* Are there any variable notifications pending ? */
	if ((psMibNwkStatus->u32NotifyChanged & VAR_MASK_NWK_STATUS) != 0)
	{
		/* Use common function to output notifications */
		Node_vJipNotifyChanged(psMibNwkStatus->hMib, &psMibNwkStatus->u32NotifyChanged, VAR_MASK_NWK_STATUS, VAR_COUNT_NWK_STATUS);
	}
}

/****************************************************************************
 *
 * NAME: MibNwkStatus_vSecond
 *
 * DESCRIPTION:
 * Timer function
 *
 ****************************************************************************/
PUBLIC void MibNwkStatus_vSecond(uint32 u32TimerSeconds)
{
	MAC_Pib_s  *psPib;
	uint32     u32Seconds;

	/* Calculate number of seconds since this function was last called */
	u32Seconds = u32TimerSeconds - psMibNwkStatus->u32NodeTimerSeconds;
	/* Note number of seconds at this call */
	psMibNwkStatus->u32NodeTimerSeconds = u32TimerSeconds;

	/* Get pointer to pib */
	psPib = MAC_psPibGetHandle(pvAppApiGetMacHandle());

	/* Security enabled ? */
	if (psMibNwkStatus->bSecurity)
	{
		/* Has our frame counter advanced since we last saved it ? */
		if (psPib->u32MacFrameCounter > psMibNwkStatus->u32SavedFrameCounter)
		{
			/* Has it advanced far enough to be worth saving ? */
			if (psPib->u32MacFrameCounter - psMibNwkStatus->u32SavedFrameCounter >= CONFIG_FRAME_COUNTER_DELTA)
			{
				/* Save data to flash */
				psMibNwkStatus->bSaveRecord = TRUE;
			}
		}
	}

	/* Update run time */
	psMibNwkStatus->sTemp.u32RunTime += u32Seconds;
	/* Been over an hour since we last saved ? */
	if (psMibNwkStatus->sTemp.u32RunTime - psMibNwkStatus->u32SavedRunTime > 3600)
	{
		/* Save data to flash */
		psMibNwkStatus->bSaveRecord = TRUE;
		/* Need to notify for the run time variable */
		psMibNwkStatus->u32NotifyChanged |= (1 <<VAR_IX_NWK_STATUS_RUN_TIME);
	}

	/* Network is up ? */
	if (psMibNwkStatus->bUp)
	{
		/* Update up timer */
		psMibNwkStatus->sPerm.u32UpTime += u32Seconds;
		/* Been up an hour ? */
		if ((psMibNwkStatus->sPerm.u32UpTime % 3600) == 0)
		{
			/* Need to notify for the up time variable */
			psMibNwkStatus->u32NotifyChanged |= (1 <<VAR_IX_NWK_STATUS_UP_TIME);
		}
	}
	/* Network is down - increment down timer */
	else
	{
		/* Update down timer */
		psMibNwkStatus->sPerm.u32DownTime += u32Seconds;
		/* Been down an hour ? */
		if ((psMibNwkStatus->sPerm.u32DownTime % 3600) == 0)
		{
			/* Need to notify for the down time variable */
			psMibNwkStatus->u32NotifyChanged |= (1 <<VAR_IX_NWK_STATUS_DOWN_TIME);
		}
	}

	/* Save record (if necessary) */
	MibNwkStatus_vSaveRecord();
}

/****************************************************************************
 *
 * NAME: MibNwkStatus_vStackEvent
 *
 * DESCRIPTION:
 * Called when stack events take place
 *
 ****************************************************************************/
PUBLIC void MibNwkStatus_vStackEvent(te6LP_StackEvent eEvent)
{
	/* Network is up ? */
	if (E_STACK_JOINED  == eEvent ||
	    E_STACK_STARTED == eEvent)
	{
		/* Was network down previously ? */
		if (FALSE == psMibNwkStatus->bUp)
		{
			/* Debug */
			DBG_vPrintf(DEBUG_MIB_NWK_STATUS, "\n%sMibNwkStatus_vStackEvent(JOINED)", acDebugIndent);
			Node_vDebugIndent(DEBUG_MIB_NWK_STATUS);

			/* Set up flag */
			psMibNwkStatus->bUp = TRUE;
			/* Increment up counters */
			if (psMibNwkStatus->sPerm.u16UpCount < 0xffff) psMibNwkStatus->sPerm.u16UpCount++;
			/* Not in standalone mode ? */
			if ((u16Api_GetStackMode() & STACK_MODE_STANDALONE) == 0)
			{
				tsNwkProfile sProfile;

				/* Get network profile ? */
				vJnc_GetNwkProfile(&sProfile);
				/* Debug */
				DBG_vPrintf(DEBUG_MIB_NWK_STATUS, "\n%ssProfile.u16RouterPingPeriod = %d", acDebugIndent, sProfile.u16RouterPingPeriod);
				/* Not configured to ping ? */
				if (0 == sProfile.u16RouterPingPeriod)
				{
					/* ASsume we entered standalone mode via fast commissioning and set stack mode accordingly */
					vApi_SetStackMode(STACK_MODE_STANDALONE);
					DBG_vPrintf(DEBUG_MIB_NWK_STATUS, "\n%svApi_SetStackMode(STACK_MODE_STANDALONE)", acDebugIndent);
				}
			}
			/* Have we joined in standalone mode ? */
			if (u16Api_GetStackMode() & STACK_MODE_STANDALONE)
			{
				/* Ok to update to standalone mode (not allowed to drop back to standalone from gateway mode) ? */
				if (psMibNwkStatus->sPerm.u8UpMode < MIB_NWK_STATUS_UP_MODE_STANDALONE)
				{
					/* Note that we are in a standalone network */
					psMibNwkStatus->sPerm.u8UpMode = MIB_NWK_STATUS_UP_MODE_STANDALONE;
				}
			}
			/* Must be in gateway mode ? */
			else
			{
				/* OK to update to gateway mode ? */
				if (psMibNwkStatus->sPerm.u8UpMode < MIB_NWK_STATUS_UP_MODE_GATEWAY)
				{
					/* Note that we are in a standalone network */
					psMibNwkStatus->sPerm.u8UpMode = MIB_NWK_STATUS_UP_MODE_GATEWAY;
				}
			}
			/* Save data to flash */
			psMibNwkStatus->bSaveRecord = TRUE;
			/* Need to notify for the run time variable */
			psMibNwkStatus->u32NotifyChanged |= (1 <<VAR_IX_NWK_STATUS_RUN_TIME);
			/* Need to notify for the up count variable */
			psMibNwkStatus->u32NotifyChanged |= (1 <<VAR_IX_NWK_STATUS_UP_COUNT);
			/* Need to notify for the up time variable */
			psMibNwkStatus->u32NotifyChanged |= (1 <<VAR_IX_NWK_STATUS_UP_TIME);
			/* Need to notify for the down time variable */
			psMibNwkStatus->u32NotifyChanged |= (1 <<VAR_IX_NWK_STATUS_DOWN_TIME);

			/* Debug */
			Node_vDebugOutdent(DEBUG_MIB_NWK_STATUS);
		}
	}
	/* Network is down ? */
	else if (E_STACK_RESET == eEvent)
	{
		/* Was network up previously ? */
 		if (TRUE == psMibNwkStatus->bUp)
 		{
			/* Debug */
			DBG_vPrintf(DEBUG_MIB_NWK_STATUS, "\n%sMibNwkStatus_vStackEvent(STACK_RESET)", acDebugIndent);
			/* Clear up flag */
			psMibNwkStatus->bUp = FALSE;
			/* Save data to flash */
			psMibNwkStatus->bSaveRecord = TRUE;
		}
	}
}

/****************************************************************************
 *
 * NAME: MibNwkStatus_vSaveRecord
 *
 * DESCRIPTION:
 * Timer function
 *
 ****************************************************************************/
PUBLIC void MibNwkStatus_vSaveRecord(void)
{
	/* Need to save record ? */
	if (psMibNwkStatus->bSaveRecord)
	{
		MAC_Pib_s       *psPib;

		/* Clear flag */
		psMibNwkStatus->bSaveRecord = FALSE;
		/* Get a PIB pointer */
		psPib = MAC_psPibGetHandle(pvAppApiGetMacHandle());
		/* Take a copy of the frame counter */
		psMibNwkStatus->sPerm.u32FrameCounter = psPib->u32MacFrameCounter;
		/* Note saved frame counters */
		psMibNwkStatus->u32SavedFrameCounter = psMibNwkStatus->sPerm.u32FrameCounter;
		/* Note saved runtime */
		psMibNwkStatus->u32SavedRunTime = psMibNwkStatus->sTemp.u32RunTime;
		/* Make sure permament data is saved */
		PDM_vSaveRecord(&psMibNwkStatus->sDesc);
		/* Debug */
		DBG_vPrintf(DEBUG_MIB_NWK_STATUS, "\n%sMibNwkStatus_vSaveRecord()", acDebugIndent);
		DBG_vPrintf(DEBUG_MIB_NWK_STATUS, " { PDM_vSaveRecord(MibNwkStatus) = %d }", psMibNwkStatus->sDesc.eState);
	}
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
