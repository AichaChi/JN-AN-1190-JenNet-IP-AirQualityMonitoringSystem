/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         Node - common functions
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
#ifndef  NODE_H_INCLUDED
#define  NODE_H_INCLUDED

#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
/* SDK includes */
#include <jendefs.h>
/* Stack includes */
#include <6LP.h>

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
/* Stack states */
#define NODE_STACK_STATE_NONE			 0
#define NODE_STACK_STATE_RESUME			 1
#define NODE_STACK_STATE_RUNNING		 2

extern char acDebugIndent[65];

/****************************************************************************/
/***        Public Function Prototypes                                     ***/
/****************************************************************************/
PUBLIC void   				Node_vInit(bool_t);
PUBLIC bool_t 				Node_bTestFactoryResetEeprom(void);
PUBLIC void   				Node_vPdmInit(uint8, uint8);
PUBLIC void   				Node_vReset(bool_t);
PUBLIC teJIP_Status 		Node_eJipInit(void);
PUBLIC void 				Node_v6lpConfigureNetwork(tsNetworkConfigData *);
PUBLIC void 				Node_v6lpResume(void);
PUBLIC void 				Node_v6lpDataEvent(int, te6LP_DataEvent, ts6LP_SockAddr *, uint8);
PUBLIC bool_t 				Node_bJipStackEvent(te6LP_StackEvent eEvent, void *pvData, uint8 u8DataLen);
PUBLIC void 				Node_vMain(void);
PUBLIC void 				Node_vSysCtrlEvent(uint32 u32Device, uint32 u32ItemBitmap);
PUBLIC void 				Node_vTickTimerEvent(void);
PUBLIC uint8 				Node_u8AnalogueEvent(void);
PUBLIC void 				Node_vTick(void);
PUBLIC void 				Node_vAppTimer100ms(void);
PUBLIC void 				Node_vSecond(void);
PUBLIC void					Node_vJipNotifyChanged(thJIP_Mib phMib, uint32 *pu32VarFlags, uint32 u32VarMask, uint8 u8VarCount);
PUBLIC void					Node_vCheckWakeTimer1(void);
PUBLIC void                 Node_vCheckCalib(uint8 u8Calib);
PUBLIC bool_t               Node_bCalibrate(void);
PUBLIC uint32			    Node_u32StackState(void);
PUBLIC bool_t 				Node_bJoined(void);
PUBLIC bool_t 				Node_bUp(void);
PUBLIC uint64			    Node_u64WakeTimer1Period(void);
PUBLIC uint32			    Node_u32WakeTimer0Recoveries(void);
PUBLIC uint32			    Node_u32WakeTimer1Recoveries(void);
PUBLIC uint32 				Node_u32TimerSeconds(void);
PUBLIC uint8                Node_u8SleepPeriods(void);
PUBLIC uint32 				Node_u32TimerTicks(void);

#if MK_BLD_NODE_TYPE_END_DEVICE
PUBLIC void 				Node_vSleep(void);
PUBLIC void 				Node_vPreSleepCallback(void);
#endif
PUBLIC void Node_vDebugIndent(bool_t bStream);
PUBLIC void Node_vDebugOutdent(bool_t bStream);

#if defined __cplusplus
}
#endif

#endif  /* NODE_H_INCLUDED */
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
