/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         DioControl MIB Implementation
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
/* JenOS includes */
#include <dbg.h>
#include <dbg_uart.h>
#include <os.h>
#include <pdm.h>
/* Application device includes */
#include "DeviceDefs.h"
#include "Node.h"
#include "Address.h"
#include "MibDio.h"
#include "MibDioControl.h"
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
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
PRIVATE tsMibDioControl *psMibDioControl;  /* MIB data */

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: MibDioControl_vInit
 *
 * DESCRIPTION:
 * Initialises data
 *
 ****************************************************************************/
PUBLIC void MibDioControl_vInit(thJIP_Mib          hMibDioControlInit,
                                tsMibDioControl *psMibDioControlInit)
{
    /* Debug */
    DBG_vPrintf(DEBUG_MIB_DIO_CONTROL, "\n%sMibDioControl_vInit() {%d}", acDebugIndent, sizeof(tsMibDioControl));
    Node_vDebugIndent(DEBUG_MIB_DIO_CONTROL);

    /* Valid data pointer ? */
    if (psMibDioControlInit != (tsMibDioControl *) NULL)
    {
        /* Take copy of pointer to data */
        psMibDioControl = psMibDioControlInit;
        /* Take a copy of the MIB handle */
        psMibDioControl->hMib = hMibDioControlInit;

		/* Read values from registers (to preserve default state if no data is read from PDM) */
		psMibDioControl->sPerm.u32Output = (u32REG_GpioRead(REG_GPIO_DOUT) & MIB_DIO_MASK);

        /* Load Dio mib data */
        (void) PDM_eLoadRecord(&psMibDioControl->sDesc,
							   (uint16)(MIB_ID_DIO_CONTROL & 0xFFFF),
                               (void *) &psMibDioControl->sPerm,
                               sizeof(psMibDioControl->sPerm),
                               FALSE);
		/* Debug */
		DBG_vPrintf(DEBUG_MIB_DIO_CONTROL, "\n%sPDM_eLoadRecord(DioControl) = %d", acDebugIndent, psMibDioControl->sDesc.eState);

		/* Initialise Dio from PDM data */
		vAHI_DioSetOutput((psMibDioControl->sPerm.u32Output & MIB_DIO_MASK), ((~psMibDioControl->sPerm.u32Output) & MIB_DIO_MASK));

		/* Read back values from registers (to resolve devices with differing numbers of DIO) */
		psMibDioControl->sPerm.u32Output = (u32REG_GpioRead(REG_GPIO_DOUT) & MIB_DIO_MASK);

		/* Building Group MIB ? */
		#if MK_BLD_MIB_GROUP
		{
			/* Was the group's MIB PDM not recovered ? */
			if (MibGroup_ePdmStateInit() != PDM_RECOVERY_STATE_RECOVERED)
			{
				in6_addr sAddr;

				/* Build "All Occupancy Sensors" group address */
				Address_vBuildGroup(&sAddr, NULL, (uint16)(MIB_ID_DIO_CONTROL & 0xffff));
				/* Make a dummy call to get it in the group address array */
				bJIP_GroupCallback(E_JIP_GROUP_JOIN, &sAddr);
			}
		}
		#endif
    }

    /* Debug */
    Node_vDebugOutdent(DEBUG_MIB_DIO_CONTROL);
}

/****************************************************************************
 *
 * NAME: MibDioControl_vRegister
 *
 * DESCRIPTION:
 * Registers MIB
 *
 ****************************************************************************/
PUBLIC void MibDioControl_vRegister(void)
{
    teJIP_Status eStatus;

    /* Debug */
    DBG_vPrintf(DEBUG_MIB_DIO_CONTROL, "\n%sMibDioControl_vRegister()", acDebugIndent);
    Node_vDebugIndent(DEBUG_MIB_DIO_CONTROL);

    /* Register MIB */
    eStatus = eJIP_RegisterMib(psMibDioControl->hMib);
    /* Debug */
    DBG_vPrintf(DEBUG_MIB_DIO_CONTROL, "\n%seJIP_RegisterMib(DioControl)=%d", acDebugIndent, eStatus);

    /* Make sure permament data is saved */
    PDM_vSaveRecord(&psMibDioControl->sDesc);
    /* Debug */
    DBG_vPrintf(DEBUG_MIB_DIO_CONTROL, "\n%sPDM_vSaveRecord(DioControl) = %d", acDebugIndent, psMibDioControl->sDesc.eState);

    /* Debug */
    Node_vDebugOutdent(DEBUG_MIB_DIO_CONTROL);
}

/****************************************************************************
 *
 * NAME: MibDioControl_vTick
 *
 * DESCRIPTION:
 * Tick function called every 10ms when stack is running
 *
 ****************************************************************************/
PUBLIC void MibDioControl_vTick(void)
{
	/* Are there any variable notifications pending ? */
	if ((psMibDioControl->u32NotifyChanged & VAR_MASK_DIO_CONTROL) != 0)
	{
		/* Use common function to output notifications */
		Node_vJipNotifyChanged(psMibDioControl->hMib, &psMibDioControl->u32NotifyChanged, VAR_MASK_DIO_CONTROL, VAR_COUNT_DIO_CONTROL);
	}
}

/****************************************************************************
 *
 * NAME: MibDioControl_eSetOutput
 *
 * DESCRIPTION:
 * Sets Output variable
 *
 ****************************************************************************/
PUBLIC teJIP_Status MibDioControl_eSetOutput(uint32 u32Val, void *pvCbData)
{
    teJIP_Status   eReturn = E_JIP_OK;

	/* Write new value */
	vAHI_DioSetOutput((u32Val & MIB_DIO_MASK), ((~u32Val) & MIB_DIO_MASK)) ;
	/* Read back final value */
	psMibDioControl->sPerm.u32Output = (u32REG_GpioRead(REG_GPIO_DOUT) & MIB_DIO_MASK);

   	/* Make sure permament data is saved */
    PDM_vSaveRecord(&psMibDioControl->sDesc);
    /* Debug */
    DBG_vPrintf(DEBUG_MIB_DIO_CONTROL, "\n%sPDM_vSaveRecord(DioControl) = %d", acDebugIndent, psMibDioControl->sDesc.eState);

    return eReturn;
}

/****************************************************************************
 *
 * NAME: MibDioControl_eSetOutputOn
 *
 * DESCRIPTION:
 * Sets OutputOn variable
 *
 ****************************************************************************/
PUBLIC teJIP_Status MibDioControl_eSetOutputOn(uint32 u32Val, void *pvCbData)
{
    teJIP_Status   eReturn = E_JIP_OK;

	/* Write new value */
	vAHI_DioSetOutput(u32Val, 0);
	/* Read back final value */
	psMibDioControl->sPerm.u32Output = (u32REG_GpioRead(REG_GPIO_DOUT) & MIB_DIO_MASK);
	/* Notify any traps */
	psMibDioControl->u32NotifyChanged |= (1 << VAR_IX_DIO_CONTROL_OUTPUT);

   	/* Make sure permament data is saved */
    PDM_vSaveRecord(&psMibDioControl->sDesc);
    /* Debug */
    DBG_vPrintf(DEBUG_MIB_DIO_CONTROL, "\n%sPDM_vSaveRecord(DioControl) = %d", acDebugIndent, psMibDioControl->sDesc.eState);

    return eReturn;
}

/****************************************************************************
 *
 * NAME: MibDioControl_eSetOutputOff
 *
 * DESCRIPTION:
 * Sets OutputOff variable
 *
 ****************************************************************************/
PUBLIC teJIP_Status MibDioControl_eSetOutputOff(uint32 u32Val, void *pvCbData)
{
    teJIP_Status   eReturn = E_JIP_OK;

	/* Write new value */
	vAHI_DioSetOutput(0, u32Val);
	/* Read back final value */
	psMibDioControl->sPerm.u32Output = (u32REG_GpioRead(REG_GPIO_DOUT) & MIB_DIO_MASK);
	/* Notify any traps */
	psMibDioControl->u32NotifyChanged |= (1 << VAR_IX_DIO_CONTROL_OUTPUT);

   	/* Make sure permament data is saved */
    PDM_vSaveRecord(&psMibDioControl->sDesc);
    /* Debug */
    DBG_vPrintf(DEBUG_MIB_DIO_CONTROL, "\n%sPDM_vSaveRecord(DioControl) = %d", acDebugIndent, psMibDioControl->sDesc.eState);

    return eReturn;
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
