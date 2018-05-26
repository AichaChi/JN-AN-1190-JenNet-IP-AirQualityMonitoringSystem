/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         NwkSecurity MIB - Interface
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
#ifndef  MIBNWKSECURITY_H_INCLUDED
#define  MIBNWKSECURITY_H_INCLUDED

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
/* Stack includes */
#include <Sec2006.h>

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define MIB_NWK_SECURITY_SECURE_ADDR_COUNT						8

#define MIB_NWK_SECURITY_SECURITY_KEY_NETWORK					0
#define MIB_NWK_SECURITY_SECURITY_KEY_GATEWAY_COMMISSIONING		1
#define MIB_NWK_SECURITY_SECURITY_KEY_STANDALONE_COMMISSIONING	2

#define MIB_NWK_SECURITY_RESET_NONE					0
#define MIB_NWK_SECURITY_RESET_FACTORY				1
#define MIB_NWK_SECURITY_RESET_CHIP	    			2
#define MIB_NWK_SECURITY_RESET_STACK_TO_GATEWAY 	3
#define MIB_NWK_SECURITY_RESET_STACK_TO_STANDALONE 	4

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
/* Network security permament data */
typedef struct
{
	uint8 				   u8Channel;
	uint16				  u16PanId;
	tsSecurityKey	  	  asSecurityKey[3];
	MAC_ExtAddr_s		   asSecureAddr[MIB_NWK_SECURITY_SECURE_ADDR_COUNT];

} tsMibNwkSecurityPerm;

/* Network security temporary data */
typedef struct
{
	uint16 u16Rejoin;

} tsMibNwkSecurityTemp;

/* Network status mib */
typedef struct
{
	/* PDM record descriptor */
	PDM_tsRecordDescriptor   sDesc;

	/* Data structures */
	tsMibNwkSecurityPerm 	 sPerm;
	tsMibNwkSecurityTemp 	 sTemp;

	/* Other data */
	bool_t 				  bUp;
	tsJIP_InitData        sJipInitData;
	tsNetworkConfigData *psNetworkConfigData;
	bool_t 				  bStandaloneBeacon;
	bool_t				  bFastCommissioning;
	uint32				u32RejoinSeconds;

} tsMibNwkSecurity;

/****************************************************************************/
/***        Public Function Prototypes                                     ***/
/****************************************************************************/
/* Unpatched functions in ROM / Libraries */
PUBLIC void 			MibNwkSecurity_vInit(void);
PUBLIC teJIP_Status		MibNwkSecurity_eJipInit(void);
PUBLIC void 			MibNwkSecurity_v6lpConfigureNetwork(tsNetworkConfigData *psNetworkConfigData);
PUBLIC void 			MibNwkSecurity_vSetUserData(void);
PUBLIC void 			MibNwkSecurity_vRegister(void);
PUBLIC void 			MibNwkSecurity_vStackEvent(te6LP_StackEvent eEvent, void *pvData, uint8 u8DataLen);
PUBLIC void 			MibNwkSecurity_vMain(void);
PUBLIC void 			MibNwkSecurity_vSecond(uint32 u32TimerSeconds);
PUBLIC bool_t			MibNwkSecurity_bAddSecureAddr(MAC_ExtAddr_s *psMacAddr);
PUBLIC bool_t			MibNwkSecurity_bDelSecureAddr(MAC_ExtAddr_s *psMacAddr);
PUBLIC void 			MibNwkSecurity_vResetSecureAddr(void);
PUBLIC void 			MibNwkSecurity_vSetSecurityKey(uint8 u8Key);
PUBLIC bool_t MibNwkSecurity_bBeaconNotifyCallback(tsScanElement *psBeaconInfo,
                                      			   uint16 	     u16ProtocolVersion);
PUBLIC bool_t MibNwkSecurity_bScanSortCallback(tsScanElement *pasScanResult,
                            			 	   uint8 			 u8ScanListSize,
                            			 	   uint8 		  *pau8ScanListOrder);
PUBLIC bool_t MibNwkSecurity_bScanSortCheckSwap(tsScanElement *pasScanResult,
                                  				uint8 		    u8ScanListItem,
                                  				uint8 	     *pau8ScanListOrder);
PUBLIC bool_t MibNwkSecurity_bNwkCallback(MAC_ExtAddr_s *psAddr,
 			                                 uint8 		   u8DataLength,
                            				 uint8 		 *pu8Data);
PUBLIC teJIP_Status 	MibNwkSecurity_eSetKey(const uint8 *pu8Val, uint8 u8Len, void *pvCbData);
PUBLIC void 			MibNwkSecurity_vGetKey(thJIP_Packet hPacket, void *pvCbData);
PUBLIC teJIP_Status 	MibNwkSecurity_eSetRejoin(uint16 u16Val, void *pvCbData);

#if defined __cplusplus
}
#endif

#endif  /* MIBNWKSECURITY_H_INCLUDED */
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
