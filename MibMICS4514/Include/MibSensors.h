/*
 * MibSensors.h
 *
 *  Created on: 5 août 2016
 *      Author: lenovo-pc
 */

#ifndef MIBSENSORS_H_
#define MIBSENSORS_H_

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/
/* SDK includes */
#include <jendefs.h>
/* JenOS includes */
#include <os.h>
#include <pdm.h>
#include <JIP.h>
#include <6LP.h>
/* Application common includes */
#include "Table.h"
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
/* CO MIBs *****************************************/

/* COStatus MIB */
#define MIB_ID_CO_STATUS					0xFFFFFE38
#define VAR_IX_CO_STATUS_PPM_CURRENT          		 0
#define VAR_IX_CO_STATUS_TARGET_STATUS        		 1
#define VAR_IX_CO_STATUS_PPM_MIN					 2
#define VAR_IX_CO_STATUS_PPM_MAX					 3
#define VAR_IX_CO_STATUS_RO                          4
#define VAR_IX_CO_STATUS_V_READ                      5
#define VAR_IX_CO_CONTROL_MODE  					 6
#define VAR_IX_CO_CONTROL_PPM_TARGET				 7
#define VAR_IX_CO_STATUS_V				             8
#define VAR_MASK_CO_STATUS			        0x000000ff
#define VAR_COUNT_CO_STATUS			            	 9

#define MIB_ID_NO2_STATUS					0xFFFFFE39
#define VAR_IX_NO2_STATUS_PPB_CURRENT          		 1
#define VAR_IX_NO2_STATUS_TARGET_STATUS        		 0
#define VAR_IX_NO2_STATUS_PPB_MIN					 2
#define VAR_IX_NO2_STATUS_PPB_MAX					 3
#define VAR_IX_NO2_STATUS_RO                         4
#define VAR_IX_NO2_STATUS_V_READ                     5
#define VAR_IX_NO2_CONTROL_MODE  					 6
#define VAR_IX_NO2_CONTROL_PPB_TARGET				 7
#define VAR_IX_NO2_STATUS_V				             8
#define VAR_MASK_NO2_STATUS			        0x0000003f
#define VAR_COUNT_NO2_STATUS			             9

#define MIB_ID_O3_STATUS					0xFFFFFE37
#define VAR_IX_O3_STATUS_PPB_CURRENT          		 2
#define VAR_IX_O3_STATUS_TARGET_STATUS        		 0
#define VAR_IX_O3_STATUS_PPB_MIN					 1
#define VAR_IX_O3_STATUS_PPB_MAX					 3
#define VAR_IX_O3_STATUS_RO                          4
#define VAR_IX_O3_STATUS_V_READ                      5
#define VAR_IX_O3_CONTROL_MODE  					 6
#define VAR_IX_O3_CONTROL_PPB_TARGET				 7
#define VAR_IX_O3_STATUS_V				             8
#define VAR_MASK_O3_STATUS			        0x0000003f
#define VAR_COUNT_O3_STATUS			                 9

#define MIB_ID_H2S_STATUS					0xFFFFFE36
#define VAR_IX_H2S_STATUS_PPM_CURRENT          		 3
#define VAR_IX_H2S_STATUS_TARGET_STATUS        		 1
#define VAR_IX_H2S_STATUS_PPM_MIN					 2
#define VAR_IX_H2S_STATUS_PPM_MAX					 3
#define VAR_IX_H2S_STATUS_RO                         4
#define VAR_IX_H2S_STATUS_V_READ                     5
#define VAR_IX_H2S_CONTROL_MODE  					 6
#define VAR_IX_H2S_CONTROL_PPM_TARGET				 7
#define VAR_IX_H2S_STATUS_V				             8
#define VAR_MASK_H2S_STATUS			        0x0000003f
#define VAR_COUNT_H2S_STATUS			             9

#define VAR_VAL_PPM_STATUS_TARGET_STATUS_OK       0
#define VAR_VAL_PPM_STATUS_TARGET_STATUS_CAUTION  1
#define VAR_VAL_PPM_STATUS_TARGET_STATUS_WARNING  2
#define VAR_VAL_PPM_STATUS_TARGET_STATUS_THREAT0  3
#define VAR_VAL_PPM_STATUS_TARGET_STATUS_THREAT1  4
#define VAR_VAL_PPM_STATUS_TARGET_STATUS_THREAT2  5
#define VAR_VAL_PPM_STATUS_TARGET_STATUS_THREAT3  6

#endif /* MIBSENSORS_H */
