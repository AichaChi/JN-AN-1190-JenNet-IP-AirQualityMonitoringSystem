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
#ifndef  CONFIG_H_INCLUDED
#define  CONFIG_H_INCLUDED

#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/
#include <jendefs.h>

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define CONFIG_NETWORK_ID 					 MK_NETWORK_ID
#define CONFIG_PAN_ID 								0xffff
#if (MK_CHANNEL>=11 && MK_CHANNEL<=26)
#define CONFIG_SCAN_CHANNELS			   (1<<MK_CHANNEL)
#else
#define CONFIG_SCAN_CHANNELS					0x07fff800
#endif
#define CONFIG_END_DEVICE_INACTIVITY_TIMEOUT		   600
#define CONFIG_FRAME_COUNTER_DELTA					  4096
#define CONFIG_ADDRESS_PREFIX		 0xfd040bd380e80002ULL
#ifdef DBG_ENABLE
#define CONFIG_ROUTING_TABLE_ENTRIES				    16
#else
#define CONFIG_ROUTING_TABLE_ENTRIES				   275
#endif
#define CONFIG_UNIQUE_WATCHERS 							16
#define CONFIG_MAX_TRAPS								16
#define CONFIG_QUEUE_LENGTH								16
#define CONFIG_MAX_NAME_LEN								16
#define CONFIG_MAX_FAILED_PACKETS						 7	  /* Stack default = 7 */
#define CONFIG_MIN_BEACON_LQI							18	  /* Stack default = 55 */
#define CONFIG_PRF_BEACON_LQI							25	  /* Stack default = NA */
#define CONFIG_ROUTER_PING_PERIOD					  1500	  /* Stack default = 1500 */
#define CONFIG_ROUTER_INDIRECT_TX_BUFFERS				 8	  /* Number of indirect buffers in routers */
#define CONFIG_JOIN_PROFILE								 6	  /* Starting join profile small gateway network */
#define CONFIG_RUN_PROFILE								 6	  /* Starting run profile small standalone network (get overridden on joining a gateway network) */
#define CONFIG_FAST_COMMISSIONING_CHANNEL				11
#define CONFIG_FAST_COMMISSIONING_PAN_ID			0x1111
#define CONFIG_FAST_COMMISSIONING_KEY_1			0x19701117
#define CONFIG_FAST_COMMISSIONING_KEY_2			0x19711104
#define CONFIG_FAST_COMMISSIONING_KEY_3			0x20040819
#define CONFIG_FAST_COMMISSIONING_KEY_4			0x20061219

/* Data sizes */
#define ESTABLISH_ROUTE_EXTRA_TYPES	4

/* Data mins and maxes */
#define CONFIG_INT16_MIN (-32768)

/* Evaluation kit board LED masks */
#define CONFIG_LED_MASK_DR1047_D1 0x10000
#define CONFIG_LED_MASK_DR1047_D2 0x20000
#define CONFIG_LED_MASK_DR1047_D3 0x40000
#define CONFIG_LED_MASK_DR1047_D4 0x80000
#define CONFIG_LED_MASK_DR1174_D3 0x00008
#define CONFIG_LED_MASK_DR1174_D6 0x00004
#define CONFIG_LED_MASK_DR1199_D1 0x10000
#define CONFIG_LED_MASK_DR1199_D2 0x02000
#define CONFIG_LED_MASK_DR1199_D3 0x00001

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
/* Beacon and Basic Establish Route Data Structure */
#pragma GCC diagnostic ignored "-Wpacked"
#pragma GCC diagnostic ignored "-Wattributes"
typedef struct
{
	uint32 u32NetworkId;
	uint16 u16DeviceType;
} PACK tsBeaconUserData;
#pragma GCC diagnostic pop

/* Extended Establish Route Data Structure */
#pragma GCC diagnostic ignored "-Wpacked"
#pragma GCC diagnostic ignored "-Wattributes"
typedef struct
{
	uint32  u32NetworkId;
	uint16  u16DeviceType;
	uint8    u8ExtraTypes;
	uint16 au16ExtraTypes[ESTABLISH_ROUTE_EXTRA_TYPES];
} PACK tsEstablishRouteUserData;
#pragma GCC diagnostic pop

/* Device PDM data */
typedef struct
{
	uint32 u32JipDeviceId;
	uint32 u32Spare;
} tsDevicePdm;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

#if defined __cplusplus
}
#endif

#endif  /* CONFIG_H_INCLUDED */
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
