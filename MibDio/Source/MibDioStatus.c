/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         DioStatus MIB Implementation
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
#include "MibDio.h"
#include "MibDioStatus.h"
//#include "Uart.h"

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
PRIVATE tsMibDioStatus *psMibDioStatus;  /* MIB data */

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: MibDioStatus_vInit
 *
 * DESCRIPTION:
 * Initialises data
 *
 ****************************************************************************/
PUBLIC void MibDioStatus_vInit(thJIP_Mib         hMibDioStatusInit,
                               tsMibDioStatus  *psMibDioStatusInit)
{
    /* Debug */
    DBG_vPrintf(DEBUG_MIB_DIO_STATUS, "\n%sMibDioStatus_vInit() {%d}", acDebugIndent, sizeof(tsMibDioStatus));

    /* Valid data pointer ? */
    if (psMibDioStatusInit != (tsMibDioStatus *) NULL)
    {
        /* Take copy of pointer to data */
        psMibDioStatus = psMibDioStatusInit;
        /* Take a copy of the MIB handle */
        psMibDioStatus->hMib = hMibDioStatusInit;

	/* Read values from registers */
	psMibDioStatus->sTemp.u32Input     = (u32AHI_DioReadInput()       & MIB_DIO_MASK);
	psMibDioStatus->sTemp.u32Interrupt = (u32AHI_DioInterruptStatus() & MIB_DIO_MASK);
    }
}

/****************************************************************************
 *
 * NAME: MibDioStatus_vRegister
 *
 * DESCRIPTION:
 * Registers MIB
 *
 ****************************************************************************/
PUBLIC void MibDioStatus_vRegister(void)
{
    teJIP_Status eStatus;

    /* Debug */
    DBG_vPrintf(DEBUG_MIB_DIO_STATUS, "\n%sMibDioConfig_vRegister()", acDebugIndent);
    Node_vDebugIndent(DEBUG_MIB_DIO_STATUS);

    /* Register MIB */
    eStatus = eJIP_RegisterMib(psMibDioStatus->hMib);
    /* Debug */
    DBG_vPrintf(DEBUG_MIB_DIO_STATUS, "\n%seJIP_RegisterMib(DioStatus) = %d", acDebugIndent, eStatus);

    /* Debug */
    Node_vDebugOutdent(DEBUG_MIB_DIO_STATUS);
}

/****************************************************************************
 *
 * NAME: MibDioStatus_vTick
 *
 * DESCRIPTION:
 * Tick function called every 10ms when stack is running
 *
 ****************************************************************************/
PUBLIC void MibDioStatus_vTick(void)
{
	/* Are there any variable notifications pending ? */
	if ((psMibDioStatus->u32NotifyChanged & VAR_MASK_DIO_STATUS) != 0)
	{
		/* Use common function to output notifications */
		Node_vJipNotifyChanged(psMibDioStatus->hMib, &psMibDioStatus->u32NotifyChanged, VAR_MASK_DIO_STATUS, VAR_COUNT_DIO_STATUS);
	}
}

/****************************************************************************
 *
 * NAME: MibDioStatus_vAppTimer100ms
 *
 * DESCRIPTION:
 * Timer function
 *
 ****************************************************************************/
PUBLIC void MibDioStatus_vAppTimer100ms(void)
{
	uint32 u32Input;

	/* UART ? */
	#ifdef UART_H_INCLUDED
	{
		/* Initialise UART */
		UART_vChar('d');
	}
	#endif
	/* Read DIO inputs */
	u32Input = (u32AHI_DioReadInput() & MIB_DIO_MASK);
	/* Compare against previous reading to check for and report upon changes */
	if (u32Input != psMibDioStatus->sTemp.u32Input)
	{
		/* Note new reading */
		psMibDioStatus->sTemp.u32Input = u32Input;
		/* Notify any traps */
		//psMibDioStatus->u32NotifyChanged |= (1 << VAR_IX_DIO_STATUS_INPUT);
	}
	/* UART ? */
	#ifdef UART_H_INCLUDED
	{
		/* Initialise UART */
		UART_vChar('e');
	}
	#endif
}

/****************************************************************************
 *
 * NAME: MibDioStatus_vSysCtrlEvent
 *
 * DESCRIPTION:
 * Peripheral event handler
 *
 ****************************************************************************/
PUBLIC void MibDioStatus_vSysCtrlEvent(uint32 u32Device, uint32 u32ItemBitmap)
{
	/* System Control interrupts are raised for DIO events */
	if (u32Device == E_AHI_DEVICE_SYSCTRL)
	{
		uint32 u32Interrupt;
		uint32 u32Input;

		/* Read DIO interrupts */
		u32Interrupt = (u32ItemBitmap & MIB_DIO_MASK);
		/* DIO interrupt ? */
		if (u32Interrupt != 0)
		{
			/* Note new reading */
			psMibDioStatus->sTemp.u32Interrupt = u32Interrupt;
			/* Notify any traps */
			//psMibDioStatus->u32NotifyChanged |= (1 << VAR_IX_DIO_STATUS_INTERRUPT);

			/* Read DIO inputs */
			u32Input = (u32AHI_DioReadInput() & MIB_DIO_MASK);
			/* Compare against previous reading to check for and report upon changes */
			if (u32Input != psMibDioStatus->sTemp.u32Input)
			{
				/* Note new reading */
				psMibDioStatus->sTemp.u32Input = u32Input;
				/* Notify any traps */
				//psMibDioStatus->u32NotifyChanged |= (1 << VAR_IX_DIO_STATUS_INPUT);
			}
		}
	}
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
