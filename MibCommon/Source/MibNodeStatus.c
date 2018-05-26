/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         NodeStatus MIB - Implementation
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
#include "MibNodeStatus.h"

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
/***        Local Variables                                               ***/
/****************************************************************************/
PRIVATE tsMibNodeStatus *psMibNodeStatus;		/* Node Status Mib data */

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: MibNodeStatus_vInit
 *
 * DESCRIPTION:
 * Initialises data
 *
 ****************************************************************************/
PUBLIC void MibNodeStatus_vInit(thJIP_Mib         hMibNodeStatusInit,
								tsMibNodeStatus *psMibNodeStatusInit)
{
	/* Valid data pointer ? */
	if (psMibNodeStatusInit != (tsMibNodeStatus *) NULL)
	{
		uint32		 u32SystemStatus;

		/* Debug */
		DBG_vPrintf(DEBUG_MIB_NODE_STATUS, "\n%sMibNodeStatus_vInit() {%d}", acDebugIndent, sizeof(tsMibNodeStatus));
		Node_vDebugIndent(DEBUG_MIB_NODE_STATUS);

		/* Take copy of pointer to data */
		psMibNodeStatus = psMibNodeStatusInit;

		/* Take a copy of the MIB handle */
		psMibNodeStatus->hMib = hMibNodeStatusInit;

		/* Load NodeStatus mib data */
		(void) PDM_eLoadRecord(&psMibNodeStatus->sDesc,
							   (uint16)(MIB_ID_NODE_STATUS & 0xFFFF),
							   (void *) &psMibNodeStatus->sPerm,
							   sizeof(psMibNodeStatus->sPerm),
							   FALSE);
		/* Debug */
		DBG_vPrintf(DEBUG_MIB_NODE_STATUS, "\n%sPDM_eLoadRecord(MibNodeStatus) = %d", acDebugIndent, psMibNodeStatus->sDesc.eState);

		/* Read system status */
		u32SystemStatus = u32REG_SysRead(REG_SYS_STAT);
		/* Copy to mib variable */
		psMibNodeStatus->sPerm.u16SystemStatus = (uint16)(u32SystemStatus & 0xffff);

		/* Cold start ? */
		if (0 == (u32SystemStatus & REG_SYSCTRL_STAT_WUS_MASK))
		{
			/* Increment cold start counter */
			psMibNodeStatus->sPerm.u16ColdStartCount++;
		}

		/* Watchdog reset ? */
		if (u32SystemStatus & REG_SYSCTRL_STAT_WD_RES_MASK)
		{
			/* Debug */
			DBG_vPrintf(DEBUG_MIB_NODE_STATUS, "\n\tWATCHDOG RESET");
			/* Increment watchdog counter */
			psMibNodeStatus->sPerm.u16WatchdogCount++;
		}

		/* Brownout reset ? */
		if (u32SystemStatus & REG_SYSCTRL_STAT_VBO_RES_MASK)
		{
			/* Debug */
			DBG_vPrintf(DEBUG_MIB_NODE_STATUS, "\n\tBROWNOUT RESET");
			/* Increment brownout counter */
			psMibNodeStatus->sPerm.u16BrownoutCount++;
		}
	}

	/* Debug */
	Node_vDebugOutdent(DEBUG_MIB_NODE_STATUS);
}

/****************************************************************************
 *
 * NAME: MibNodeStatus_vRegister
 *
 * DESCRIPTION:
 * Registers MIB
 *
 ****************************************************************************/
PUBLIC void MibNodeStatus_vRegister(void)
{
	teJIP_Status eStatus;

	/* Debug */
	DBG_vPrintf(DEBUG_MIB_NODE_STATUS, "\n%sMibNodeStatus_vRegister()", acDebugIndent);
	Node_vDebugIndent(DEBUG_MIB_NODE_STATUS);

	/* Register MIB */
	eStatus = eJIP_RegisterMib(psMibNodeStatus->hMib);
	DBG_vPrintf(DEBUG_MIB_NODE_STATUS, "\n%seJIP_RegisterMib(NodeStatus) = %d", acDebugIndent, eStatus);

	/* Make sure permament data is saved */
	PDM_vSaveRecord(&psMibNodeStatus->sDesc);
	/* Debug */
	DBG_vPrintf(DEBUG_MIB_NODE_STATUS, "\n%sPDM_vSaveRecord(MibNodeStatus) = %d", acDebugIndent, psMibNodeStatus->sDesc.eState);
	/* Debug */
	Node_vDebugOutdent(DEBUG_MIB_NODE_STATUS);
}

/****************************************************************************
 *
 * NAME: MibNodeStatus_vIncrementResetCount
 *
 * DESCRIPTION:
 * Called to increment reset counter due to a deliberate reset
 *
 ****************************************************************************/
PUBLIC void MibNodeStatus_vIncrementResetCount(void)
{
	/* Increment counter */
	psMibNodeStatus->sPerm.u16ResetCount++;
	/* Make sure permament data is saved */
	PDM_vSaveRecord(&psMibNodeStatus->sDesc);
	/* Debug */
	DBG_vPrintf(DEBUG_MIB_NODE_STATUS, "\n%sMibNodeStatus_vIncrementResetCount { PDM_vSaveRecord(MibNodeStatus) = %d }", psMibNodeStatus->sDesc.eState);
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
