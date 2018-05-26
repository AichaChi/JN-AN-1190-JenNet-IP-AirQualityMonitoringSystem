/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         Node MIB - Implementation
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
#include <AppApi.h>
/* JenOS includes */
#include <dbg.h>
#include <dbg_uart.h>
#include <os.h>
#include <pdm.h>
/* Application common includes */
#include "DeviceDefs.h"
#include "Node.h"
#include "MibCommon.h"
#include "MibNode.h"
#include"MibCOstatus.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#define PS_MIB_CO_STATUS ((tsMibCOstatus *) psMibNode->pvMibCOstatus)

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
PRIVATE void MibNode_vNameCallback(char *pcName);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
PRIVATE tsMibNode	    *psMibNode;		/* Permament stack node mib */
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: MibNode_vInit
 *
 * DESCRIPTION:
 * Initialises data
 *
 ****************************************************************************/
PUBLIC void MibNode_vInit(tsMibNode *psMibNodeInit)
{
	/* Debug */
	DBG_vPrintf(DEBUG_MIB_NODE, "\n%sMibNode_vInit() {%d}", acDebugIndent, sizeof(tsMibNode));
	Node_vDebugIndent(DEBUG_MIB_NODE);

	/* Take copy of pointer to data */
	psMibNode = psMibNodeInit;

	/* Load Node Status mib data */
	(void) PDM_eLoadRecord(&psMibNode->sDesc,
						   (uint16)(MIB_ID_NODE & 0xFFFF),
						   (void *) &psMibNode->sPerm,
						   sizeof(psMibNode->sPerm),
						   FALSE);
	/* Debug */
	DBG_vPrintf(DEBUG_MIB_NODE, "\n%sPDM_eLoadRecord(MibNode) = %d", acDebugIndent, psMibNode->sDesc.eState);
	/* Debug */
	Node_vDebugOutdent(DEBUG_MIB_NODE);
}

/****************************************************************************
 *
 * NAME: MibNode_vRegister
 *
 * DESCRIPTION:
 * Registers MIB and name with stack
 *
 ****************************************************************************/
PUBLIC void MibNode_vRegister(void)
{
	/* Debug */
	DBG_vPrintf(DEBUG_MIB_NODE, "\n%sMibNode_vRegister()", acDebugIndent);
	Node_vDebugIndent(DEBUG_MIB_NODE);

	/* Did we not load a name from flash ? */
	if (psMibNode->sPerm.acName[0] == '\0')
	{
		uint8 		  u8NameLen;
		char          acHex[17]="0123456789ABCDEF";
		uint64      *pu64MacAddr;

		/* Get pointer to MAC address */
		pu64MacAddr = (uint64 *) pvAppApiGetMacAddrLocation();
		/* Base name specified by makefile ? */
		#ifdef MK_JIP_NODE_NAME
		{
			/* Copy base device name */
			strncpy(psMibNode->sPerm.acName, MK_JIP_NODE_NAME, 8);
		}
		/* No base name specified by makefile */
		#else
		{
			/* Use JIP Device ID */
			sMibNode.sPerm.acName[0] = acHex[((MK_JIP_DEVICE_ID >> 28) & 0xf)];
			sMibNode.sPerm.acName[1] = acHex[((MK_JIP_DEVICE_ID >> 24) & 0xf)];
			sMibNode.sPerm.acName[2] = acHex[((MK_JIP_DEVICE_ID >> 20) & 0xf)];
			sMibNode.sPerm.acName[3] = acHex[((MK_JIP_DEVICE_ID >> 16) & 0xf)];
			sMibNode.sPerm.acName[4] = acHex[((MK_JIP_DEVICE_ID >> 12) & 0xf)];
			sMibNode.sPerm.acName[5] = acHex[((MK_JIP_DEVICE_ID >>  8) & 0xf)];
			sMibNode.sPerm.acName[6] = acHex[((MK_JIP_DEVICE_ID >>  4) & 0xf)];
			sMibNode.sPerm.acName[7] = acHex[((MK_JIP_DEVICE_ID      ) & 0xf)];
		}
		#endif
		/* Terminate early if required */
		psMibNode->sPerm.acName[8] = '\0';
		/* Note length */
		u8NameLen = strlen(psMibNode->sPerm.acName);
		/* Append least significant 6 digits of mac address */
		psMibNode->sPerm.acName[u8NameLen    ] = ' ';
		psMibNode->sPerm.acName[u8NameLen + 1] = acHex[((*pu64MacAddr >> 20) & 0xf)];
		psMibNode->sPerm.acName[u8NameLen + 2] = acHex[((*pu64MacAddr >> 16) & 0xf)];
		psMibNode->sPerm.acName[u8NameLen + 3] = acHex[((*pu64MacAddr >> 12) & 0xf)];
		psMibNode->sPerm.acName[u8NameLen + 4] = acHex[((*pu64MacAddr >>  8) & 0xf)];
		psMibNode->sPerm.acName[u8NameLen + 5] = acHex[((*pu64MacAddr >>  4) & 0xf)];
		psMibNode->sPerm.acName[u8NameLen + 6] = acHex[((*pu64MacAddr      ) & 0xf)];
		/* Make sure permament data is saved */
		PDM_vSaveRecord(&psMibNode->sDesc);
		/* Debug */
		DBG_vPrintf(DEBUG_MIB_NODE, "\n%sPDM_vSaveRecord(MibNode) = %d", acDebugIndent, psMibNode->sDesc.eState);
	}

	/* Tell the stack what its Node mib Name variable should be set to */
	vJIP_SetNodeName(psMibNode->sPerm.acName);
	/* Register a callback when the stack's Node mib Name variable is updated */
	vJIP_RegisterSetNameCallback(MibNode_vNameCallback);

	/* Debug */
	Node_vDebugOutdent(DEBUG_MIB_NODE);
}

/****************************************************************************/
/***        Callback Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: MibNode_vNameCallback
 *
 * DESCRIPTION:
 * Callback when stack's Node Mib Name Variable is updated by JIP
 *
 ****************************************************************************/
PRIVATE void MibNode_vNameCallback(char *pcName)
{
	/* Debug */
	DBG_vPrintf(DEBUG_MIB_NODE, "\n%sMibNode_vNameCallback()", acDebugIndent);
	Node_vDebugIndent(DEBUG_MIB_NODE);

	/* Copy new name */
	strcpy(psMibNode->sPerm.acName, pcName);
	/* Make sure permament data is saved */
	PDM_vSaveRecord(&psMibNode->sDesc);
	/* Debug */
	DBG_vPrintf(DEBUG_MIB_NODE, "\n%sPDM_vSaveRecord(MibNode) = %d", acDebugIndent, psMibNode->sDesc.eState);

	/* Debug */
	Node_vDebugOutdent(DEBUG_MIB_NODE);
}


/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
