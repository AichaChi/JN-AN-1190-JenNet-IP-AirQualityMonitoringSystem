/*
 * DriverOXsensor.h
 *
 *  Created on: 30 août 2016
 *      Author: lenovo-pc
 */

#ifndef DRIVEROXSENSOR_H_
#define DRIVEROXSENSOR_H_

#define OX_SENSOR_DEFAULT_RO     1060   //1378.33333333

#define OX_SENSOR_MAX_RSRO       4000
#define OX_SENSOR_MIN_RSRO       3

#define OX_SENSOR_MAX_NO2        900
#define OX_SENSOR_MIN_NO2        10

#define VAL_NO2_TARGET           53

#define OX_RL_VALUE              812
#define VAL_SENSOR_ALIM          5000
#define VAL_OX_SENSOR_RGND       2200

#define INT1_NO2                 3
#define INT2_NO2                 7
#define INT3_NO2                 30
#define INT4_NO2                 400
#define INT5_NO2                 4000

#define VAL10_NO2                6.913021619 //*10-2
#define VAL11_NO2               -1.522750126
#define VAL12_NO2                16.01621418
#define VAL13_NO2               -26.21040724

#define VAL20_NO2                1.557106558 //*10-4
#define VAL21_NO2               -1.451477427
#define VAL22_NO2                0.5118850507
#define VAL23_NO2               -8.465126374
#define VAL24_NO2                67.58556656
#define VAL25_NO2               -166.6514012

#define VAL30_NO2               -1.205919378 //*10-8
#define VAL31_NO2                8.076735268 //*10-6
#define VAL32_NO2               -1.551450303 //*10-3
#define VAL33_NO2                0.1037683924
#define VAL34_NO2               -0.8666235438
#define VAL35_NO2                58.24722707

#define VAL40_NO2               -2.095599873 //*10-11
#define VAL41_NO2                1.909876543 //*10-7
#define VAL42_NO2               -6.015479582 //*10-4
#define VAL43_NO2                0.911592276
#define VAL44_NO2                19.92402659










/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
PUBLIC float DriverOXsensor_getro(float resvalue);
PUBLIC float DriverOXsensor_getppb(float resvalue,float ro);
PUBLIC int16 DriverOXsensor_i16Analogue(uint8 u8Adc, uint16 u16AdcRead);
PUBLIC float fastlog2 (float x);
PUBLIC float fastpow2 (float p);
PUBLIC float fastpow (float x,float p);


#endif /* DRIVEROXSENSOR_H_ */
