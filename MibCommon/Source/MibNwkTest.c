/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         NwkTest MIB - Implementation
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
#include "MibNwkTest.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

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
extern PUBLIC uint8  u8LastPktLqi;

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
PRIVATE tsMibNwkTest *psMibNwkTest;			/* Nwk Status Mib data */

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: MibNwkTest_vInit
 *
 * DESCRIPTION:
 * Initialises data
 *
 ****************************************************************************/
PUBLIC void MibNwkTest_vInit(thJIP_Mib        hMibNwkTestInit,
							   tsMibNwkTest  *psMibNwkTestInit)
{
	/* Valid data pointer ? */
	if (psMibNwkTestInit != (tsMibNwkTest *) NULL)
	{
		/* Debug */
		DBG_vPrintf(DEBUG_MIB_NWK_TEST, "\n%sMibNwkTest_vInit()", acDebugIndent);

		/* Take copy of pointer to data */
		psMibNwkTest = psMibNwkTestInit;

		/* Take a copy of the MIB handle */
		psMibNwkTest->hMib = hMibNwkTestInit;
	}
}

/****************************************************************************
 *
 * NAME: MibNwkTest_vRegister
 *
 * DESCRIPTION:
 * Registers MIB
 *
 ****************************************************************************/
PUBLIC void MibNwkTest_vRegister(void)
{
	teJIP_Status eStatus;

	/* Debug */
	DBG_vPrintf(DEBUG_MIB_NWK_TEST, "\n%sMibNwkTest_vRegister()", acDebugIndent);
	/* Register MIB */
	eStatus = eJIP_RegisterMib(psMibNwkTest->hMib);
	/* Debug */
	DBG_vPrintf(DEBUG_MIB_NWK_TEST, " { eJIP_RegisterMib(NwkTest) = %d }", eStatus);
}

/****************************************************************************
 *
 * NAME: MibNwkTest_vTick
 *
 * DESCRIPTION:
 * Timer function
 *
 ****************************************************************************/
PUBLIC void MibNwkTest_vTick(void)
{
	void            *pvMac;
	MAC_Pib_s       *psPib;

	/* Test is pending ? */
	if (psMibNwkTest->u8Pending > 0)
	{
		psMibNwkTest->u8Pending--;
		/* Reached 0 ? */
		if (psMibNwkTest->u8Pending == 0 && psMibNwkTest->sTemp.u8Tests == 0)
		{
			/* Test was running ? */
			if (psMibNwkTest->bRun)
			{
				/* No longer running */
				psMibNwkTest->bRun = FALSE;

				/* Debug */
				DBG_vPrintf(DEBUG_MIB_NWK_TEST, "\n%sMibNwkTest_vTick() COMPLETE", acDebugIndent);
				Node_vDebugIndent(DEBUG_MIB_NWK_TEST);

				/* Get MAC and PIB pointers */
				pvMac = pvAppApiGetMacHandle();
				psPib = MAC_psPibGetHandle(pvMac);
				/* Restore default MAC retries */
				psPib->u8MaxFrameRetries = psMibNwkTest->u8DefaultMacRetries;

				/* Restore original run profile */
				(void) bJnc_SetRunProfile(psMibNwkTest->u8RunProfile, &psMibNwkTest->sRunProfile);

				/* Debug */
				DBG_vPrintf(DEBUG_MIB_NWK_TEST, "\n%sTxReq=%d TxOk=%d RxOk=%d RxLqiMin=%d RxLqiMax=%d RxLqiMean=%d TxLqiMin=%d TxLqiMax=%d TxLqiMean=%d",
					acDebugIndent,
					psMibNwkTest->sTemp.u8TxReq,
					psMibNwkTest->sTemp.u8TxOk,
					psMibNwkTest->sTemp.u8RxOk,
					psMibNwkTest->sTemp.u8RxLqiMin,
					psMibNwkTest->sTemp.u8RxLqiMax,
					psMibNwkTest->sTemp.u8RxLqiMean,
					psMibNwkTest->sTemp.u8TxLqiMin,
					psMibNwkTest->sTemp.u8TxLqiMax,
					psMibNwkTest->sTemp.u8TxLqiMean);
				/* Debug */
				Node_vDebugOutdent(DEBUG_MIB_NWK_TEST);
			}
		}
	}
	/* Test is not pending ? */
	else
	{
		/* Stack is running and network is up ? */
		if (Node_u32StackState() == NODE_STACK_STATE_RUNNING && psMibNwkTest->bUp)
		{
			/* More tests to run ? */
			if (psMibNwkTest->sTemp.u8Tests > 0)
			{
				teJIP_Status eStatus;

				/* Test not running ? */
				if (psMibNwkTest->bRun == FALSE)
				{
					tsNwkProfile sNwkProfile;

					/* Now running */
					psMibNwkTest->bRun = TRUE;

					/* Debug */
					DBG_vPrintf(DEBUG_MIB_NWK_TEST, "\n%sMibNwkTest_vTick() BEGIN", acDebugIndent);

					/* Read running network profile to restore when test completes */
					psMibNwkTest->u8RunProfile = u8GetCurRunProfile();
					vJnc_GetNwkProfile(&psMibNwkTest->sRunProfile);
					/* Read running network profile for manipulation */
					vJnc_GetNwkProfile(&sNwkProfile);
					/* Disable max failed packets */
					sNwkProfile.u8MaxFailedPkts = 0;
					/* Apply as user profile */
					(void) bJnc_SetRunProfile(PROFILE_USER, &sNwkProfile);

					/* Get MAC and PIB pointers */
					pvMac = pvAppApiGetMacHandle();
					psPib = MAC_psPibGetHandle(pvMac);
					/* Note default MAC retries */
					psMibNwkTest->u8DefaultMacRetries = psPib->u8MaxFrameRetries;
					/* Override default MAC retries */
					psPib->u8MaxFrameRetries = psMibNwkTest->sTemp.u8MacRetries;
				}

				/* Issue remote variable read request */
				eStatus = eJIP_Remote_Mib_Get(&psMibNwkTest->s6LP_SockAddr,
											   psMibNwkTest->sTemp.u8Tests,
											   MIB_ID_NWK_TEST,			/* NwkTest MIB */
											   VAR_IX_NWK_TEST_RX_LQI);	/* RxLqi (last received) Variable */
				/* Successful request */
				if (E_JIP_OK == eStatus) psMibNwkTest->sTemp.u8TxReq++;
				/* Set pending counter */
				psMibNwkTest->u8Pending = 10;
				/* Decrement count */
				psMibNwkTest->sTemp.u8Tests--;
			}
		}
	}
}

/****************************************************************************
 *
 * NAME: MibNwkTest_vStackEvent
 *
 * DESCRIPTION:
 * Called when stack events take place
 *
 ****************************************************************************/
PUBLIC void MibNwkTest_vStackEvent(te6LP_StackEvent eEvent, void *pvData, uint8 u8DataLen)
{
	/* Network is up and we think it is down ? */
	if (E_STACK_JOINED  == eEvent ||
		E_STACK_STARTED == eEvent)
	{
		/* Was network down previously ? */
		if (FALSE == psMibNwkTest->bUp)
		{
			tsNwkInfo *psNwkInfo;
			EUI64_s     sIntAddr;

			/* Debug */
			DBG_vPrintf(DEBUG_MIB_NWK_TEST, "\n%sMibNwkTest_vStackEvent(JOINED)", acDebugIndent);
			/* Set up flag */
			psMibNwkTest->bUp = TRUE;
			psNwkInfo = (tsNwkInfo *) pvData;
			/* Build 6LP address for testing from parent's address */
			i6LP_CreateInterfaceIdFrom64(&sIntAddr, (EUI64_s *) &psNwkInfo->sParentAddr);
			i6LP_CreateLinkLocalAddress (&psMibNwkTest->s6LP_SockAddr.sin6_addr, &sIntAddr);
			/* Complete full socket address */
			psMibNwkTest->s6LP_SockAddr.sin6_family = E_6LP_PF_INET6;
			psMibNwkTest->s6LP_SockAddr.sin6_flowinfo =0;
			psMibNwkTest->s6LP_SockAddr.sin6_port = JIP_DEFAULT_PORT;
			psMibNwkTest->s6LP_SockAddr.sin6_scope_id =0;
		}
	}
	/* Network is down and we think it is up ? */
	if (E_STACK_RESET == eEvent)
	{
		/* Was network up previously ? */
		if (TRUE == psMibNwkTest->bUp)
		{
			/* Debug */
			DBG_vPrintf(DEBUG_MIB_NWK_TEST, "\n%sMibNwkTest_vStackEvent(STACK_RESET)", acDebugIndent);
			/* Clear up flag */
			psMibNwkTest->bUp = FALSE;
			/* Test was running ? */
			if (psMibNwkTest->bRun)
			{
				void            *pvMac;
				MAC_Pib_s       *psPib;

				/* No longer running */
				psMibNwkTest->bRun = FALSE;

				/* Debug */
				DBG_vPrintf(DEBUG_MIB_NWK_TEST, " ABORT");

				/* Clear test count to abort test */
				psMibNwkTest->sTemp.u8Tests = 0;
				psMibNwkTest->u8Pending = 0;

				/* Get MAC and PIB pointers */
				pvMac = pvAppApiGetMacHandle();
				psPib = MAC_psPibGetHandle(pvMac);
				/* Restore default MAC retries */
				psPib->u8MaxFrameRetries = psMibNwkTest->u8DefaultMacRetries;

				/* Restore original run profile */
				(void) bJnc_SetRunProfile(psMibNwkTest->u8RunProfile, &psMibNwkTest->sRunProfile);
			}
		}
	}
}

/****************************************************************************
 *
 * NAME: MibNwkTest_eSetTests
 *
 * DESCRIPTION:
 * Handle remote set of a key
 *
 ****************************************************************************/
PUBLIC teJIP_Status MibNwkTest_eSetTests(uint8 u8Val, void *pvCbData)
{
	bool_t bReturn = FALSE;

	/* Zero previously recorded data */
	psMibNwkTest->sTemp.u8TxReq     = 0;
	psMibNwkTest->sTemp.u8TxOk      = 255;	   /* There is an extra reading */
	psMibNwkTest->sTemp.u8RxOk      = 0;
	psMibNwkTest->sTemp.u8RxLqiMin  = 255;
	psMibNwkTest->sTemp.u8RxLqiMax  = 0;
	psMibNwkTest->sTemp.u8RxLqiMean = 0;
	psMibNwkTest->sTemp.u8TxLqiMin  = 255;
	psMibNwkTest->sTemp.u8TxLqiMax  = 0;
	psMibNwkTest->sTemp.u8TxLqiMean = 0;
	psMibNwkTest->u16RxLqiTotal     = 0;
	psMibNwkTest->u16TxLqiTotal     = 0;
	psMibNwkTest->u8Pending         = 0;
	/* Copy data */
	memcpy(pvCbData, &u8Val, sizeof(uint8));
	/* Success */
	bReturn = TRUE;

	return bReturn;
}

PUBLIC WEAK void vJIP_Remote_DataSent(ts6LP_SockAddr *psAddr,
									   teJIP_Status eStatus)
{
	/* Command is pending ? */
	if (psMibNwkTest->bRun && eStatus == E_JIP_OK)
	{
		/* Increment counter */
		psMibNwkTest->sTemp.u8TxOk++;
	}
}

PUBLIC WEAK void vJIP_Remote_GetResponse(ts6LP_SockAddr *psAddr,
        uint8 u8Handle,
        uint8 u8MibIndex,
        uint8 u8VarIndex,
        teJIP_Status eStatus,
        teJIP_VarType eVarType,
        const void *pvVal,
        uint32 u32ValSize)
{
	/* Command is pending ? */
	if (psMibNwkTest->bRun)
	{
		/* Was this a repsonse for the variable we requested ? */
		if (u8VarIndex == VAR_IX_NWK_TEST_RX_LQI)
		{
			/* Increment counter */
			psMibNwkTest->sTemp.u8RxOk++;
			/* Increment running LQI total */
			psMibNwkTest->u16RxLqiTotal += u8LastPktLqi;
			/* Calculate mean */
			psMibNwkTest->sTemp.u8RxLqiMean = (uint8)(psMibNwkTest->u16RxLqiTotal / (uint16) psMibNwkTest->sTemp.u8RxOk);
			/* Update min and max */
			if (u8LastPktLqi > psMibNwkTest->sTemp.u8RxLqiMax) psMibNwkTest->sTemp.u8RxLqiMax = u8LastPktLqi;
			if (u8LastPktLqi < psMibNwkTest->sTemp.u8RxLqiMin) psMibNwkTest->sTemp.u8RxLqiMin = u8LastPktLqi;
			/* Was request successful ? */
			if (eStatus == E_JIP_OK)
			{
				uint8 u8TxLqi;

				/* Extract value from repsonse */
				u8TxLqi = *((uint8 *) pvVal);
				/* Increment running LQI total */
				psMibNwkTest->u16TxLqiTotal += u8TxLqi;
				/* Calculate mean (against received OK packets) */
				psMibNwkTest->sTemp.u8TxLqiMean = (uint8)(psMibNwkTest->u16TxLqiTotal / (uint16) psMibNwkTest->sTemp.u8RxOk);
				/* Update min and max */
				if (u8TxLqi > psMibNwkTest->sTemp.u8TxLqiMax) psMibNwkTest->sTemp.u8TxLqiMax = u8TxLqi;
				if (u8TxLqi < psMibNwkTest->sTemp.u8TxLqiMin) psMibNwkTest->sTemp.u8TxLqiMin = u8TxLqi;
			}
		}
	}
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
