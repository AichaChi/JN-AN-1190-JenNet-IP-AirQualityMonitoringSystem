/*
 * DriverOXsensor.h
 *
 *  Created on: 30 août 2016
 *      Author: lenovo-pc
 */


#ifndef DRIVERMICS2614SENSOR_H_
#define DRIVERMICS2614SENSOR_H_

#define MICS2614_SENSOR_DEFAULT_RO     45914

#define MICS2614_SENSOR_MAX_RSRO      10
#define MICS2614_SENSOR_MIN_RSRO      0.065
#define MICS2614_SENSOR_MAX_PPB       1000
#define MICS2614_SENSOR_MIN_PPB       10
#define MICS2614_RL_VALUE             812
#define VAL_O3_TARGET                 30

#define VAL_SENSOR_ALIM               5000
#define VAL_MICS2614_SENSOR_RGND      916

#define INT1_O3                       0.065
#define INT2_O3                       0.6
#define INT3_O3                       4.1
#define INT4_O3                       6

#define VAL10_O3                      28105.54453
#define VAL11_O3                     -47894.02225
#define VAL12_O3                      29610.84289
#define VAL13_O3                     -7930.860026
#define VAL14_O3                      943.0791608
#define VAL15_O3                     -25.10181193

#define VAL20_O3                      27.47252747
#define VAL21_O3                     -230.7692308
#define VAL22_O3                      753.5714286
#define VAL23_O3                     -603.8461539

#define VAL30_O3                     -7.326309504
#define VAL31_O3                      139.7989488
#define VAL32_O3                     -686.9450272
#define VAL33_O3                      1471.39086
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
PUBLIC float DriverMICS2614_getro(float resvalue);
PUBLIC float DriverMICS2614_getppb(float resvalue,float ro);
PUBLIC int16 DriverMICS2614_i16Analogue(uint8 u8Adc, uint16 u16AdcRead);
PUBLIC float fastlog2 (float x);
PUBLIC float fastpow2 (float p);
PUBLIC float fastpow (float x,float p);


#endif /* DRIVEROXSENSOR_H_ */
