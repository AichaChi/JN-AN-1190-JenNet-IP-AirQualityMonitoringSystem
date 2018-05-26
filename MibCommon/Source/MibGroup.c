/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         Group MIB - Implementation
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
#include <JIP.h>
/* JenOS includes */
#include <dbg.h>
#include <dbg_uart.h>
#include <os.h>
#include <pdm.h>
/* Application common includes */
#include "DeviceDefs.h"
#include "Node.h"
#include "Address.h"
#include "MibCommon.h"
#include "MibGroup.h"

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
/***        Imported Variables                                            ***/
/****************************************************************************/
extern uint8 u8SocketMaxGroupAddrs;	   /* Maximum groups variable for stack */

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
PRIVATE tsMibGroup	    *psMibGroup;		    /* Permament stack node mib */

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: MibGroup_vInit
 *
 * DESCRIPTION:
 * Initialises data
 *
 ****************************************************************************/
PUBLIC void MibGroup_vInit(tsMibGroup	    *psMibGroupInit)
{
	/* Debug */
	DBG_vPrintf(DEBUG_MIB_GROUP, "\n%sMibGroup_vInit() {%d}", acDebugIndent, sizeof(tsMibGroup));
	Node_vDebugIndent(DEBUG_MIB_GROUP);

	/* Take copy of pointer to data */
	psMibGroup = psMibGroupInit;

	/* Load Node Status mib data */
	(void) PDM_eLoadRecord(&psMibGroup->sDesc,
						   (uint16)(MIB_ID_GROUPS & 0xFFFF),
						   (void *) &psMibGroup->sPerm,
						   sizeof(psMibGroup->sPerm),
						   FALSE);
	/* Note the recovery state at initialisation */
	psMibGroup->ePdmStateInit = psMibGroup->sDesc.eState;
	/* Debug */
	DBG_vPrintf(DEBUG_MIB_GROUP, "\n%sPDM_eLoadRecord(MibGroup) = %d", acDebugIndent, psMibGroup->sDesc.eState);

	/* Failed to recover PDM data ? */
	if (psMibGroup->sDesc.eState != PDM_RECOVERY_STATE_RECOVERED)
	{
		in6_addr sAddr;

		/* Build "All Devices" group address */
		Address_vBuildGroup(&sAddr, NULL, 0xf00f);
		/* Make a dummy call to get it in the group address array */
		bJIP_GroupCallback(E_JIP_GROUP_JOIN, &sAddr);
	}

	/* Tell stack maximum number of groups we want to support */
	u8SocketMaxGroupAddrs = MIB_GROUP_MAX;

	/* Debug */
	Node_vDebugOutdent(DEBUG_MIB_GROUP);
}

/****************************************************************************
 *
 * NAME: MibGroup_vRestore
 *
 * DESCRIPTION:
 * Restore groups following a power cycle
 *
 ****************************************************************************/
PUBLIC void MibGroup_vRestore(void)
{
	/* Not already restored ? */
	if (psMibGroup->bRestored == FALSE)
	{
		uint8 u8Group;

		/* Loop through current groups */
		for (u8Group = 0; u8Group < MIB_GROUP_MAX; u8Group++)
		{
			/* Valid group ? */
			if (memcmp(&psMibGroup->sPerm.asGroupAddr[u8Group], &psMibGroup->sIn6AddrBlank, sizeof(in6_addr)) != 0)
			{
				/* Try to join the group */
				psMibGroup->bRestored = bJIP_AddGroupAddr(&psMibGroup->sPerm.asGroupAddr[u8Group]);
			}
		}
	}
}

/****************************************************************************
 *
 * NAME: MibGroup_ePdmStateInit
 *
 * DESCRIPTION:
 * Returns the PDM recovery state at initialisation
 *
 ****************************************************************************/
PUBLIC PDM_teRecoveryState MibGroup_ePdmStateInit(void)
{
	return psMibGroup->ePdmStateInit;
}

/****************************************************************************/
/***        Callback Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: bJIP_GroupCallback
 *
 * DESCRIPTION:
 * Callback when stack's Node Mib Name Variable is updated by JIP
 *
 ****************************************************************************/
PUBLIC WEAK bool_t bJIP_GroupCallback(teJIP_GroupEvent eEvent, in6_addr *psAddr)
{
	uint8 u8Group;
	uint8 u8Unused = 0xff;
	uint8 u8Found  = 0xff;
	bool_t bReturn = FALSE;

	/* Debug */
	DBG_vPrintf(DEBUG_MIB_GROUP, "\n%sbJIP_GroupCallback(%d, %x:%x:%x:%x:%x:%x:%x:%x)",
		acDebugIndent,
		eEvent,
		psAddr->s6_addr16[0],
		psAddr->s6_addr16[1],
		psAddr->s6_addr16[2],
		psAddr->s6_addr16[3],
		psAddr->s6_addr16[4],
		psAddr->s6_addr16[5],
		psAddr->s6_addr16[6],
		psAddr->s6_addr16[7]);
	Node_vDebugIndent(DEBUG_MIB_GROUP);

	/* Valid group ? */
	if (memcmp(psAddr, &psMibGroup->sIn6AddrBlank, sizeof(in6_addr)) != 0)
	{
		/* Loop through current groups */
		for (u8Group = 0; u8Group < MIB_GROUP_MAX; u8Group++)
		{
			/* Is this the group we are looking for ? */
			if (memcmp(&psMibGroup->sPerm.asGroupAddr[u8Group], psAddr, sizeof(in6_addr)) == 0)
			{
				/* Note the index */
				if (u8Found == 0xff) u8Found = u8Group;
			}
			/* Is this an unused group ? */
			if (memcmp(&psMibGroup->sPerm.asGroupAddr[u8Group], &psMibGroup->sIn6AddrBlank, sizeof(in6_addr)) == 0)
			{
				/* Note the index */
				if (u8Unused == 0xff) u8Unused = u8Group;
			}
		}

		/* Joining a group ? */
		if (E_JIP_GROUP_JOIN == eEvent)
		{
			/* Not already in the group and we have an unused slot to store it ? */
			if (u8Found == 0xff && u8Unused < MIB_GROUP_MAX)
			{
				/* Note group ID */
				memcpy(&psMibGroup->sPerm.asGroupAddr[u8Unused], psAddr, sizeof(in6_addr));
				/* Make sure permament data is saved */
				PDM_vSaveRecord(&psMibGroup->sDesc);
				/* Debug */
				DBG_vPrintf(DEBUG_MIB_GROUP, "\n%sPDM_vSaveRecord(MibGroup) = %d", acDebugIndent, psMibGroup->sDesc.eState);
				/* Allow join */
				bReturn = TRUE;
			}
			/* Already in the group ? */
			else if (u8Found < MIB_GROUP_MAX)
			{
				/* Allow join */
				bReturn = TRUE;
			}
		}
		/* Leaving a group ? */
		else if (E_JIP_GROUP_LEAVE)
		{
			/* Already in the group ? */
			if (u8Found < MIB_GROUP_MAX)
			{
				/* Clear group ID */
				memset(&psMibGroup->sPerm.asGroupAddr[u8Found], 0, sizeof(in6_addr));
				/* Make sure permament data is saved */
				PDM_vSaveRecord(&psMibGroup->sDesc);
				/* Debug */
				DBG_vPrintf(DEBUG_MIB_GROUP, "\n%sPDM_vSaveRecord(MibGroup) = %d", acDebugIndent, psMibGroup->sDesc.eState);
				/* Allow leave */
				bReturn = TRUE;
			}
			/* Not in group ? */
			else
			{
				/* Allow leave */
				bReturn = TRUE;
			}
		}
	}

	/* Debug */
	Node_vDebugOutdent(DEBUG_MIB_GROUP);

	return bReturn;
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
