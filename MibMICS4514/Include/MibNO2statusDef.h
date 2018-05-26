/*
 * MibNO2StatusDef.h
 *
 *  Created on: 14 avr. 2016
 *      Author: lenovo-pc
 */

#undef MIB_HEADER
#define MIB_HEADER "MibNO2statusDef.h"
/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
/* Stack includes */
#include <jip_define_mib.h>
/* Application common includes */
#include "MibNO2status.h"
#include "MibSensors.h"
/****************************************************************************/
/***        MIB definition                                                ***/
/****************************************************************************/
/* NO2Status MIB */
START_DEFINE_MIB(MIB_ID_NO2_STATUS, NO2statusDef)
/*         ID 					   	     		    Type    Name          Disp  Flags      Access               Cache Security */
DEFINE_VAR(VAR_IX_NO2_STATUS_PPB_CURRENT,           UINT8, NO2ppb,         NULL, 0,    (READ | TRAP),             NONE, NONE)
DEFINE_VAR(VAR_IX_NO2_STATUS_TARGET_STATUS,         UINT8, TargetStatus,   NULL, 0,    (READ | TRAP),             NONE, NONE)
DEFINE_VAR(VAR_IX_NO2_STATUS_PPB_MIN,               UINT8, NO2Min,         NULL, 0,    (READ),                    NONE, NONE)
DEFINE_VAR(VAR_IX_NO2_STATUS_PPB_MAX,               UINT8, NO2Max,         NULL, 0,    (READ),                    NONE, NONE)
DEFINE_VAR(VAR_IX_NO2_STATUS_RO,                    UINT8, Ro,             NULL, 0,    (READ),                    NONE, NONE)
DEFINE_VAR(VAR_IX_NO2_CONTROL_PPB_TARGET,           FLOAT, NO2target,      NULL, 0,    (READ | WRITE | TRAP)    , NONE, NONE)
DEFINE_VAR(VAR_IX_NO2_STATUS_V,                     FLOAT, i16Volts,       NULL, 0,    (READ | WRITE | TRAP)    , NONE, NONE)

END_DEFINE_MIB(NO2statusDef)

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

