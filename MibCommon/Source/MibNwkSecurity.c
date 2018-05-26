/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         NwkSecurity MIB - Implementation
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
#include <mac_pib.h>
#include <JIP.h>
#include <6LP.h>
#include <AccessFunctions.h>
#include <OverNetworkDownload.h>
/* JenOS includes */
#include <dbg.h>
#include <dbg_uart.h>
#include <os.h>
#include <pdm.h>
/* Application common includes */
#include "Config.h"
#include "DeviceDefs.h"
#include "Security.h"
#include "Node.h"
#include "MibCommon.h"
#include "MibNwkSecurity.h"
#include "MibNwkStatus.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
/* Special stack modes */
#define STACK_MODE_STANDALONE  0x0001
#define STACK_MODE_COMMISSION  0x0002

/* Custom scan sort settings */
#define MIB_NWK_SECURITY_SCAN_SORT_PIVOT_LQI		96
#define MIB_NWK_SECURITY_SCAN_SORT_PIVOT_WEIGHTED	FALSE

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
/***        External Variables                                            ***/
/****************************************************************************/
/* Primary MIB structures */
extern tsMibNwkSecurity	 sMibNwkSecurity;
extern thJIP_Mib		 hMibNwkSecurity;

/* Secondary MIB structures */
extern tsMibNwkStatus	 sMibNwkStatus;

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Imported Functions                                            ***/
/****************************************************************************/
PUBLIC void Device_vReset(bool_t bFactoryReset);

/****************************************************************************
 *
 * NAME: MibNwkSecurity_vInit
 *
 * DESCRIPTION:
 * Initialises data
 *
 ****************************************************************************/
PUBLIC void MibNwkSecurity_vInit(void)
{
	/* Debug */
	DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%sMibNwkSecurity_vInit() {%d}", acDebugIndent, sizeof(tsMibNwkSecurity));
	Node_vDebugIndent(DEBUG_MIB_NWK_SECURITY);

	/* Load NodeStatus mib data */
	(void) PDM_eLoadRecord(&sMibNwkSecurity.sDesc,
						   (uint16)(MIB_ID_NWK_SECURITY & 0xFFFF),
						   (void *) &sMibNwkSecurity.sPerm,
						   sizeof(sMibNwkSecurity.sPerm),
						   FALSE);
	/* Debug */
	DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%sPDM_eLoadRecord(MibNwkSecurity) = %d", acDebugIndent, sMibNwkSecurity.sDesc.eState);
	DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, " [Chan=%d, PanId=0x%x, NwkKey=%x:%x:%x:%x]",
		sMibNwkSecurity.sPerm.u8Channel,
		sMibNwkSecurity.sPerm.u16PanId,
		sMibNwkSecurity.sPerm.asSecurityKey[0].u32KeyVal_1,
		sMibNwkSecurity.sPerm.asSecurityKey[0].u32KeyVal_2,
		sMibNwkSecurity.sPerm.asSecurityKey[0].u32KeyVal_3,
		sMibNwkSecurity.sPerm.asSecurityKey[0].u32KeyVal_4);

	/* Debug */
	Node_vDebugOutdent(DEBUG_MIB_NWK_SECURITY);
}

/****************************************************************************
 *
 * NAME: MibNwkSecurity_vJipInitData
 *
 * DESCRIPTION:
 * Initialises jip stack data
 *
 ****************************************************************************/
PUBLIC teJIP_Status MibNwkSecurity_eJipInit(void)
{
	teJIP_Status     eStatus;

	/* Debug */
	DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%sMibNwkSecurity_vJipInit()", acDebugIndent);
	Node_vDebugIndent(DEBUG_MIB_NWK_SECURITY);

	/* Configure JIP */
	sMibNwkSecurity.sJipInitData.u64AddressPrefix     	= CONFIG_ADDRESS_PREFIX; 		/* IPv6 address prefix (C only) */
	sMibNwkSecurity.sJipInitData.u32Channel				= CONFIG_SCAN_CHANNELS;     	/* Channel 'bitmap' */
	sMibNwkSecurity.sJipInitData.u16PanId				= 0xffff; 						/* PAN ID to use or look for (0xffff to search/generate) */
	sMibNwkSecurity.sJipInitData.u16MaxIpPacketSize		= 0; 							/* Max IP packet size, 0 defaults to 1280 */
	sMibNwkSecurity.sJipInitData.u16NumPacketBuffers	= 4;    						/* Number of IP packet buffers */
	sMibNwkSecurity.sJipInitData.eDeviceType			= MK_NODE_TYPE;             	/* Device type (C, R, or ED) */
	sMibNwkSecurity.sJipInitData.u32RoutingTableEntries	= CONFIG_ROUTING_TABLE_ENTRIES; /* Routing table size (not ED) */
	sMibNwkSecurity.sJipInitData.u32DeviceId			= MK_JIP_DEVICE_ID;
	sMibNwkSecurity.sJipInitData.u8UniqueWatchers		= CONFIG_UNIQUE_WATCHERS;
	sMibNwkSecurity.sJipInitData.u8MaxTraps				= CONFIG_MAX_TRAPS;
	sMibNwkSecurity.sJipInitData.u8QueueLength 			= CONFIG_QUEUE_LENGTH;
	sMibNwkSecurity.sJipInitData.u8MaxNameLength		= CONFIG_MAX_NAME_LEN;
	sMibNwkSecurity.sJipInitData.u16Port				= JIP_DEFAULT_PORT;
	sMibNwkSecurity.sJipInitData.pcVersion 				= MK_VERSION;

	/* SDK version dependent configuration settings ? */
	#if (MK_JENNIC_SDK_VERSION >= 1107)
	sMibNwkSecurity.sJipInitData.u16JMP_Port            = JIP_DEFAULT_PORT + 2; 		/* Unicast commissioning port (JIP_DEFAULT_PORT + 1 used for OND) */
	#endif

	/* Device dependent configuration settings */
	#ifdef DEVICE_UDP_SOCKETS
	sMibNwkSecurity.sJipInitData.u8UdpSockets			= DEVICE_UDP_SOCKETS;			/* Number of UDP sockets supported */
	#else
	sMibNwkSecurity.sJipInitData.u8UdpSockets			= 2;           					/* Number of UDP sockets supported */
	#endif

	/* Valid channel and PAN ID ? */
	if ((sMibNwkSecurity.sPerm.u8Channel >= 11 && sMibNwkSecurity.sPerm.u8Channel <=    26)
	&&  (sMibNwkSecurity.sPerm.u16PanId  >   0 && sMibNwkSecurity.sPerm.u16PanId  < 0xffff))
	{
		/* Limit scan mask to only previous channel */
		sMibNwkSecurity.sJipInitData.u32Channel = (1<<sMibNwkSecurity.sPerm.u8Channel);
		/* Force previous PAN ID */
		sMibNwkSecurity.sJipInitData.u16PanId = sMibNwkSecurity.sPerm.u16PanId;
	}

	/* Building end device support ? */
	#ifdef MK_BLD_NODE_TYPE_END_DEVICE
	{
		/* Zero routing table entries */
		sMibNwkSecurity.sJipInitData.u32RoutingTableEntries = 0;
	}
	/* Coordinator or router ? */
	#else
	{
		extern uint8 gMAC_u8MaxBuffers;
		extern uint8 u8JNT_IndirectTxBuffers;

		/* Set number of MAC buffers */
		gMAC_u8MaxBuffers 		= CONFIG_ROUTER_INDIRECT_TX_BUFFERS + 2;
		u8JNT_IndirectTxBuffers = CONFIG_ROUTER_INDIRECT_TX_BUFFERS;
	}
	#endif


	/* Debug */
	DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%ssMibNwkSecurity.sJipInitData.eDeviceType            = %d",     acDebugIndent, sMibNwkSecurity.sJipInitData.eDeviceType);
	DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%ssMibNwkSecurity.sJipInitData.u32DeviceId            = 0x%08x", acDebugIndent, sMibNwkSecurity.sJipInitData.u32DeviceId);
	DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%ssMibNwkSecurity.sJipInitData.u32Channel             = 0x%08x", acDebugIndent, sMibNwkSecurity.sJipInitData.u32Channel);
	DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%ssMibNwkSecurity.sJipInitData.u16PanId               = 0x%04x", acDebugIndent, sMibNwkSecurity.sJipInitData.u16PanId);
	DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%ssMibNwkSecurity.sJipInitData.u32RoutingTableEntries = %d",     acDebugIndent, sMibNwkSecurity.sJipInitData.u32RoutingTableEntries);

	/* Debug */
	DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%seJIP_Init()", acDebugIndent);
	Node_vDebugIndent(DEBUG_MIB_NWK_SECURITY);
	/* Wait for clock to stablise */
	while(bAHI_Clock32MHzStable() == FALSE);
	/* Initialise JIP */
	eStatus = eJIP_Init(&sMibNwkSecurity.sJipInitData);
	/* Debug */
	Node_vDebugOutdent(DEBUG_MIB_NWK_SECURITY);
	DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, " = %d", eStatus);

	/* Set 1 second defrag timeout */
	v6LP_SetPacketDefragTimeout(1);

	/* Debug */
	Node_vDebugOutdent(DEBUG_MIB_NWK_SECURITY);

	return eStatus;
}

/****************************************************************************
 *
 * NAME: MibNwkSecurity_u86lpConfigureNetwork
 *
 * DESCRIPTION:
 * Sets network configuration data
 *
 ****************************************************************************/
PUBLIC void MibNwkSecurity_v6lpConfigureNetwork(tsNetworkConfigData *psNetworkConfigData)
{
	uint8 u8Key;
	uint8 u8StartKey   = 1;
	bool_t bResumed    = FALSE;
	bool_t bNetworkKey = FALSE;

	/* Debug */
	DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%sMibNwkSecurity_u86lpConfigureNetwork()", acDebugIndent);
	Node_vDebugIndent(DEBUG_MIB_NWK_SECURITY);
	DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%ssMibNwkStatus.sPerm.u8UpMode = %d", acDebugIndent, sMibNwkStatus.sPerm.u8UpMode);

	/* Initialse configuration */
	psNetworkConfigData->u32EndDeviceActivityTimeout = CONFIG_END_DEVICE_INACTIVITY_TIMEOUT;
  //  psNetworkConfigData->u32EndDevicePollPeriod     = 850;
	/* Only ping when no data sent in previous wake cycle */
	psNetworkConfigData->u8EndDevicePingInterval     = 2;

	/* Building end device support ? */
	#ifdef MK_BLD_NODE_TYPE_END_DEVICE
	{
		/* Want to sleep between scans ? */
		#if DEVICE_ED_SCAN_SLEEP_MS
		{
			/* End device ? */
			if (sMibNwkSecurity.sJipInitData.eDeviceType == E_JIP_DEVICE_END_DEVICE)
			{
				/* Sleep during backoff for end device */
				psNetworkConfigData->bSleepDuringBackoff   = TRUE;
				psNetworkConfigData->u32EndDeviceScanSleep = DEVICE_ED_SCAN_SLEEP_MS;
			}
		}
		#endif
	}
	#endif

	/* Take a pointer to the configuration data for later editing */
	sMibNwkSecurity.psNetworkConfigData = psNetworkConfigData;

	/* Coordinator build ? */
	#ifdef MK_BLD_NODE_TYPE_COORDINATOR
	{
		/* Coordinator ? */
		if (sMibNwkSecurity.sJipInitData.eDeviceType == E_JIP_DEVICE_COORDINATOR)
		{
			/* Also check and default network key below */
			u8StartKey = 0;
		}
	}
	#endif

	/* Loop through commissioning keys */
	for (u8Key = u8StartKey; u8Key < 3; u8Key++)
	{
		/* Did we not load a valid commissioning key from PDM ? */
		if (sMibNwkSecurity.sPerm.asSecurityKey[u8Key].u32KeyVal_1 == 0 &&
			sMibNwkSecurity.sPerm.asSecurityKey[u8Key].u32KeyVal_2 == 0 &&
			sMibNwkSecurity.sPerm.asSecurityKey[u8Key].u32KeyVal_3 == 0 &&
			sMibNwkSecurity.sPerm.asSecurityKey[u8Key].u32KeyVal_4 == 0)
		{
			/* Network key (coordinators only) */
			if (u8Key == 0)
			{
				/* Set default fixed network key - insecure but easier to debug (random would be better but needs to be retained incase of coordinator replacement) */
				sMibNwkSecurity.sPerm.asSecurityKey[u8Key].u32KeyVal_1 =
				sMibNwkSecurity.sPerm.asSecurityKey[u8Key].u32KeyVal_2 =
				sMibNwkSecurity.sPerm.asSecurityKey[u8Key].u32KeyVal_3 =
				sMibNwkSecurity.sPerm.asSecurityKey[u8Key].u32KeyVal_4 = 0x00010001;
			}
			/* Commissioning key ? */
			else
			{
				/* Generate commissioning key from MAC address */
				Security_vBuildCommissioningKey((uint8 *) pvAppApiGetMacAddrLocation(),
												(uint8 *) &sMibNwkSecurity.sPerm.asSecurityKey[u8Key]);
			}

			/* Make sure permament data is saved */
			PDM_vSaveRecord(&sMibNwkSecurity.sDesc);
			/* Debug */
			DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%sPDM_vSaveRecord(MibNwkSecurity) = %d", acDebugIndent, sMibNwkSecurity.sDesc.eState);
		}
//		else
//			{
//			tsSecurityKey  sKey;
//							sKey.u32KeyVal_1=0101010101010101;
//							sKey.u32KeyVal_2=0101010101010101;
//							sKey.u32KeyVal_3=0101010101010101;
//							sKey.u32KeyVal_4=0101010101010101;
//							vSecurityUpdateKey(1, &sKey);
//							DBG_vPrintf(TRUE,"%d%d%d%d :",sKey.u32KeyVal_1,sKey.u32KeyVal_2,sKey.u32KeyVal_3,sKey.u32KeyVal_4);
//           }
	}

	/* Update user data (used in beacon responses and establish routes) */
	MibNwkSecurity_vSetUserData();
	/* Set to default join profile */
	(void) bJnc_SetJoinProfile(CONFIG_JOIN_PROFILE, NULL);
	/* Set to default run profile (will be overridden upon joining a gateway network) */
	(void) bJnc_SetRunProfile(CONFIG_RUN_PROFILE, NULL);

	/* Set up data structures ready for key retrieval and insertion */
	v6LP_EnableSecurity();

	/* Valid channel and PAN ID ? */
	if ((sMibNwkSecurity.sPerm.u8Channel >= 11 && sMibNwkSecurity.sPerm.u8Channel <=    26)
	&&  (sMibNwkSecurity.sPerm.u16PanId  >   0 && sMibNwkSecurity.sPerm.u16PanId  < 0xffff))
	{
		/* Override scan channels and PAN ID to restore previous network */
		psNetworkConfigData->u32ScanChannels = (1<<sMibNwkSecurity.sPerm.u8Channel);
		psNetworkConfigData->u16PanID		 =     sMibNwkSecurity.sPerm.u16PanId;
	}

	/* Do we have a valid security key ? */
	if (sMibNwkSecurity.sPerm.asSecurityKey[MIB_NWK_SECURITY_SECURITY_KEY_NETWORK].u32KeyVal_1 != 0
	||	sMibNwkSecurity.sPerm.asSecurityKey[MIB_NWK_SECURITY_SECURITY_KEY_NETWORK].u32KeyVal_2 != 0
	||	sMibNwkSecurity.sPerm.asSecurityKey[MIB_NWK_SECURITY_SECURITY_KEY_NETWORK].u32KeyVal_3 != 0
	||	sMibNwkSecurity.sPerm.asSecurityKey[MIB_NWK_SECURITY_SECURITY_KEY_NETWORK].u32KeyVal_4 != 0)
	{
		MAC_DeviceDescriptor_s sDeviceDescriptor;
		uint8 u8SecureAddr;
		uint8 u8Restored = 0;

		/* Set network key */
		MibNwkSecurity_vSetSecurityKey(MIB_NWK_SECURITY_SECURITY_KEY_NETWORK);
		/* Build security descriptor for this node */
		memcpy(&sDeviceDescriptor.sExtAddr, pvAppApiGetMacAddrLocation(), sizeof(MAC_ExtAddr_s));
		sDeviceDescriptor.u32FrameCounter = sMibNwkStatus.sPerm.u32FrameCounter;
		/* Restore security descriptor for this node */
		(void) bSecuritySetDescriptor(u8Restored++, &sDeviceDescriptor);
		/* Debug */
		DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%sbSecuritySetDescriptor(%d, %x:%x, %d)",
			acDebugIndent,
			(u8Restored-1),
			sDeviceDescriptor.sExtAddr.u32H,
			sDeviceDescriptor.sExtAddr.u32L,
			sDeviceDescriptor.u32FrameCounter);
		/* Note that we set the network key */
		bNetworkKey = TRUE;

		/* Valid channel and PAN ID ? */
		if ((sMibNwkSecurity.sPerm.u8Channel >= 11 && sMibNwkSecurity.sPerm.u8Channel <=    26)
		&&  (sMibNwkSecurity.sPerm.u16PanId  >   0 && sMibNwkSecurity.sPerm.u16PanId  < 0xffff))
		{
			/* Router ? */
			if (sMibNwkSecurity.sJipInitData.eDeviceType == E_JIP_DEVICE_ROUTER)
			{
				/* Was router in a gateway network ? */
				if (sMibNwkStatus.sPerm.u8UpMode == MIB_NWK_STATUS_UP_MODE_GATEWAY)
				{
					/* SDK version dependent functionality ? */
					#if (MK_JENNIC_SDK_VERSION > 1107)
					{
						/* Allow broadcasts to be received on known channel and PAN until we join */
						vApi_ConfigureKnownGoodNetwork(sMibNwkSecurity.sPerm.u8Channel, sMibNwkSecurity.sPerm.u16PanId);
						/* Debug */
						DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%svApi_ConfigureKnownGoodNetwork(%d, 0x%x)", acDebugIndent, sMibNwkSecurity.sPerm.u8Channel, sMibNwkSecurity.sPerm.u16PanId);
					}
					#else
					{
						/* Output warning */
						//#warning Rejoins gateway without being able to receive broadcasts!
					}
					#endif

					/* Note we have attempted to resume */
					bResumed = TRUE;
				}
				/* Was router in a standalone network ? */
				else if (sMibNwkStatus.sPerm.u8UpMode == MIB_NWK_STATUS_UP_MODE_STANDALONE)
				{
					/* Zero frame counter for other descriptors we are going to create */
					sDeviceDescriptor.u32FrameCounter = 0;
					/* Loop through stored secure addresses */
					for (u8SecureAddr = 0;
						 u8SecureAddr < MIB_NWK_SECURITY_SECURE_ADDR_COUNT;
						 u8SecureAddr++)
					{
						/* Valid secure address ? */
						if (sMibNwkSecurity.sPerm.asSecureAddr[u8SecureAddr].u32H != 0 || sMibNwkSecurity.sPerm.asSecureAddr[u8SecureAddr].u32L != 0)
						{
							/* Build security descriptor for this node */
							memcpy(&sDeviceDescriptor.sExtAddr, &sMibNwkSecurity.sPerm.asSecureAddr[u8SecureAddr], sizeof(MAC_ExtAddr_s));
							/* Restore security descriptor for this node (well reserve a slot anyway as we are zeroing the frame counter) */
							(void) bSecuritySetDescriptor(u8Restored++, &sDeviceDescriptor);
							/* Debug */
							DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%sbSecuritySetDescriptor(%d, %x:%x, %d)",
								acDebugIndent,
								u8Restored,
								sDeviceDescriptor.sExtAddr.u32H,
								sDeviceDescriptor.sExtAddr.u32L,
								sDeviceDescriptor.u32FrameCounter);
						}
					}

					/* Start in standalone mode */
					vApi_SetStackMode(STACK_MODE_STANDALONE);
					/* Debug */
					DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%svApi_SetStackMode(%x)", acDebugIndent, STACK_MODE_STANDALONE);
					/* Set to standalone run profile */
					(void) bJnc_SetRunProfile(8, NULL);
					/* Debug */
					DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%sbJnc_SetRunProfile(8)", acDebugIndent);
					/* Skip the normal joining process */
					vApi_SkipJoin(sMibNwkSecurity.sPerm.u16PanId, sMibNwkSecurity.sPerm.u8Channel);
					/* Debug */
					DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%svApi_SkipJoin(0x%x, %d)", acDebugIndent, sMibNwkSecurity.sPerm.u16PanId, sMibNwkSecurity.sPerm.u8Channel);
					/* Note we have attempted to resume */
					bResumed = TRUE;
				}
			}
		}
	}

	/* Didn't resume gateway or standalone mode ? */
	if (FALSE == bResumed)
	{
		tsSecurityKey    sFastKey;

		/* Revert to none up mode */
		sMibNwkStatus.sPerm.u8UpMode = MIB_NWK_STATUS_UP_MODE_NONE;
		/* Not a coordinator ? */
		if (sMibNwkSecurity.sJipInitData.eDeviceType != E_JIP_DEVICE_COORDINATOR)
		{
			/* Did we not set a stored network key earlier ? */
			#if 1
			if (FALSE == bNetworkKey)
			{
				/* Use gateway commissioning key */
				MibNwkSecurity_vSetSecurityKey(MIB_NWK_SECURITY_SECURITY_KEY_GATEWAY_COMMISSIONING);
				/* Set up fast commissioning key */
				sFastKey.u32KeyVal_1 = CONFIG_FAST_COMMISSIONING_KEY_1;
				sFastKey.u32KeyVal_2 = CONFIG_FAST_COMMISSIONING_KEY_2;
				sFastKey.u32KeyVal_3 = CONFIG_FAST_COMMISSIONING_KEY_3;
				sFastKey.u32KeyVal_4 = CONFIG_FAST_COMMISSIONING_KEY_4;
				/* Configure fast commissioning */
				vApi_ConfigureFastCommission(CONFIG_FAST_COMMISSIONING_CHANNEL, CONFIG_FAST_COMMISSIONING_PAN_ID);
				DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%svApi_ConfigureFastCommission(%d, 0x%x)", acDebugIndent, CONFIG_FAST_COMMISSIONING_CHANNEL, CONFIG_FAST_COMMISSIONING_PAN_ID);
				/* Set fast commissioning key */
				vSecurityUpdateKey(2, &sFastKey);
				DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%svSecurityUpdateKey(2, %x:%x:%x:%x)",
						acDebugIndent,
						sFastKey.u32KeyVal_1,
						sFastKey.u32KeyVal_2,
						sFastKey.u32KeyVal_3,
						sFastKey.u32KeyVal_4);
				/* Set flag */
				sMibNwkSecurity.bFastCommissioning = TRUE;
			}
			#endif
		}
		/* Coordinator ? */
		else
		{
			/* Use network key */
			MibNwkSecurity_vSetSecurityKey(MIB_NWK_SECURITY_SECURITY_KEY_NETWORK);
		}
	}

	/* Debug */
	DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%spsNetworkConfigData->u32ScanChannels = 0x%x", acDebugIndent, psNetworkConfigData->u32ScanChannels);
	DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%spsNetworkConfigData->u8Channel       = %d",   acDebugIndent, psNetworkConfigData->u8Channel);
	DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%spsNetworkConfigData->u16PanID        = 0x%x", acDebugIndent, psNetworkConfigData->u16PanID);
	{
		/* Get current key */
		tsSecurityKey *psSecurityKey;
		psSecurityKey = psApi_GetNwkKey();
		/* Debug */
		DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%sKey                                  = %x:%x:%x:%x", acDebugIndent, psSecurityKey->u32KeyVal_1, psSecurityKey->u32KeyVal_2, psSecurityKey->u32KeyVal_3, psSecurityKey->u32KeyVal_4);
	}

	/* Debug */
	DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%ssMibNwkStatus.sPerm.u8UpMode         = %d", acDebugIndent, sMibNwkStatus.sPerm.u8UpMode);
	Node_vDebugOutdent(DEBUG_MIB_NWK_SECURITY);
}

/****************************************************************************
 *
 * NAME: MibNwkSecurity_vSetUserData
 *
 * DESCRIPTION:
 * Puts wanted network id into establish route requests and beacon responses
 *
 ****************************************************************************/
PUBLIC void MibNwkSecurity_vSetUserData(void)
{
	static tsBeaconUserData sBeaconUserData;

	/* Debug */
	DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%sNode_vSetUserData()", acDebugIndent);
	Node_vDebugIndent(DEBUG_MIB_NWK_SECURITY);

	/* Set up user data */
	sBeaconUserData.u32NetworkId  = CONFIG_NETWORK_ID;
	sBeaconUserData.u16DeviceType = MK_JIP_DEVICE_TYPE;

	/* Set beacon payload */
	vApi_SetUserBeaconBits((uint8 *) &sBeaconUserData);
	/* Set up beacon response callback */
	vApi_RegBeaconNotifyCallback(MibNwkSecurity_bBeaconNotifyCallback);
	/* Override scan sort handler (for alternative sorting order) */
	/* vApi_RegScanSortCallback(MibNwkSecurity_bScanSortCallback); */

	/* Set establish route payload */
	v6LP_SetUserData(sizeof(tsBeaconUserData), (uint8 *) &sBeaconUserData);
	/* Set up establish route callback */
	v6LP_SetNwkCallback(MibNwkSecurity_bNwkCallback);

	/* Set device types */
	vJIP_SetDeviceTypes(1, &sBeaconUserData.u16DeviceType);

	/* Debug */
	Node_vDebugOutdent(DEBUG_MIB_NWK_SECURITY);
}

/****************************************************************************
 *
 * NAME: MibNwkSecurity_vRegister
 *
 * DESCRIPTION:
 * Registers MIB
 *
 ****************************************************************************/
PUBLIC void MibNwkSecurity_vRegister(void)
{
	teJIP_Status eStatus;

	/* Debug */
	DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%sMibNwkSecurity_vRegister()", acDebugIndent);
	Node_vDebugIndent(DEBUG_MIB_NWK_SECURITY);
	/* Register MIB */
	eStatus = eJIP_RegisterMib(hMibNwkSecurity);
	/* Debug */
	DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%seJIP_RegisterMib(NwkSecurity) = %d", acDebugIndent, eStatus);
	/* Make sure permament data is saved */
	PDM_vSaveRecord(&sMibNwkSecurity.sDesc);
	/* Debug */
	DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%sPDM_vSaveRecord(MibNwkSecurity) = %d", acDebugIndent, sMibNwkSecurity.sDesc.eState);
	DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, " [Chan=%d, PanId=0x%x, NwkKey=%x:%x:%x:%x]",
		sMibNwkSecurity.sPerm.u8Channel,
		sMibNwkSecurity.sPerm.u16PanId,
		sMibNwkSecurity.sPerm.asSecurityKey[0].u32KeyVal_1,
		sMibNwkSecurity.sPerm.asSecurityKey[0].u32KeyVal_2,
		sMibNwkSecurity.sPerm.asSecurityKey[0].u32KeyVal_3,
		sMibNwkSecurity.sPerm.asSecurityKey[0].u32KeyVal_4);
	/* Debug */
	Node_vDebugOutdent(DEBUG_MIB_NWK_SECURITY);
}

/****************************************************************************
 *
 * NAME: MibNwkSecurity_vMain
 *
 * DESCRIPTION:
 * Callback when processing beacon responses
 * Ignores responses from nodes using a different network ID
 *
 ****************************************************************************/
PUBLIC void MibNwkSecurity_vMain(void)
{
	/* Swapped to standalone mode due to a beacon ? */
	if (sMibNwkSecurity.bStandaloneBeacon == TRUE)
	{
		/* Debug */
		DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%sMibNwkSecurity_vMain() SA_BEACON", acDebugIndent);
		Node_vDebugIndent(DEBUG_MIB_NWK_SECURITY);
		/* Clear flag */
		sMibNwkSecurity.bStandaloneBeacon = FALSE;
		/* Use standalone commissioning key */
		MibNwkSecurity_vSetSecurityKey(MIB_NWK_SECURITY_SECURITY_KEY_STANDALONE_COMMISSIONING);
		/* Debug */
		Node_vDebugOutdent(DEBUG_MIB_NWK_SECURITY);
	}
}

/****************************************************************************
 *
 * NAME: MibNwkSecurity_vSecond
 *
 * DESCRIPTION:
 * Timing function
 *
 ****************************************************************************/
PUBLIC void MibNwkSecurity_vSecond(uint32 u32TimerSeconds)
{
	/* Is a rejoin scheduled ? */
	if (sMibNwkSecurity.u32RejoinSeconds != 0)
	{
		/* Is it time to reset now ? */
		if (u32TimerSeconds >= sMibNwkSecurity.u32RejoinSeconds)
		{
			/* Debug */
			DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%sMibNwkSecurity_vSecond() REJOIN", acDebugIndent);
			Node_vDebugIndent(DEBUG_MIB_NWK_SECURITY);
			/* Zero timers */
			sMibNwkSecurity.u32RejoinSeconds = 0;
			sMibNwkSecurity.sTemp.u16Rejoin  = 0;
			/* Clear out current network information */
			sMibNwkSecurity.sPerm.asSecurityKey[MIB_NWK_SECURITY_SECURITY_KEY_NETWORK].u32KeyVal_1 = 0;
			sMibNwkSecurity.sPerm.asSecurityKey[MIB_NWK_SECURITY_SECURITY_KEY_NETWORK].u32KeyVal_2 = 0;
			sMibNwkSecurity.sPerm.asSecurityKey[MIB_NWK_SECURITY_SECURITY_KEY_NETWORK].u32KeyVal_3 = 0;
			sMibNwkSecurity.sPerm.asSecurityKey[MIB_NWK_SECURITY_SECURITY_KEY_NETWORK].u32KeyVal_4 = 0;
			sMibNwkSecurity.sPerm.u8Channel = 0;
			sMibNwkSecurity.sPerm.u16PanId  = 0;
			/* Perform reset (will save PDMs and force rejoin) */
			Device_vReset(FALSE);
			/* Debug */
			Node_vDebugOutdent(DEBUG_MIB_NWK_SECURITY);
		}
		/* Not yet time to reset ? */
		else
		{
			/* Update countdown */
			sMibNwkSecurity.sTemp.u16Rejoin = sMibNwkSecurity.u32RejoinSeconds - u32TimerSeconds;
		}
	}
}

/****************************************************************************
 *
 * NAME: MibNwkSecurity_vStackEvent
 *
 * DESCRIPTION:
 * Called when stack events take place
 *
 ****************************************************************************/
PUBLIC void MibNwkSecurity_vStackEvent(te6LP_StackEvent eEvent, void *pvData, uint8 u8DataLen)
{
	/* Which event ? */
    switch (eEvent)
    {
    	/* Joined network ? */
    	/* Started network ? */
		case E_STACK_JOINED:
		case E_STACK_STARTED:
		{
			tsNwkInfo *psNwkInfo;

			/* Cast data pointer to correct type */
			psNwkInfo = (tsNwkInfo *) pvData;
			/* Debug */
			DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%sMibNwkSecurity_vStackEvent(JOINED) {%x}", acDebugIndent, u16Api_GetStackMode());
			Node_vDebugIndent(DEBUG_MIB_NWK_SECURITY);

			/* Call set user data function */
			MibNwkSecurity_vSetUserData();

			/* OND ? */
			#ifdef OND_H_INCLUDED
				#ifdef MK_BLD_NODE_TYPE_END_DEVICE
				/* End device ? */
				if (sMibNwkSecurity.sJipInitData.eDeviceType == E_JIP_DEVICE_END_DEVICE)
				{
					/* Debug */
					DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%seOND_SleepingDevInit()", acDebugIndent);
					/* Initialise OND */
					eOND_SleepingDevInit();
					/* Configure OND */
					vOND_SleepConfigure(DEVICE_ED_OND_POLL_DELAY, DEVICE_ED_OND_WAKES_PER_POLL, DEVICE_ED_OND_POLLS_PER_REFRESH);
				}
				else
				#endif
				{
					/* Debug */
					DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%seOND_DevInit()", acDebugIndent);
					/* Initialise OND */
					eOND_DevInit();
				}
		    #else
		    	#warning OND IS DISABLED!!!
		    #endif

			/* Note the channel and PAN ID */
			sMibNwkSecurity.sPerm.u8Channel = psNwkInfo->u8Channel;
			sMibNwkSecurity.sPerm.u16PanId  = psNwkInfo->u16PanID;
			/* Update the network configuration data to limit future rejoins to this channel and PAN ID */
			sMibNwkSecurity.psNetworkConfigData->u32ScanChannels = (1<<psNwkInfo->u8Channel);
			sMibNwkSecurity.psNetworkConfigData->u16PanID		 =     psNwkInfo->u16PanID;

			/* Not a coordinator ? */
			if (sMibNwkSecurity.sJipInitData.eDeviceType != E_JIP_DEVICE_COORDINATOR)
			{
				/* Take a copy of the network key */
				memcpy(&sMibNwkSecurity.sPerm.asSecurityKey[MIB_NWK_SECURITY_SECURITY_KEY_NETWORK], psApi_GetNwkKey(), sizeof(tsSecurityKey));
				/* Invalidate commissioning key */
				vSecurityInvalidateKey(1);
				/* Is fast commissioning enabled ? */
				{
					/* Configure fast commissioning to use network settings */
					vApi_ConfigureFastCommission(sMibNwkSecurity.sPerm.u8Channel, sMibNwkSecurity.sPerm.u16PanId);
					DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%svApi_ConfigureFastCommission(%d, 0x%x)", acDebugIndent, sMibNwkSecurity.sPerm.u8Channel, sMibNwkSecurity.sPerm.u16PanId);
					/* Set fast commissioning key to network key */
					vSecurityUpdateKey(2, &sMibNwkSecurity.sPerm.asSecurityKey[MIB_NWK_SECURITY_SECURITY_KEY_NETWORK]);
					DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%svSecurityUpdateKey(2, %x:%x:%x:%x)",
							acDebugIndent,
							sMibNwkSecurity.sPerm.asSecurityKey[MIB_NWK_SECURITY_SECURITY_KEY_NETWORK].u32KeyVal_1,
							sMibNwkSecurity.sPerm.asSecurityKey[MIB_NWK_SECURITY_SECURITY_KEY_NETWORK].u32KeyVal_2,
							sMibNwkSecurity.sPerm.asSecurityKey[MIB_NWK_SECURITY_SECURITY_KEY_NETWORK].u32KeyVal_3,
							sMibNwkSecurity.sPerm.asSecurityKey[MIB_NWK_SECURITY_SECURITY_KEY_NETWORK].u32KeyVal_4);
					/* Clear flag */
					sMibNwkSecurity.bFastCommissioning = FALSE;
				}
				/* Have we joined in standalone mode ? */
				if (u16Api_GetStackMode() & STACK_MODE_STANDALONE)
				{
					/* Make sure we are not in commissioning mode */
					vApi_SetStackMode(STACK_MODE_STANDALONE);
					/* Debug */
					DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%svApi_SetStackMode(%x)", acDebugIndent, STACK_MODE_STANDALONE);
					/* Add parent as secure address */
					MibNwkSecurity_bAddSecureAddr(&psNwkInfo->sParentAddr);
				}
			}

			/* Note network is up */
			sMibNwkSecurity.bUp = TRUE;
			/* Make sure permament data is saved */
			PDM_vSaveRecord(&sMibNwkSecurity.sDesc);
			/* Debug */
			DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%sPDM_vSaveRecord(MibNwkSecurity) = %d", acDebugIndent, sMibNwkSecurity.sDesc.eState);
			DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, " [Chan=%d, PanId=0x%x, NwkKey=%x:%x:%x:%x]",
				sMibNwkSecurity.sPerm.u8Channel,
				sMibNwkSecurity.sPerm.u16PanId,
				sMibNwkSecurity.sPerm.asSecurityKey[0].u32KeyVal_1,
				sMibNwkSecurity.sPerm.asSecurityKey[0].u32KeyVal_2,
				sMibNwkSecurity.sPerm.asSecurityKey[0].u32KeyVal_3,
				sMibNwkSecurity.sPerm.asSecurityKey[0].u32KeyVal_4);
			/* Debug */
			Node_vDebugOutdent(DEBUG_MIB_NWK_SECURITY);
		}
		break;

		/* Stack reset ? */
		case E_STACK_RESET:
		{
			/* Debug */
			DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%sMibNwkSecurity_vStackEvent(RESET, %d) {%x}", acDebugIndent, sMibNwkSecurity.bUp, u16Api_GetStackMode());
			Node_vDebugIndent(DEBUG_MIB_NWK_SECURITY);

			/* Not a coordinator ? */
			if (sMibNwkSecurity.sJipInitData.eDeviceType != E_JIP_DEVICE_COORDINATOR)
			{
				/* Not joined ? */
				if (sMibNwkSecurity.bUp == FALSE)
				{
					/* Were we trying to join in standalone mode ? */
					if (u16Api_GetStackMode() & STACK_MODE_STANDALONE)
					{
						/* Debug */
						DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, " SA_JOIN_RESET");
						/* Use gateway commissioning key */
						MibNwkSecurity_vSetSecurityKey(MIB_NWK_SECURITY_SECURITY_KEY_GATEWAY_COMMISSIONING);
						/* Set to default run profile (will be overridden upon joining a gateway network) */
						(void) bJnc_SetRunProfile(CONFIG_RUN_PROFILE, NULL);
						/* Debug */
						DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%svApi_SetStackMode(%x)", acDebugIndent,  0);
						/* Swap to gateway mode */
						vApi_SetStackMode(0);
					}
				}
			}
			/* Note network is down */
			sMibNwkSecurity.bUp = FALSE;

			/* Debug */
			Node_vDebugOutdent(DEBUG_MIB_NWK_SECURITY);
		}
		break;

		/* Gateway present ? */
		case E_STACK_NETWORK_ANNOUNCE:
		{
			/* Debug */
			DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%sMibNwkSecurity_vStackEvent(ANNOUNCE, %d) {%x}", acDebugIndent, u16Api_GetStackMode());
			Node_vDebugIndent(DEBUG_MIB_NWK_SECURITY);
			/* In standalone mode ? */
			if (u16Api_GetStackMode() & STACK_MODE_STANDALONE)
			{
				/* Debug */
				DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, " SA_TO_GW");
				/* Pretend we were in a gateway network */
				sMibNwkStatus.sPerm.u8UpMode = MIB_NWK_STATUS_UP_MODE_GATEWAY;
				/* Schedule a rejoin in 5 seconds */
				sMibNwkSecurity.u32RejoinSeconds = Node_u32TimerSeconds() + 5;
			}
			/* Debug */
			Node_vDebugOutdent(DEBUG_MIB_NWK_SECURITY);
		}
		break;


		/* Others ? */
		default:
		{
			/* Do nothing */
			;
		}
		break;
	}
}

/****************************************************************************
 *
 * NAME: MibNwkSecurity_bBeaconNotifyCallback
 *
 * DESCRIPTION:
 * Callback when processing beacon responses
 * Ignores responses from nodes using a different network ID
 *
 ****************************************************************************/
PUBLIC bool_t MibNwkSecurity_bBeaconNotifyCallback(tsScanElement *psBeaconInfo,
                                      			   uint16 	     u16ProtocolVersion)
{
	bool_t bReturn 		= TRUE;
    uint32 u32NetworkId = 0;

    /* Extract network id from request */
    memcpy((uint8 *)&u32NetworkId, psBeaconInfo->au8UserDefined, sizeof(uint32));

	/* Not a coordinator (don't filter results for a coordinator) ? */
	if (sMibNwkSecurity.sJipInitData.eDeviceType != E_JIP_DEVICE_COORDINATOR)
	{
        /* Does it not match the network ID we are using ? */
        if (u32NetworkId != CONFIG_NETWORK_ID)
        {
			/* Discard the beacon */
            bReturn = FALSE;
        }
        else
        {
			/* Joining for first time ? */
			if (sMibNwkStatus.sPerm.u8UpMode == MIB_NWK_STATUS_UP_MODE_NONE)
			{
				/* LQI below preferred level ? */
				if (psBeaconInfo->u8LinkQuality < CONFIG_PRF_BEACON_LQI)
				{
					/* Reject beacon */
					bReturn = FALSE;
				}
			}
			/* Re-joining ? */
			else
			{
				/* LQI below minimum level ? */
				if (psBeaconInfo->u8LinkQuality < CONFIG_MIN_BEACON_LQI)
				{
					/* Reject beacon */
					bReturn = FALSE;
				}
			}
		}

		#if 0
		/* Still going to accept this repsonse ? */
		if (bReturn)
		{
			/* Beacon indicates a standard gateway join profile ? */
			if (psBeaconInfo->u8JoinProfile < 8)
			{
				uint8 au8MaxChildren[] = {10, 16, 10, 16, 10, 16, 10, 16};

				/* Is this node full ? (ASSUMPTION: join profile is same as run profile) */
				if (psBeaconInfo->u8NumChildren >= au8MaxChildren[psBeaconInfo->u8JoinProfile])
				{
					/* Discard this beacon */
					bReturn = FALSE;
				}
			}
		}
		#endif

		/* Still going to accept this repsonse ? */
		if (bReturn)
		{
			/* Is this a response from a device in standalone commissioning mode ? */
			if ((psBeaconInfo->u16StackMode & (STACK_MODE_STANDALONE | STACK_MODE_COMMISSION)) == (STACK_MODE_STANDALONE | STACK_MODE_COMMISSION))
			{
				/* Are we currently in gateway mode ? */
				if ((u16Api_GetStackMode() & STACK_MODE_STANDALONE) == 0)
				{
					/* Flag that we need to complete swapping to standalone mode */
					sMibNwkSecurity.bStandaloneBeacon = TRUE;
					/* Swap to standalone commissioning mode */
					vApi_SetStackMode(STACK_MODE_STANDALONE | STACK_MODE_COMMISSION);
					/* Debug */
					DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%sMibNwkSecurity_bBeaconNotifyCallback { vApi_SetStackMode(%x) }", acDebugIndent, (STACK_MODE_STANDALONE | STACK_MODE_COMMISSION));
				}
			}
		}
	}
	/* Debug */
	DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%sMibNwkSecurity_bBeaconNotifyCallback(%x:%x 0x%x %d %d %d %x%x%x%x%x%x)=%d",
		acDebugIndent,
		psBeaconInfo->sExtAddr.u32H,
		psBeaconInfo->sExtAddr.u32L,
		psBeaconInfo->u16PanId,
		psBeaconInfo->u16Depth,
		psBeaconInfo->u8Channel,
		psBeaconInfo->u8LinkQuality,
		psBeaconInfo->au8UserDefined[0],
		psBeaconInfo->au8UserDefined[1],
		psBeaconInfo->au8UserDefined[2],
		psBeaconInfo->au8UserDefined[3],
		psBeaconInfo->au8UserDefined[4],
		psBeaconInfo->au8UserDefined[5],
		bReturn);

    return bReturn;
}

/****************************************************************************
 *
 * NAME: MibNwkSecurity_bScanSortCallback
 *
 * DESCRIPTION:
 * Sorts incoming beacon repsonses into preferred order
 *
 ****************************************************************************/
 PUBLIC bool_t MibNwkSecurity_bScanSortCallback(tsScanElement *pasScanResult,
                            			 		uint8 			 u8ScanListSize,
                            			 		uint8 		  *pau8ScanListOrder)
{
	bool_t bReturn = FALSE;

    uint8   i,n,tmp;
    bool_t  bSwapped;

	/* Debug */
	DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%sMibNwkSecurity_bScanSortCallback(%d)", acDebugIndent, u8ScanListSize);
	Node_vDebugIndent(DEBUG_MIB_NWK_SECURITY);

    if (u8ScanListSize > 0)
    {
		n = u8ScanListSize;

		/* simple bubblesort for the time being, since it's only a small list */
		do
		{
			bSwapped = FALSE;
			n = n - 1;

			for (i=0;i<n;i++)
			{
				/* sort order is depth, loading and link quality in that order */
				if (MibNwkSecurity_bScanSortCheckSwap(pasScanResult, i, pau8ScanListOrder))
				{
					/* to make sort quicker we'll find the final order first,
					   then copy all the data later */
					tmp = pau8ScanListOrder[i];
					pau8ScanListOrder[i] = pau8ScanListOrder[i+1];
					pau8ScanListOrder[i+1] = tmp;

					bSwapped = TRUE;
				}
			}
		} while (bSwapped);

		/* Loop through entries */
		for (i = 0; i < u8ScanListSize; i++)
		{
			/* Debug */
			DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%spasScanResult[%d] = {%x:%x 0x%x %d %d %d %x%x%x%x%x%x}",
				acDebugIndent,
				i,
				pasScanResult[pau8ScanListOrder[i]].sExtAddr.u32H,
				pasScanResult[pau8ScanListOrder[i]].sExtAddr.u32L,
				pasScanResult[pau8ScanListOrder[i]].u16PanId,
				pasScanResult[pau8ScanListOrder[i]].u16Depth,
				pasScanResult[pau8ScanListOrder[i]].u8Channel,
				pasScanResult[pau8ScanListOrder[i]].u8LinkQuality,
				pasScanResult[pau8ScanListOrder[i]].au8UserDefined[0],
				pasScanResult[pau8ScanListOrder[i]].au8UserDefined[1],
				pasScanResult[pau8ScanListOrder[i]].au8UserDefined[2],
				pasScanResult[pau8ScanListOrder[i]].au8UserDefined[3],
				pasScanResult[pau8ScanListOrder[i]].au8UserDefined[4],
				pasScanResult[pau8ScanListOrder[i]].au8UserDefined[5]);
		}
	}

	/* Debug */
	Node_vDebugOutdent(DEBUG_MIB_NWK_SECURITY);

	return bReturn;
}

/****************************************************************************
 *
 * NAME: MibNwkSecurity_bScanSortCheckSwap
 *
 * DESCRIPTION:
 * Applies scan sort algorithm by checking if two beacons need to be reordered
 *
 ****************************************************************************/
PUBLIC bool_t MibNwkSecurity_bScanSortCheckSwap(tsScanElement *pasScanResult,
                                  				uint8 		    u8ScanListItem,
                                  				uint8 	     *pau8ScanListOrder)
{
    uint8 j,k;

    j = pau8ScanListOrder[u8ScanListItem];   // GNATS PR438
    k = pau8ScanListOrder[u8ScanListItem+1];

    /* LQI of both items is above preferred LQI ? */
    if (pasScanResult[j].u8LinkQuality >= MIB_NWK_SECURITY_SCAN_SORT_PIVOT_LQI &&
        pasScanResult[k].u8LinkQuality >= MIB_NWK_SECURITY_SCAN_SORT_PIVOT_LQI)
    {
		/* Using weighted sorting algorithm ? */
		#if MIB_NWK_SECURITY_SCAN_SORT_PIVOT_WEIGHTED
		{
			uint32 u32WeightJ;
	   	 	uint32 u32WeightK;

			/* Calculate weights (lower is better) */
			u32WeightJ = (pasScanResult[j].u16Depth * 2) + pasScanResult[j].u8NumChildren;
			u32WeightK = (pasScanResult[k].u16Depth * 2) + pasScanResult[k].u8NumChildren;

			/* Sort by weight, LQI */
			if (u32WeightJ > u32WeightK)
			{
				return TRUE;
			}
			else if ((u32WeightJ == u32WeightK) &&
					 (pasScanResult[j].u8LinkQuality < pasScanResult[k].u8LinkQuality) )
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
		/* Using standard sorting algorithm ? */
		#else
		{
			/* Sort by depth, children, LQI */
			if (pasScanResult[j].u16Depth > pasScanResult[k].u16Depth)
			{
				return TRUE;
			}
			else if ( (pasScanResult[j].u16Depth == pasScanResult[k].u16Depth) &&
					  (pasScanResult[j].u8NumChildren > pasScanResult[k].u8NumChildren) )
			{
				return TRUE;
			}
			else if ( (pasScanResult[j].u16Depth == pasScanResult[k].u16Depth) &&
					  (pasScanResult[j].u8NumChildren == pasScanResult[k].u8NumChildren) &&
					  (pasScanResult[j].u8LinkQuality < pasScanResult[k].u8LinkQuality) )
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
		#endif
	}
    /* LQI of both items is below preferred LQI ? */
    else if (pasScanResult[j].u8LinkQuality < (uint8) MIB_NWK_SECURITY_SCAN_SORT_PIVOT_LQI &&
        	 pasScanResult[k].u8LinkQuality < (uint8) MIB_NWK_SECURITY_SCAN_SORT_PIVOT_LQI)
    {
		/* Sort by LQI, depth, children */
		if (pasScanResult[j].u8LinkQuality < pasScanResult[k].u8LinkQuality)
		{
			return TRUE;
		}
		else if ( (pasScanResult[j].u8LinkQuality == pasScanResult[k].u8LinkQuality) &&
				  (pasScanResult[j].u16Depth > pasScanResult[k].u16Depth) )
		{
			return TRUE;
		}
		else if ( (pasScanResult[j].u8LinkQuality == pasScanResult[k].u8LinkQuality) &&
				  (pasScanResult[j].u16Depth == pasScanResult[k].u16Depth) &&
				  (pasScanResult[j].u8NumChildren > pasScanResult[k].u8NumChildren) )
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	/* LQI of first item is below preferred LQI while second is above ? */
	else if (pasScanResult[j].u8LinkQuality <  (uint8) MIB_NWK_SECURITY_SCAN_SORT_PIVOT_LQI &&
        	 pasScanResult[k].u8LinkQuality >= (uint8) MIB_NWK_SECURITY_SCAN_SORT_PIVOT_LQI)
    {
		return TRUE;
    }
    /* items are ok */
    else
    {
    	return FALSE;
    }
}

/****************************************************************************
 *
 * NAME: MibNwkSecurity_bNwkCallback
 *
 * DESCRIPTION:
 * Callback when processing establish route request
 * Rejects requests from nodes using a different network ID
 *
 ****************************************************************************/
PUBLIC bool_t MibNwkSecurity_bNwkCallback(MAC_ExtAddr_s *psAddr,
 			                                 uint8 		   u8DataLength,
                            				 uint8 		 *pu8Data)
{
	bool_t             bReturn		  = FALSE;
	tsEstablishRouteUserData *psEstablishRouteUserData;

	/* Take a user data pointer to the data */
	psEstablishRouteUserData = (tsEstablishRouteUserData *) pu8Data;

	/* There is at least a uint32 in the data ? */
    if (u8DataLength >= sizeof(uint32))
    {
        /* Does it match the network ID we are using ? */
        if (psEstablishRouteUserData->u32NetworkId == CONFIG_NETWORK_ID)
        {
			/* Allow the establish route */
            bReturn = TRUE;
        }
    }

    return bReturn;
}

/****************************************************************************
 *
 * NAME: MibNwkSecurity_bAddSecureAddr
 *
 * DESCRIPTION:
 * Reserve a secure slot for a mac address
 *
 ****************************************************************************/
PUBLIC bool_t MibNwkSecurity_bAddSecureAddr(MAC_ExtAddr_s *psMacAddr)
{
	uint8 u8Found = 0xff;
	uint8 u8Free  = 0xff;
	uint8 u8SecureAddress;
	bool_t bReturn = TRUE;

	/* Loop through secure addresses looking for the passed in address and an unused slot */
	for (u8SecureAddress = 0;
		 u8SecureAddress < MIB_NWK_SECURITY_SECURE_ADDR_COUNT;
		 u8SecureAddress++)
	{
		/* Is this the device we are looking for ? */
		if (memcmp(&sMibNwkSecurity.sPerm.asSecureAddr[u8SecureAddress], psMacAddr, sizeof(MAC_ExtAddr_s)) == 0)
		{
			/* Note we've found it */
			u8Found = u8SecureAddress;
		}
		/* Is this a blank address and we've not found one already ? */
		if (sMibNwkSecurity.sPerm.asSecureAddr[u8SecureAddress].u32H == 0 &&
			sMibNwkSecurity.sPerm.asSecureAddr[u8SecureAddress].u32L == 0 &&
			u8Free == 0xff)
		{
			/* Note the free slot */
			u8Free = u8SecureAddress;
		}
	}

	/* Did we not find the address already and we have an unused slot ? */
	if (u8Found == 0xff && u8Free != 0xff)
	{
		/* Store the address in the free slot */
		memcpy(&sMibNwkSecurity.sPerm.asSecureAddr[u8Free], psMacAddr, sizeof(MAC_ExtAddr_s));
		/* Make sure permament data is saved */
		PDM_vSaveRecord(&sMibNwkSecurity.sDesc);
		/* Debug */
		DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%sMibNwkSecurity_bAddSecureAddr { PDM_vSaveRecord(MibNwkSecurity) = %d }", acDebugIndent, sMibNwkSecurity.sDesc.eState);
		DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, " [Chan=%d, PanId=0x%x, NwkKey=%x:%x:%x:%x]",
			sMibNwkSecurity.sPerm.u8Channel,
			sMibNwkSecurity.sPerm.u16PanId,
			sMibNwkSecurity.sPerm.asSecurityKey[0].u32KeyVal_1,
			sMibNwkSecurity.sPerm.asSecurityKey[0].u32KeyVal_2,
			sMibNwkSecurity.sPerm.asSecurityKey[0].u32KeyVal_3,
			sMibNwkSecurity.sPerm.asSecurityKey[0].u32KeyVal_4);
	}
	/* We wanted to add the address but didnt have space ? */
	else if (u8Found == 0xff && u8Free == 0xff)
	{
		/* Failed */
		bReturn = FALSE;
	}

	return bReturn;
}

/****************************************************************************
 *
 * NAME: MibNwkSecurity_bDelSecureAddr
 *
 * DESCRIPTION:
 * Unreserve a secure slot for a mac address
 *
 ****************************************************************************/
PUBLIC bool_t MibNwkSecurity_bDelSecureAddr(MAC_ExtAddr_s *psMacAddr)
{
	uint8 u8SecureAddress;

	/* Loop through secure addresses looking for the passed in address and an unused slot */
	for (u8SecureAddress = 0;
		 u8SecureAddress < MIB_NWK_SECURITY_SECURE_ADDR_COUNT;
		 u8SecureAddress++)
	{
		/* Is this the device we are looking for ? */
		if (memcmp(&sMibNwkSecurity.sPerm.asSecureAddr[u8SecureAddress], psMacAddr, sizeof(MAC_ExtAddr_s)) == 0)
		{
			/* Blank the address */
			sMibNwkSecurity.sPerm.asSecureAddr[u8SecureAddress].u32H = 0;
			sMibNwkSecurity.sPerm.asSecureAddr[u8SecureAddress].u32L = 0;
			/* Make sure permament data is saved */
			PDM_vSaveRecord(&sMibNwkSecurity.sDesc);
			/* Debug */
			DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%sMibNwkSecurity_bDelSecureAddr { PDM_vSaveRecord(MibNwkSecurity) = %d }", acDebugIndent, sMibNwkSecurity.sDesc.eState);
			DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, " [Chan=%d, PanId=0x%x, NwkKey=%x:%x:%x:%x]",
				sMibNwkSecurity.sPerm.u8Channel,
				sMibNwkSecurity.sPerm.u16PanId,
				sMibNwkSecurity.sPerm.asSecurityKey[0].u32KeyVal_1,
				sMibNwkSecurity.sPerm.asSecurityKey[0].u32KeyVal_2,
				sMibNwkSecurity.sPerm.asSecurityKey[0].u32KeyVal_3,
				sMibNwkSecurity.sPerm.asSecurityKey[0].u32KeyVal_4);
		}
	}

	return TRUE;
}

/****************************************************************************
 *
 * NAME: MibNwkSecurity_vResetSecureAddr
 *
 * DESCRIPTION:
 * Reset all secure address slots
 *
 ****************************************************************************/
PUBLIC void MibNwkSecurity_vResetSecureAddr(void)
{
	uint8 u8SecureAddress;

	/* Loop through secure addresses looking for the passed in address and an unused slot */
	for (u8SecureAddress = 0;
		 u8SecureAddress < MIB_NWK_SECURITY_SECURE_ADDR_COUNT;
		 u8SecureAddress++)
	{
		/* Blank the address */
		sMibNwkSecurity.sPerm.asSecureAddr[u8SecureAddress].u32H = 0;
		sMibNwkSecurity.sPerm.asSecureAddr[u8SecureAddress].u32L = 0;
	}
	/* Make sure permament data is saved */
	PDM_vSaveRecord(&sMibNwkSecurity.sDesc);
	/* Debug */
	DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%sMibNwkSecurity_vResetSecureAddr { PDM_vSaveRecord(MibNwkSecurity) = %d }", acDebugIndent, sMibNwkSecurity.sDesc.eState);
	DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, " [Chan=%d, PanId=0x%x, NwkKey=%x:%x:%x:%x]",
		sMibNwkSecurity.sPerm.u8Channel,
		sMibNwkSecurity.sPerm.u16PanId,
		sMibNwkSecurity.sPerm.asSecurityKey[0].u32KeyVal_1,
		sMibNwkSecurity.sPerm.asSecurityKey[0].u32KeyVal_2,
		sMibNwkSecurity.sPerm.asSecurityKey[0].u32KeyVal_3,
		sMibNwkSecurity.sPerm.asSecurityKey[0].u32KeyVal_4);
}

/****************************************************************************
 *
 * NAME: MibNwkSecurity_vSetSecurityKey
 *
 * DESCRIPTION:
 * Set security key to apply
 *
 ****************************************************************************/
PUBLIC void MibNwkSecurity_vSetSecurityKey(uint8 u8Key)
{
	/* Debug */
	DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%sMibNwkSecurity_vSetSecurityKey(%d)", acDebugIndent, u8Key);
	/* Valid key ? */
	if (u8Key < 3)
	{
		/* Setting network key ? */
		if (u8Key == MIB_NWK_SECURITY_SECURITY_KEY_NETWORK)
		{
			/* Invalidate commissioning key */
			vSecurityInvalidateKey(1);
			/* Apply network key */
			vApi_SetNwkKey(0, &sMibNwkSecurity.sPerm.asSecurityKey[u8Key]);
		}
		/* Setting a commissioning key ? */
		else
		{
			/* Invalidate unused key */
			vSecurityInvalidateKey(0);
			/* Apply commissioning key */
			vApi_SetNwkKey(1, &sMibNwkSecurity.sPerm.asSecurityKey[u8Key]);
		}
	}
}

/****************************************************************************
 *
 * NAME: MibNwkSecurity_eSetKey
 *
 * DESCRIPTION:
 * Handle remote set of a key
 *
 ****************************************************************************/
PUBLIC teJIP_Status MibNwkSecurity_eSetKey(const uint8 *pu8Val, uint8 u8Len, void *pvCbData)
{
	bool_t bReturn = FALSE;

	/* Has the correct amount of data being passed in ? */
	if (u8Len == sizeof(tsSecurityKey))
	{
		/* Copy data */
		memcpy(pvCbData, pu8Val, u8Len);
		/* Success */
		bReturn = TRUE;
		/* Make sure permament data is saved */
		PDM_vSaveRecord(&sMibNwkSecurity.sDesc);
		/* Debug */
		DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, "\n%sMibNwkSecurity_eSetKey { PDM_vSaveRecord(MibNwkSecurity) = %d }", acDebugIndent, sMibNwkSecurity.sDesc.eState);
		DBG_vPrintf(DEBUG_MIB_NWK_SECURITY, " [Chan=%d, PanId=0x%x, NwkKey=%x:%x:%x:%x]",
			sMibNwkSecurity.sPerm.u8Channel,
			sMibNwkSecurity.sPerm.u16PanId,
			sMibNwkSecurity.sPerm.asSecurityKey[0].u32KeyVal_1,
			sMibNwkSecurity.sPerm.asSecurityKey[0].u32KeyVal_2,
			sMibNwkSecurity.sPerm.asSecurityKey[0].u32KeyVal_3,
			sMibNwkSecurity.sPerm.asSecurityKey[0].u32KeyVal_4);
	}

	return bReturn;
}

/****************************************************************************
 *
 * NAME: MibNwkSecurity_vGetKey
 *
 * DESCRIPTION:
 * Handle remote get of a key
 *
 ****************************************************************************/
PUBLIC void MibNwkSecurity_vGetKey(thJIP_Packet hPacket, void *pvCbData)
{
	eJIP_PacketAddData(hPacket, pvCbData, sizeof(tsSecurityKey), 0);
}

/****************************************************************************
 *
 * NAME: MibNwkSecurity_eSetReset
 *
 * DESCRIPTION:
 * Sets Reset variable
 *
 ****************************************************************************/
PUBLIC teJIP_Status MibNwkSecurity_eSetRejoin(uint16 u16Val, void *pvCbData)
{
    teJIP_Status eReturn;

    /* Call standard function */
    eReturn = eSetUint16(u16Val, pvCbData);
    /* Clearing rejoin timer ? */
    if (u16Val == 0)
    {
		/* Clear the reset time */
		sMibNwkSecurity.u32RejoinSeconds = 0;
	}
	/* Setting reset timer ? */
	else
	{
		/* Calculate reset time */
		sMibNwkSecurity.u32RejoinSeconds = Node_u32TimerSeconds() + u16Val;
	}

    return eReturn;
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
