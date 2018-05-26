/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         NodeControl MIB - Implementation
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
#include <AccessFunctions.h>
/* JenOS includes */
#include <dbg.h>
#include <dbg_uart.h>
#include <os.h>
#include <pdm.h>
/* Application common includes */
#include "DeviceDefs.h"
#include "Node.h"
#include "MibNodeControl.h"

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
PRIVATE tsMibNodeControl 	*psMibNodeControl;	   /* Node Control Mib data */

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Imported Functions                                            ***/
/****************************************************************************/
PUBLIC void Device_vReset(bool_t bFactoryReset);

/****************************************************************************
 *
 * NAME: MibNodeControl_vInit
 *
 * DESCRIPTION:
 * Initialises data
 *
 ****************************************************************************/
PUBLIC void MibNodeControl_vInit(thJIP_Mib            hMibNodeControlInit,
								 tsMibNodeControl 	*psMibNodeControlInit)
{
	/* Debug */
	DBG_vPrintf(DEBUG_MIB_NODE_CONTROL, "\n%sMibNodeControl_vInit() {%d}", acDebugIndent, sizeof(tsMibNodeControl));

	/* Valid data pointer ? */
	if (psMibNodeControlInit != (tsMibNodeControl *) NULL)
	{
		/* Take copy of pointer to data */
		psMibNodeControl = psMibNodeControlInit;

		/* Take a copy of the MIB handle */
		psMibNodeControl->hMib = hMibNodeControlInit;
	}
}

/****************************************************************************
 *
 * NAME: MibNodeControl_vRegister
 *
 * DESCRIPTION:
 * Registers MIB
 *
 ****************************************************************************/
PUBLIC void MibNodeControl_vRegister(void)
{
	teJIP_Status eStatus;

	/* Debug */
	DBG_vPrintf(DEBUG_MIB_NODE_CONTROL, "\n%sMibNodeControl_vRegister()", acDebugIndent);
	/* Register MIB */
	eStatus = eJIP_RegisterMib(psMibNodeControl->hMib);
	/* Debug */
	DBG_vPrintf(DEBUG_MIB_NODE_CONTROL, " { eJIP_RegisterMib(NodeControl) = %d }", eStatus);
}

/****************************************************************************
 *
 * NAME: MibNodeControl_vSecond
 *
 * DESCRIPTION:
 * Timing function
 *
 ****************************************************************************/
PUBLIC void MibNodeControl_vSecond(uint32 u32TimerSeconds)
{
	/* Is a factory reset scheduled ? */
	if (psMibNodeControl->u32FactoryResetSeconds != 0)
	{
		/* Is it time to reset now ? */
		if (u32TimerSeconds >= psMibNodeControl->u32FactoryResetSeconds)
		{
			/* Zero timers */
			psMibNodeControl->u32FactoryResetSeconds = 0;
			psMibNodeControl->sTemp.u16FactoryReset  = 0;
			/* Debug */
			DBG_vPrintf(DEBUG_MIB_NODE_CONTROL, "\n%sMibNodeControl_vSecond() FACTORY_RESET", acDebugIndent);
			Node_vDebugIndent(DEBUG_MIB_NODE_CONTROL);
			/* Perform factory reset */
			Device_vReset(TRUE);
			/* Debug */
			Node_vDebugOutdent(DEBUG_MIB_NODE_CONTROL);
		}
		/* Not yet time to reset ? */
		else
		{
			/* Update countdown */
			psMibNodeControl->sTemp.u16FactoryReset = psMibNodeControl->u32FactoryResetSeconds - u32TimerSeconds;
		}
	}
	/* Is a reset scheduled ? */
	if (psMibNodeControl->u32ResetSeconds != 0)
	{
		/* Is it time to reset now ? */
		if (u32TimerSeconds >= psMibNodeControl->u32ResetSeconds)
		{
			/* Zero timers */
			psMibNodeControl->u32ResetSeconds = 0;
			psMibNodeControl->sTemp.u16Reset  = 0;
			/* Debug */
			DBG_vPrintf(DEBUG_MIB_NODE_CONTROL, "\n%sMibNodeControl_vSecond() RESET", acDebugIndent);
			Node_vDebugIndent(DEBUG_MIB_NODE_CONTROL);
			/* Perform reset */
			Device_vReset(FALSE);
			/* Debug */
			Node_vDebugOutdent(DEBUG_MIB_NODE_CONTROL);
		}
		/* Not yet time to reset ? */
		else
		{
			/* Update countdown */
			psMibNodeControl->sTemp.u16Reset = psMibNodeControl->u32ResetSeconds - u32TimerSeconds;
		}
	}
}

/****************************************************************************
 *
 * NAME: MibNodeControl_eSetReset
 *
 * DESCRIPTION:
 * Sets Reset variable
 *
 ****************************************************************************/
PUBLIC teJIP_Status MibNodeControl_eSetReset(uint16 u16Val, void *pvCbData)
{
    teJIP_Status eReturn;

    /* Call standard function */
    eReturn = eSetUint16(u16Val, pvCbData);
    /* Clearing reset timer ? */
    if (u16Val == 0)
    {
		/* Clear the reset time */
		psMibNodeControl->u32ResetSeconds = 0;
	}
	/* Setting reset timer ? */
	else
	{
		/* Calculate reset time */
		psMibNodeControl->u32ResetSeconds = Node_u32TimerSeconds() + u16Val;
	}

    return eReturn;
}

/****************************************************************************
 *
 * NAME: MibNodeControl_eSetFactoryReset
 *
 * DESCRIPTION:
 * Sets Reset variable
 *
 ****************************************************************************/
PUBLIC teJIP_Status MibNodeControl_eSetFactoryReset(uint16 u16Val, void *pvCbData)
{
    teJIP_Status eReturn;

    /* Call standard function */
    eReturn = eSetUint16(u16Val, pvCbData);
    /* Clearing reset timer ? */
    if (u16Val == 0)
    {
		/* Clear the reset time */
		psMibNodeControl->u32FactoryResetSeconds = 0;
	}
	/* Setting reset timer */
	else
	{
		/* Calculate reset time */
		psMibNodeControl->u32FactoryResetSeconds = Node_u32TimerSeconds() + u16Val;
	}

    return eReturn;
}
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
