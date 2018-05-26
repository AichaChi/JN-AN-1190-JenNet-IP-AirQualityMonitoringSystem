/*
 * MibCOStatusDef.h
 *
 *  Created on: 14 avr. 2016
 *      Author: lenovo-pc
 */

#undef MIB_HEADER
#define MIB_HEADER "MibCOstatusDef.h"
/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
/* Stack includes */
#include <jip_define_mib.h>
/* Application common includes */
#include "MibCOstatus.h"
#include "MibSensors.h"
/****************************************************************************/
/***        MIB definition                                                ***/
/****************************************************************************/
/* COStatus MIB */
START_DEFINE_MIB(MIB_ID_CO_STATUS, COstatusDef)
/*         ID 					   	     		    Type    Name          Disp  Flags Access         Cache Security */
DEFINE_VAR(VAR_IX_CO_STATUS_PPM_CURRENT,           UINT8, COppm,      NULL, 0,    (READ | TRAP), NONE, NONE)
DEFINE_VAR(VAR_IX_CO_STATUS_TARGET_STATUS,         UINT8, TargetStatus,   NULL, 0,    (READ | TRAP), NONE, NONE)
DEFINE_VAR(VAR_IX_CO_STATUS_PPM_MIN,               UINT8, COMin,          NULL, 0,    (READ),        NONE, NONE)
DEFINE_VAR(VAR_IX_CO_STATUS_PPM_MAX,               UINT8, COMax,          NULL, 0,    (READ),        NONE, NONE)
DEFINE_VAR(VAR_IX_CO_STATUS_RO,                    UINT8, Ro,             NULL, 0,    (READ),        NONE, NONE)
DEFINE_VAR(VAR_IX_CO_CONTROL_PPM_TARGET,           FLOAT,  COTarget,       NULL, 0,    (READ)    , NONE, NONE)
DEFINE_VAR(VAR_IX_CO_STATUS_V,                     FLOAT, i16Volts,       NULL, 0,    (READ)    , NONE, NONE)

END_DEFINE_MIB(COstatusDef)

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

