/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         General Configuration Parameters
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
#ifndef  DEVICEDEFS_H_INCLUDED
#define  DEVICEDEFS_H_INCLUDED

#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/
/* SDK includes */
#include <jendefs.h>
/* JenOS includes */
#include <dbg_uart.h>
#include <AppHardwareApi.h>
/* Stack includes */
#include <jip.h>
/* Application includes */
#include "Config.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
/* Operating defines */
#define DEVICE_UDP_SOCKETS						   3 /* Number of UDP sockets available */
#define DEVICE_COMMISSION_TIMEOUT				  10 /* Factory reset if not fully commissioned after joining standalone network */
#define DEVICE_JOIN_TIMEOUT		 				   0 /* No join timeout */
#define DEVICE_WAKE_TIMER_PERIOD_10MS 	          10 /* Wake timer period (in 10ms intervals) */
#define DEVICE_ED_STACK_RESUME					  20 /* Time for stack to sleep when network is up (in multiples of DEVICE_WAKE_TIMER_PERIOD_10MS) */
#define DEVICE_ED_SCAN_SLEEP_MS	 				5000 /* End device scan sleep period milliseconds */
#define DEVICE_ED_APP_SLEEP_MS 				   (DEVICE_ED_STACK_RESUME * DEVICE_WAKE_TIMER_PERIOD_10MS * 20) /* End failsafe sleep period milliseconds (double the normal sleep period uncalibrated) */
#define DEVICE_ED_REQ_AWAKE						   2 /* Stay awake next poll time (in multiples of DEVICE_WAKE_TIMER_PERIOD_10MS) */
#define DEVICE_ED_OND_POLL_DELAY				 100 /* Delay between OND refresh request and poll in ms */
#define DEVICE_ED_OND_WAKES_PER_POLL			   1 /* Number of stack wakes per poll for new data during OND download */
#define DEVICE_ED_OND_POLLS_PER_REFRESH			 150 /* Number of ond poll cycles between checking for a new image */
/* Wake timer configuration */
#define DEVICE_WAKE_TIMER_MAX	 			     0x1FFFFFFFFFFULL
#define DEVICE_WAKE_TIMER_ROLLOVER			     0x1FFF9222FFFULL
#define DEVICE_WAKE_TIMER_SCALE_10MS		 3200000
#define DEVICE_WAKE_TIMER_CALIBRATE_TICK_TIMER FALSE	/* Calibrate wake timer using tick timer */
/* Application output masks */
#define DEVICE_OUTPUT_MASK_WK1 					   (1<<16)
#define DEVICE_OUTPUT_MASK_WK0                     0
#define DEVICE_OUTPUT_MASK_APP                     0
#define DEVICE_OUTPUT_MASK_JIP                     0
#define DEVICE_OUTPUT_MASK_TX 	                   0
#define DEVICE_OUTPUT_MASK_SLEEP	               0
#define DEVICE_OUTPUT_MASK_STAY_AWAKE	           0
#define DEVICE_OUTPUT_MASK_NWK					   (1<<17)
#define DEVICE_OUTPUT_MASK_ALL (DEVICE_OUTPUT_MASK_WK1|DEVICE_OUTPUT_MASK_WK0|DEVICE_OUTPUT_MASK_APP|DEVICE_OUTPUT_MASK_JIP|DEVICE_OUTPUT_MASK_TX|DEVICE_OUTPUT_MASK_SLEEP|DEVICE_OUTPUT_MASK_STAY_AWAKE|DEVICE_OUTPUT_MASK_NWK)

/* Debug flags */
#ifdef DBG_ENABLE
#define DEBUG_UART								E_AHI_UART_0
#define DEBUG_BAUD_RATE							E_AHI_UART_RATE_115200
#define DEBUG_EXCEPTION							TRUE
#define DEBUG_DEVICE_FUNC						FALSE
#define DEBUG_DEVICE_VARS						FALSE
#define DEBUG_NODE_FUNC							FALSE
#define DEBUG_NODE_VARS							FALSE
#define DEBUG_NODE_LQI						    FALSE
#define DEBUG_PROTOCOL							TRUE
#define DEBUG_MIB_NODE							FALSE
#define DEBUG_MIB_GROUP							FALSE
#define DEBUG_MIB_ADC_STATUS					FALSE
#define DEBUG_MIB_NODE_STATUS					FALSE
#define DEBUG_MIB_NODE_CONTROL					FALSE
#define DEBUG_MIB_NWK_STATUS					FALSE
#define DEBUG_MIB_NWK_SECURITY					FALSE
#define DEBUG_MIB_NWK_TEST						FALSE
#else
#define DEBUG_EXCEPTION							FALSE
#define DEBUG_DEVICE_FUNC						FALSE
#define DEBUG_DEVICE_VARS						FALSE
#define DEBUG_NODE_FUNC							FALSE
#define DEBUG_NODE_VARS							FALSE
#define DEBUG_NODE_LQI						    FALSE
#define DEBUG_PROTOCOL							FALSE
#define DEBUG_MIB_NODE							FALSE
#define DEBUG_MIB_GROUP							FALSE
#define DEBUG_MIB_ADC_STATUS					FALSE
#define DEBUG_MIB_NODE_STATUS					FALSE
#define DEBUG_MIB_NODE_CONTROL					FALSE
#define DEBUG_MIB_NWK_STATUS					FALSE
#define DEBUG_MIB_NWK_SECURITY					FALSE
#define DEBUG_MIB_NWK_TEST						FALSE
#endif

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

#if defined __cplusplus
}
#endif

#endif  /* DEVICEDEFS_H_INCLUDED */
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
