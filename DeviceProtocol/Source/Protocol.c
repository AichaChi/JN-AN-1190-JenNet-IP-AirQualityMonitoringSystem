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

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

/* SDK includes */
#include <jendefs.h>
#include <JIP.h>
#include <6LP.h>
#include <string.h>
/* Application includes */
#include "DeviceDefs.h"
#include "Node.h"
#include "Address.h"
#include "Protocol.h"
//#include "Uart.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define PROTOCOL_CMD_SEND_REG_REQ 0x1

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
PRIVATE void  Protocol_vOpenSocket(void);
PRIVATE void  Protocol_vSendTo(ts6LP_SockAddr *ps6lpSockAddr, uint8 *pu8Data, uint16 u16DataLen);
#ifdef MK_BLD_NODE_TYPE_COORDINATOR
/* Coordinator only functions */
PRIVATE void  Protocol_vSendCmdRegRsp(ts6LP_SockAddr *ps6lpSockAddr);
PRIVATE void  Protocol_vRecvCmdRegReq(ts6LP_SockAddr *ps6lpSockAddr, tsProtocolCmdRegReq *psProtocolCmdRegReq);
#else
/* Non-coordinator functions */
PRIVATE void  Protocol_vSendCmdRegReq(ts6LP_SockAddr *ps6lpSockAddr);
PRIVATE void  Protocol_vRecvCmdRegRsp(ts6LP_SockAddr *ps6lpSockAddr, tsProtocolCmdRegRsp *psProtocolCmdRegRsp);
#endif

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
PRIVATE bool_t          	  bProtocolSocket;
PRIVATE int					  iProtocolSocket;
PRIVATE ts6LP_SockAddr 		  s6lpSockAddrCoord;
PRIVATE ts6LP_SockAddr 		  s6lpSockAddrNetwork;
PRIVATE ts6LP_SockAddr 		  s6lpSockAddrUnicast;
PRIVATE ts6LP_SockAddr 		  s6lpSockAddrLocal;
/* Non-coordinator ? */
#ifndef MK_BLD_NODE_TYPE_COORDINATOR
PRIVATE uint32				u32CmdRegReqSecond;
PRIVATE uint32				u32CmdSend;
#endif


/****************************************************************************/
/***        External Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: Protocol_vInit
 *
 * DESCRIPTION:
 * Initialisation
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void Protocol_vInit(bool_t bWarmStart)
{
	/* Cold start ? */
	if (FALSE == bWarmStart)
	{
		/* Debug */
		DBG_vPrintf(DEBUG_PROTOCOL, "\n%sProtocol_vInit(COLD)", acDebugIndent);

		/* Build common part of coordinator socket address */
		s6lpSockAddrCoord.sin6_family   = E_6LP_PF_INET6;
		s6lpSockAddrCoord.sin6_flowinfo = 0;
		s6lpSockAddrCoord.sin6_port     = PROTOCOL_PORT;
		s6lpSockAddrCoord.sin6_scope_id = 0;
		/* Build coordinator address */
		s6lpSockAddrCoord.sin6_addr.s6_addr32[0] =
		s6lpSockAddrCoord.sin6_addr.s6_addr32[1] =
		s6lpSockAddrCoord.sin6_addr.s6_addr32[2] =
		s6lpSockAddrCoord.sin6_addr.s6_addr32[3] = 0;

		/* Build common part of network socket address */
		s6lpSockAddrNetwork.sin6_family   = E_6LP_PF_INET6;
		s6lpSockAddrNetwork.sin6_flowinfo = 0;
		s6lpSockAddrNetwork.sin6_port     = PROTOCOL_PORT;
		s6lpSockAddrNetwork.sin6_scope_id = 0;
		/* Build network address */
		Address_vBuildGroup(&s6lpSockAddrNetwork.sin6_addr, NULL, PROTOCOL_GROUP);

		/* Build common part of unicast socket address */
		s6lpSockAddrUnicast.sin6_family   = E_6LP_PF_INET6;
		s6lpSockAddrUnicast.sin6_flowinfo = 0;
		s6lpSockAddrUnicast.sin6_port     = PROTOCOL_PORT;
		s6lpSockAddrUnicast.sin6_scope_id = 0;
	}
}

/****************************************************************************
 *
 * NAME: Protocol_vJipStackEvent
 *
 * DESCRIPTION:
 * Processes any incoming stack events.
 *
 * PARAMETERS: Name          RW Usage
 *             eEvent        R  Stack event
 *             pu8Data       R  Additional information associated with event
 *             u8DataLen     R  Length of additional information
 *
 ****************************************************************************/
PUBLIC void Protocol_vJipStackEvent(te6LP_StackEvent eEvent, uint8 *pu8Data, uint8 u8DataLen)
{
	/* Which event ? */
    switch (eEvent)
    {
		/* Coordinator only events ? */
		#ifdef MK_BLD_NODE_TYPE_COORDINATOR

    	/* Started network ? */
		case E_STACK_STARTED:
		{
			/* Debug */
			DBG_vPrintf(DEBUG_PROTOCOL, "\n%sProtocol_vJipStackEvent(STARTED)", acDebugIndent);
			Node_vDebugIndent(DEBUG_PROTOCOL);

			/* Open socket */
			Protocol_vOpenSocket();

			/* Debug */
			Node_vDebugOutdent(DEBUG_PROTOCOL);
		}
		break;

		/* Non-coordinator events */
		#else

    	/* Joined network ? */
		case E_STACK_JOINED:
		{
			/* Debug */
			DBG_vPrintf(DEBUG_PROTOCOL, "\n%sProtocol_vJipStackEvent(JOINED)", acDebugIndent);
			Node_vDebugIndent(DEBUG_PROTOCOL);

			/* Open socket */
			Protocol_vOpenSocket();

			/* Set flag to send register request command */
			U32_SET_BITS(&u32CmdSend, PROTOCOL_CMD_SEND_REG_REQ);

			/* Debug */
			Node_vDebugOutdent(DEBUG_PROTOCOL);
		}
		break;

		#endif

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
 * NAME: Protocol_v6lpDataEvent
 *
 * DESCRIPTION:
 * As this app uses JIP for all communication we are not interested in the
 * 6LP data events. For any receive events we simply discard the packet to
 * free the packet buffer.
 *
 * PARAMETERS: Name        RW  Usage
 *             iSocket     R   Socket on which packet received
 *             eEvent      R   Data event
 *             psAddr      R   Source address (for RX) or destination (for TX)
 *             u8AddrLen   R   Length of address
 *
 ****************************************************************************/
PUBLIC void Protocol_v6lpDataEvent(  int 			  iSocket,
								  	 te6LP_DataEvent  eEvent,
            	                  	 ts6LP_SockAddr *psAddr,
            	                  	 uint8           u8AddrLen)
{
	/* Which event ? */
	switch (eEvent)
	{
		/* Data sent ? */
		case E_DATA_SENT:
		{
			/* Debug */
			DBG_vPrintf(DEBUG_PROTOCOL, "\n%sProtocol_vJipDataEvent(SENT)", acDebugIndent);
		}
		break;

		/* Data send failed ? */
		case E_DATA_SEND_FAILED:
		{
			/* Debug */
			DBG_vPrintf(DEBUG_PROTOCOL, "\n%sProtocol_vJipDataEvent(FAILED)", acDebugIndent);
		}
		break;

		/* Data received ? */
		case E_DATA_RECEIVED:
		{
			int					iRecvFromResult;
			ts6LP_SockAddr 	    s6lpSockAddr;
			uint8		       u8SockAddrLen;
			uint8    		  au8Data[256];
			void              *pvData = (void *) au8Data;

			/* Debug */
			DBG_vPrintf(DEBUG_PROTOCOL, "\n%sProtocol_vJipDataEvent(RECEIVED)", acDebugIndent);
			Node_vDebugIndent(DEBUG_PROTOCOL);

			/* Read 6LP packet */
			iRecvFromResult = i6LP_RecvFrom(iSocket, au8Data, 256, 0, &s6lpSockAddr, &u8SockAddrLen);
			/* Debug */
			DBG_vPrintf(DEBUG_PROTOCOL, "\n%si6LP_RecvFrom(0x%x, %c) = %d", acDebugIndent, s6lpSockAddr.sin6_addr.s6_addr32[3], au8Data[0], iRecvFromResult);

			/* Error ? */
			if (iRecvFromResult == -1)
			{
				uint32 u32ErrNo;
				/* Get error no */
				u32ErrNo = u32_6LP_GetErrNo();
				/* Debug */
				DBG_vPrintf(DEBUG_PROTOCOL, " {0x%x}", u32ErrNo);
			}
			/* Success ? */
			else
			{
				/* Coordinator build ? */
				#ifdef MK_BLD_NODE_TYPE_COORDINATOR
				{
					/* Which command ? */
					switch (au8Data[0])
					{
						/* Register request ? */
						case PROTOCOL_CMD_REG_REQ:  Protocol_vRecvCmdRegReq(&s6lpSockAddr, (tsProtocolCmdRegReq *) pvData); break;
						/* Default ? */
						default: break;
					}
				}
				/* Non-coordinator build ? */
				#else
				{
					/* Which command ? */
					switch (au8Data[0])
					{
						/* Register response ? */
						case PROTOCOL_CMD_REG_RSP:  Protocol_vRecvCmdRegRsp(&s6lpSockAddr, (tsProtocolCmdRegRsp *) pvData); break;
						/* Default ? */
						default: break;
					}
				}
				#endif
			}

			/* Debug */
			Node_vDebugOutdent(DEBUG_PROTOCOL);
		}
		break;

		/* IP data received ? */
		case E_IP_DATA_RECEIVED:
		{
			/* Debug */
			DBG_vPrintf(DEBUG_PROTOCOL, "\n%sProtocol_vJipDataEvent(DATA_RECEIVED)", acDebugIndent);

			/* Discard 6LP packets as only interested in JIP communication  */
			i6LP_RecvFrom(iSocket, NULL, 0, 0, NULL, NULL);
		}
		break;

		/* 6LP ICMP message ? */
		case E_6LP_ICMP_MESSAGE:
		{
			/* Debug */
			DBG_vPrintf(DEBUG_PROTOCOL, "\n%sProtocol_vJipDataEvent(ICMP_MESSAGE)", acDebugIndent);

			/* Discard 6LP packets as only interested in JIP communication  */
			i6LP_RecvFrom(iSocket, NULL, 0, 0, NULL, NULL);
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
 * NAME: Protocol_vMain
 *
 * DESCRIPTION:
 * Timer function
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void Protocol_vMain(void)
{
	/* Non-coordinator ? */
	#ifndef MK_BLD_NODE_TYPE_COORDINATOR
	{
		/* Need to send register request ? */
		if (u32CmdSend & PROTOCOL_CMD_SEND_REG_REQ)
		{
			/* Debug */
			DBG_vPrintf(DEBUG_PROTOCOL, "\n%sProtocol_vMain(REG_REQ)", acDebugIndent);
			Node_vDebugIndent(DEBUG_PROTOCOL);

			/* Debug */
			DBG_vPrintf(DEBUG_PROTOCOL, "\n%sNode_bUp() = %d", acDebugIndent, Node_bUp());
			/* Network up ? */
			if (TRUE == Node_bUp())
			{
				/* Debug */
				DBG_vPrintf(DEBUG_PROTOCOL, "\n%sNode_u32StackState() = %d", acDebugIndent, Node_u32StackState());
				/* Stack is running ? */
				if (NODE_STACK_STATE_RUNNING == Node_u32StackState())
				{
					/* Send register request to coordinator */
					Protocol_vSendCmdRegReq(&s6lpSockAddrCoord);
					/* Clear flag */
					U32_CLR_BITS(&u32CmdSend, PROTOCOL_CMD_SEND_REG_REQ);
					/* Note time of transmission */
					u32CmdRegReqSecond = Node_u32TimerSeconds();
					/* End device build ? */
					#ifdef MK_BLD_NODE_TYPE_END_DEVICE
					{
						/* Pretend a stay awake request came in - will cause us to resume after a few hundren ms to get the response */
						vJIP_StayAwakeRequest();
						/* Debug */
						DBG_vPrintf(DEBUG_PROTOCOL, "\n%svJIP_StayAwakeRequest()", acDebugIndent);
					}
					#endif
				}
				/* End device build ? */
				#ifdef MK_BLD_NODE_TYPE_END_DEVICE
				/* Stack is idle ? */
				else if (NODE_STACK_STATE_NONE == Node_u32StackState())
				{
					/* Resume running the stack */
					Node_v6lpResume();
					/* Debug */
					DBG_vPrintf(DEBUG_PROTOCOL, "\n%sNode_v6lpResume()", acDebugIndent);
				}
				#endif
			}
			/* Stack is down ? */
			else
			{
				/* Clear flag */
				U32_CLR_BITS(&u32CmdSend, PROTOCOL_CMD_SEND_REG_REQ);
			}

			/* Debug */
			Node_vDebugOutdent(DEBUG_PROTOCOL);
		}
	}
	#endif
}

/****************************************************************************
 *
 * NAME: Protocol_vSecond
 *
 * DESCRIPTION:
 * Timer function
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void Protocol_vSecond(uint32 u32TimerSeconds)
{
	/* Non-coordinator ? */
	#ifndef MK_BLD_NODE_TYPE_COORDINATOR
	{
		/* Is it time to transmit our register request ? */
		if (u32TimerSeconds - u32CmdRegReqSecond >= PROTOCOL_CMD_REG_REQ_SECONDS)
		{
			/* Debug */
			DBG_vPrintf(DEBUG_PROTOCOL, "\n%sProtocol_vSecond(REG_REQ)", acDebugIndent);
			/* Set flag to send register request command */
			U32_SET_BITS(&u32CmdSend, PROTOCOL_CMD_SEND_REG_REQ);
		}
	}
	#endif
}

/****************************************************************************/
/***        Internal Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: Protocol_vOpenSocket
 *
 * DESCRIPTION:
 * Open socket
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void Protocol_vOpenSocket(void)
{
	/* Not yet opened socket ? */
	if (FALSE == bProtocolSocket)
	{
		/* Debug */
		DBG_vPrintf(DEBUG_PROTOCOL, "\n%sProtocol_vOpenSocket()", acDebugIndent);
		Node_vDebugIndent(DEBUG_PROTOCOL);

		/* Application port defined ? */
		#if PROTOCOL_PORT
		{
			/* Register socket */
			iProtocolSocket = i6LP_Socket(E_6LP_PF_INET6,
										  E_6LP_SOCK_DGRAM,
										  E_6LP_PROTOCOL_ONLY_ONE);
			/* Debug */
			DBG_vPrintf(DEBUG_PROTOCOL, "\n%si6LP_Socket() = %d", acDebugIndent, iProtocolSocket);

			/* Failed ? */
			if (iProtocolSocket == -1)
			{
				uint32 u32ErrNo;
				/* Get error no */
				u32ErrNo = u32_6LP_GetErrNo();
				/* Debug */
				DBG_vPrintf(DEBUG_PROTOCOL, " {0x%x}", u32ErrNo);
			}
			/* Registered socket ? */
			else
			{
				int			   iBindResult;
				int 		   iSocketAddGroupAddr;

				/* Socket has been opened */
				bProtocolSocket = TRUE;

				/* Get own address */
				(void) i6LP_GetOwnDeviceAddress(&s6lpSockAddrLocal, FALSE);
				/* Set socket port in address */
				s6lpSockAddrLocal.sin6_port = PROTOCOL_PORT;

				/* Bind socket to address (including application port) */
				iBindResult = i6LP_Bind(iProtocolSocket, &s6lpSockAddrLocal, sizeof(ts6LP_SockAddr));
				/* Debug */
				DBG_vPrintf(DEBUG_PROTOCOL, "\n%si6LP_Bind() = %d", acDebugIndent, iBindResult);
				/* Failed ? */
				if (iBindResult != 0)
				{
					uint32 u32ErrNo;
					/* Get error no */
					u32ErrNo = u32_6LP_GetErrNo();
					/* Debug */
					DBG_vPrintf(DEBUG_PROTOCOL, " {0x%x}", u32ErrNo);
				}

				/* Bind the group address to the socket */
				iSocketAddGroupAddr = i6LP_SocketAddGroupAddr(iProtocolSocket, &s6lpSockAddrNetwork.sin6_addr);
				/* Debug */
				DBG_vPrintf(DEBUG_PROTOCOL, "\n%si6LP_SocketAddGroupAddr() = %d", acDebugIndent, iSocketAddGroupAddr);
				/* Failed ? */
				if (iSocketAddGroupAddr == -1)
				{
					uint32 u32ErrNo;
					/* Get error no */
					u32ErrNo = u32_6LP_GetErrNo();
					/* Debug */
					DBG_vPrintf(DEBUG_PROTOCOL, " {0x%x}", u32ErrNo);
				}
			}
		}
		#endif

		/* Debug */
		Node_vDebugOutdent(DEBUG_PROTOCOL);
	}
}

/****************************************************************************
 *
 * NAME: Protocol_vSendTo
 *
 * DESCRIPTION:
 * Generic transmit command
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void Protocol_vSendTo(ts6LP_SockAddr *ps6lpSockAddr, uint8 *pu8Data, uint16 u16DataLen)
{
	int iGetDataBufferResult;
	int iSendToResult;
	uint8 *pu8Buffer;

	/* Debug */
	DBG_vPrintf(DEBUG_PROTOCOL, "\n%sProtocol_vSendTo(0x%x)", acDebugIndent, ps6lpSockAddr->sin6_addr.s6_addr32[3]);
	Node_vDebugIndent(DEBUG_PROTOCOL);

	/* Debug */
	DBG_vPrintf(DEBUG_PROTOCOL, "\n%sNode_bUp() = %d", acDebugIndent, Node_bUp());
	/* Network up ? */
	if (TRUE == Node_bUp())
	{
		/* Debug */
		DBG_vPrintf(DEBUG_PROTOCOL, "\n%sNode_u32StackState() = %d", acDebugIndent, Node_u32StackState());
		/* Stack is running ? */
		if (NODE_STACK_STATE_RUNNING == Node_u32StackState())
		{
			/* Try to get a data buffer */
			iGetDataBufferResult = i6LP_GetDataBuffer(&pu8Buffer);
			/* Debug */
			DBG_vPrintf(DEBUG_PROTOCOL, "\n%si6LP_GetDataBuffer() = %d", acDebugIndent, iGetDataBufferResult);
			/* Error ? */
			if (iGetDataBufferResult == -1)
			{
				uint32 u32ErrNo;
				/* Get error no */
				u32ErrNo = u32_6LP_GetErrNo();
				/* Debug */
				DBG_vPrintf(DEBUG_PROTOCOL, " {0x%x}", u32ErrNo);
			}
			/* Got data buffer ? */
			else
			{
				/* Copy transmission data into buffer */
				memcpy(pu8Buffer, pu8Data, u16DataLen);
				/* Transmit packet */
				iSendToResult = i6LP_SendTo(iProtocolSocket, pu8Buffer, u16DataLen, 0, ps6lpSockAddr, sizeof(ts6LP_SockAddr));
				/* Debug */
				DBG_vPrintf(DEBUG_PROTOCOL, "\n%si6LP_SendTo(0x%x) = %d", acDebugIndent, ps6lpSockAddr->sin6_addr.s6_addr32[3], iSendToResult);
				/* Error ? */
				if (iSendToResult == -1)
				{
					uint32 u32ErrNo;
					/* Get error no */
					u32ErrNo = u32_6LP_GetErrNo();
					/* Debug */
					DBG_vPrintf(DEBUG_PROTOCOL, " {0x%x}", u32ErrNo);
				}
			}
		}
	}

	/* Debug */
	Node_vDebugOutdent(DEBUG_PROTOCOL);
}

/****************************************************************************/
/***        Internal Functions (Coordinator only)                         ***/
/****************************************************************************/
#ifdef MK_BLD_NODE_TYPE_COORDINATOR

/****************************************************************************
 *
 * NAME: Protocol_vSendCmdRegRsp
 *
 * DESCRIPTION:
 * Transmit register request command
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void Protocol_vSendCmdRegRsp(ts6LP_SockAddr *ps6lpSockAddr)
{
	tsProtocolCmdRegRsp sProtocolCmdRegRsp;

	/* Debug */
	DBG_vPrintf(DEBUG_PROTOCOL, "\n%sProtocol_vSendCmdRegRsp(0x%x)", acDebugIndent, ps6lpSockAddr->sin6_addr.s6_addr32[3]);
	Node_vDebugIndent(DEBUG_PROTOCOL);

	/* Build register request command */
	sProtocolCmdRegRsp.cCmd = PROTOCOL_CMD_REG_RSP;
	/* Transmit command to coordinator */
	Protocol_vSendTo(ps6lpSockAddr, (uint8 *) &sProtocolCmdRegRsp, sizeof(sProtocolCmdRegRsp));

	/* Debug */
	Node_vDebugOutdent(DEBUG_PROTOCOL);
}

/****************************************************************************
 *
 * NAME: Protocol_vRecvCmdRegReq
 *
 * DESCRIPTION:
 * Transmit register response command
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void Protocol_vRecvCmdRegReq(ts6LP_SockAddr *ps6lpSockAddr, tsProtocolCmdRegReq *psProtocolCmdRegReq)
{
	/* Debug */
	DBG_vPrintf(DEBUG_PROTOCOL, "\n%sProtocol_vRecvCmdRegReq(0x%x)", acDebugIndent, ps6lpSockAddr->sin6_addr.s6_addr32[3]);
	Node_vDebugIndent(DEBUG_PROTOCOL);

	/* Transmit command */
	Protocol_vSendCmdRegRsp(ps6lpSockAddr);

	/* Debug */
	Node_vDebugOutdent(DEBUG_PROTOCOL);
}

#endif
/* Coordinator only functions */

/****************************************************************************/
/***        Internal Functions (Non-coordinator only)                     ***/
/****************************************************************************/
#ifndef MK_BLD_NODE_TYPE_COORDINATOR

/****************************************************************************
 *
 * NAME: Protocol_vSendCmdRegReq
 *
 * DESCRIPTION:
 * Transmit register request command
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void Protocol_vSendCmdRegReq(ts6LP_SockAddr *ps6lpSockAddr)
{
	tsProtocolCmdRegReq sProtocolCmdRegReq;

	/* Debug */
	DBG_vPrintf(DEBUG_PROTOCOL, "\n%sProtocol_vSendCmdRegReq(0x%x)", acDebugIndent, ps6lpSockAddr->sin6_addr.s6_addr32[3]);
	Node_vDebugIndent(DEBUG_PROTOCOL);

	/* Build register request command */
	sProtocolCmdRegReq.cCmd = PROTOCOL_CMD_REG_REQ;
	/* Transmit command to coordinator */
	Protocol_vSendTo(&s6lpSockAddrCoord, (uint8 *) &sProtocolCmdRegReq, sizeof(sProtocolCmdRegReq));

	/* Debug */
	Node_vDebugOutdent(DEBUG_PROTOCOL);
}

/****************************************************************************
 *
 * NAME: Protocol_vRecvCmdRegRsp
 *
 * DESCRIPTION:
 * Receive register response command
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void Protocol_vRecvCmdRegRsp(ts6LP_SockAddr *ps6lpSockAddr, tsProtocolCmdRegRsp *psProtocolCmdRegRsp)
{
	/* Debug */
	DBG_vPrintf(DEBUG_PROTOCOL, "\n%sProtocol_vRecvCmdRegRsp(0x%x)", acDebugIndent, ps6lpSockAddr->sin6_addr.s6_addr32[3]);
	Node_vDebugIndent(DEBUG_PROTOCOL);

	/* Note the coordinators address for furutre use */
	memcpy(&s6lpSockAddrCoord, ps6lpSockAddr, sizeof(ts6LP_SockAddr));

	/* Debug */
	Node_vDebugOutdent(DEBUG_PROTOCOL);
}

#endif
/* Non-coordinator only functions */

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
