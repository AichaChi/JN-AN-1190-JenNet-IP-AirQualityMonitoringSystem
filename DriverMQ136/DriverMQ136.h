/*
 * DriverOXsensor.h
 *
 *  Created on: 30 août 2016
 *      Author: lenovo-pc
 */

#ifndef DRIVERMQ136SENSOR_H_
#define DRIVERMQ136SENSOR_H_

#define MQ136_SENSOR_DEFAULT_RO     25510

#define MQ136_SENSOR_MAX_RSRO       0.6
#define MQ136_SENSOR_MIN_RSRO       0.15
#define VAL_H2S_TARGET              1

#define MQ136_SENSOR_MAX_PPM        200
#define MQ136_SENSOR_MIN_PPM	    1

#define VAL_SENSOR_ALIM             5000
#define VAL_MQ136_SENSOR_RGND       3300

#define INT1_H2S                    0.15
#define INT2_H2S                    0.18
#define INT3_H2S                    0.2
#define INT4_H2S                    0.325
#define INT5_H2S                    0.6

#define VAL40_H2S                   1684.208319
#define VAL41_H2S                  -3886.656535
#define VAL42_H2S                   3347.662861
#define VAL43_H2S                  -1286.26406
#define VAL44_H2S                   188.8411783


#define VAL30_H2S                  -171314.6091
#define VAL31_H2S                   149474.2349
#define VAL32_H2S                  -42868.40608
#define VAL33_H2S                   3722.304615
#define VAL34_H2S                   108.354701

#define VAL20_H2S                   1428.863033
#define VAL21_H2S                  -1542.967953
#define VAL22_H2S                   311.4390692

#define VAL10_H2S                   17142874.28
#define VAL11_H2S                  -8657151.611
#define VAL12_H2S                   1448858.626
#define VAL13_H2S                  -80200.08345


/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
PUBLIC float DriverMQ136_getro(float resvalue);
PUBLIC float DriverMQ136_getppm(float resvalue,float ro);
PUBLIC int16 DriverMQ136_i16Analogue(uint8 u8Adc, uint16 u16AdcRead);
PUBLIC float fastlog2 (float x);
PUBLIC float fastpow2 (float p);
PUBLIC float fastpow (float x,float p);


#endif /* DRIVEROXSENSOR_H_ */
