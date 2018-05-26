/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         Protocol
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
#ifndef  PROTOCOL_H_INCLUDED
#define  PROTOCOL_H_INCLUDED

#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
/* SDK includes */
#include <jendefs.h>
#include <Api.h>
#include <JIP.h>
#include <6LP.h>

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define PROTOCOL_PORT				 	 (1190)
#define PROTOCOL_GROUP				 	0x1190

#define PROTOCOL_CMD_REG_REQ 			    'R'
#define PROTOCOL_CMD_REG_RSP			    'r'

#define PROTOCOL_CMD_REG_REQ_SECONDS	    15

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
/* Command structures */
typedef struct
{
	char		    cCmd;
} tsProtocolCmdRegReq;

typedef struct
{
	char		    cCmd;
} tsProtocolCmdRegRsp;

/****************************************************************************/
/***        Public Function Prototypes                                    ***/
/****************************************************************************/
PUBLIC void Protocol_vInit(bool_t bWarmStart);
PUBLIC void Protocol_vJipStackEvent(te6LP_StackEvent eEvent, uint8 *pu8Data, uint8 u8DataLen);
PUBLIC void Protocol_v6lpDataEvent(int iSocket, te6LP_DataEvent  eEvent, ts6LP_SockAddr *psAddr, uint8 u8AddrLen);
PUBLIC void Protocol_vMain(void);
PUBLIC void Protocol_vSecond(uint32 u32TimerSeconds);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

#if defined __cplusplus
}
#endif

#endif  /* PROTOCOL_H_INCLUDED */
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
