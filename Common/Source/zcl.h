/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         JenOS Overlay Removal Defines
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
#ifndef  ZCL_H_INCLUDED
#define  ZCL_H_INCLUDED

#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
/* SDK includes */
#include <jendefs.h>

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
// ZCL return status codes returned to the user
typedef enum PACK
{
   // General
   E_ZCL_SUCCESS = 0x0,
   E_ZCL_FAIL,                  // 01
   E_ZCL_ERR_PARAMETER_NULL,    // 02
   E_ZCL_ERR_PARAMETER_RANGE,   // 03
   E_ZCL_ERR_HEAP_FAIL,         // 04
#if 0
   // Specific ZCL status codes
   E_ZCL_ERR_EP_RANGE,          // 05
   E_ZCL_ERR_EP_UNKNOWN,        // 06
   E_ZCL_ERR_SECURITY_RANGE,    // 07
   E_ZCL_ERR_CLUSTER_0,         // 08
   E_ZCL_ERR_CLUSTER_NULL,      // 09
   E_ZCL_ERR_CLUSTER_NOT_FOUND, // 10
   E_ZCL_ERR_CLUSTER_ID_RANGE,  // 11
   E_ZCL_ERR_ATTRIBUTES_NULL,   // 12
   E_ZCL_ERR_ATTRIBUTES_0,      // 13
   E_ZCL_ERR_ATTRIBUTE_WO,      // 14
   E_ZCL_ERR_ATTRIBUTE_RO,      // 15
   E_ZCL_ERR_ATTRIBUTES_ACCESS, // 16
   E_ZCL_ERR_ATTRIBUTE_TYPE_UNSUPPORTED, // 17
   E_ZCL_ERR_ATTRIBUTE_NOT_FOUND,   // 18
   E_ZCL_ERR_CALLBACK_NULL,         // 19
   E_ZCL_ERR_ZBUFFER_FAIL,          // 20
   E_ZCL_ERR_ZTRANSMIT_FAIL,        // 21
   E_ZCL_ERR_CLIENT_SERVER_STATUS,  // 22
   E_ZCL_ERR_TIMER_RESOURCE,        // 23
   E_ZCL_ERR_ATTRIBUTE_IS_CLIENT,   // 24
   E_ZCL_ERR_ATTRIBUTE_IS_SERVER,   // 25
   E_ZCL_ERR_ATTRIBUTE_RANGE,       // 26
   E_ZCL_ERR_ATTRIBUTE_MISMATCH,    // 27
   E_ZCL_ERR_KEY_ESTABLISHMENT_MORE_THAN_ONE_CLUSTER, //28
   E_ZCL_ERR_INSUFFICIENT_SPACE,    // 29
   E_ZCL_ERR_NO_REPORTABLE_CHANGE,  // 30
   E_ZCL_ERR_NO_REPORT_ENTRIES,     // 31
   E_ZCL_ERR_ATTRIBUTE_NOT_REPORTABLE, //32
   E_ZCL_ERR_ATTRIBUTE_ID_ORDER,    // 33
   E_ZCL_ERR_MALFORMED_MESSAGE,     // 34
   E_ZCL_ERR_MANUFACTURER_SPECIFIC, // 35
   E_ZCL_ERR_PROFILE_ID,            // 36
   E_ZCL_ERR_INVALID_VALUE,         // 37
   E_ZCL_ERR_CERT_NOT_FOUND,        // 38
   E_ZCL_ERR_CUSTOM_DATA_NULL,      // 39
   E_ZCL_ERR_TIME_NOT_SYNCHRONISED, // 40
   E_ZCL_ERR_SIGNATURE_VERIFY_FAILED, //41
   E_ZCL_ERR_ZRECEIVE_FAIL,          // 42
   E_ZCL_ERR_KEY_ESTABLISHMENT_END_POINT_NOT_FOUND, // 43
   E_ZCL_ERR_KEY_ESTABLISHMENT_CLUSTER_ENTRY_NOT_FOUND, // 44
   E_ZCL_ERR_KEY_ESTABLISHMENT_CALLBACK_ERROR, // 45
   E_ZCL_ERR_SECURITY_INSUFFICIENT_FOR_CLUSTER, // 46
   E_ZCL_ERR_CUSTOM_COMMAND_HANDLER_NULL_OR_RETURNED_ERROR, // 47
   E_ZCL_ERR_INVALID_IMAGE_SIZE,         // 48
   E_ZCL_ERR_INVALID_IMAGE_VERSION,         // 49
   E_ZCL_READ_ATTR_REQ_NOT_FINISHED,         // 50
   E_ZCL_DENY_ATTRIBUTE_ACCESS,             // 51
   E_ZCL_ERR_SECURITY_FAIL,                 // 52
#endif
   E_ZCL_ERR_ENUM_END
} teZCL_Status;

#endif  /* ZCL_H_INCLUDED */
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
