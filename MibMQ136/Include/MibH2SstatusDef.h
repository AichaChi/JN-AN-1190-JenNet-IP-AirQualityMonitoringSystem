/*
 * MibH2SStatusDef.h
 *
 *  Created on: 14 avr. 2016
 *      Author: lenovo-pc
 */

#undef MIB_HEADER
#define MIB_HEADER "MibH2SstatusDef.h"
/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
/* Stack includes */
#include <jip_define_mib.h>
/* Application common includes */
#include "MibH2Sstatus.h"
#include "MibSensors.h"
/****************************************************************************/
/***        MIB definition                                                ***/
/****************************************************************************/
/* COStatus MIB */
START_DEFINE_MIB(MIB_ID_H2S_STATUS, H2SstatusDef)
/*         ID 					   	     		    Type    Name          Disp  Flags     Access    Cache Security */
DEFINE_VAR(VAR_IX_H2S_STATUS_PPM_CURRENT,           UINT8, H2Sppm,         NULL, 0,    (READ | TRAP), NONE, NONE)
DEFINE_VAR(VAR_IX_H2S_STATUS_TARGET_STATUS,         UINT8, TargetStatus,   NULL, 0,    (READ | TRAP), NONE, NONE)
DEFINE_VAR(VAR_IX_H2S_STATUS_PPM_MIN,               UINT8, H2SMin,         NULL, 0,    (READ),        NONE, NONE)
DEFINE_VAR(VAR_IX_H2S_STATUS_PPM_MAX,               UINT8, H2SMax,         NULL, 0,    (READ),        NONE, NONE)
DEFINE_VAR(VAR_IX_H2S_STATUS_RO,                    UINT8, Ro,             NULL, 0,    (READ),        NONE, NONE)
DEFINE_VAR(VAR_IX_H2S_CONTROL_PPM_TARGET,           FLOAT, H2STarget,      NULL, 0,    (READ | WRITE | TRAP)    , NONE, NONE)
DEFINE_VAR(VAR_IX_H2S_STATUS_V,                     FLOAT, i16Volts,       NULL, 0,    (READ | WRITE | TRAP)    , NONE, NONE)

END_DEFINE_MIB(H2SstatusDef)

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

