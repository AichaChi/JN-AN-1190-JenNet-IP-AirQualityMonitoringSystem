/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         NwkStatus MIB - Interface
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
#ifndef  MIBNWKSTATUS_H_INCLUDED
#define  MIBNWKSTATUS_H_INCLUDED

#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
/* SDK includes */
#include <jendefs.h>
/* JenOS includes */
#include <os.h>
#include <pdm.h>
/* Application common includes */
#include "Table.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define MIB_NWK_STATUS_UP_MODE_NONE					0
#define MIB_NWK_STATUS_UP_MODE_GATEWAY				1
#define MIB_NWK_STATUS_UP_MODE_STANDALONE			2

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
/* Network status permament data */
typedef struct
{
	uint8	u8UpMode;
	uint16 u16UpCount;
	uint32 u32UpTime;
	uint32 u32DownTime;
	uint32 u32FrameCounter;

} tsMibNwkStatusPerm;


/* Network status temporary data */
typedef struct
{
	uint32 u32RunTime;

} tsMibNwkStatusTemp;

/* Network status mib */
typedef struct
{
	/* MIB handle */
	thJIP_Mib 				 hMib;

	/* PDM record descriptor */
	PDM_tsRecordDescriptor   sDesc;

	/* Data structures */
	tsMibNwkStatusPerm 		 sPerm;
	tsMibNwkStatusTemp 		 sTemp;

	/* Other data */
	bool_t         bSaveRecord;
	bool_t		   bUp;						/* Network is up */
	bool_t         bSecurity;
	uint32 		 u32SavedFrameCounter;
	uint32       u32SavedRunTime;
	uint32		 u32NodeTimerSeconds;
	uint32		 u32NotifyChanged;

} tsMibNwkStatus;

/****************************************************************************/
/***        Public Function Prototypes                                     ***/
/****************************************************************************/
PUBLIC void 			MibNwkStatus_vInit( thJIP_Mib       hMibNwkStatusInit,
							   				tsMibNwkStatus *psMibNwkStatusInit,
											bool_t           bMibNwkStatusSecurity);
PUBLIC void 			MibNwkStatus_vRegister(void);
PUBLIC void 			MibNwkStatus_vTick(void);
PUBLIC void 			MibNwkStatus_vSecond(uint32 u8TimerSeconds);
PUBLIC void 			MibNwkStatus_vStackEvent(te6LP_StackEvent eEvent);
PUBLIC void 			MibNwkStatus_vSaveRecord(void);

#if defined __cplusplus
}
#endif

#endif  /* MIBNWKSTATUS_H_INCLUDED */
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
