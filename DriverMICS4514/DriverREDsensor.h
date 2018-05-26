
#ifndef REDSENSOR_H_
#define REDSENSOR_H_

#define RED_SENSOR_DEFAULT_RO     297221

#define RED_SENSOR_MAX_RSRO       0.7
#define RED_SENSOR_MIN_RSRO       0.085

#define RED_SENSOR_MAX_CO         1000
#define RED_SENSOR_MIN_CO         1

#define RED_RL_VALUE              812
#define VAL_SENSOR_ALIM           5000
#define VAL_RED_SENSOR_RGND       8990
#define VAL_CO_TARGET             1

#define INT1_CO                   0.027
#define INT2_CO                   0.085
#define INT3_CO                   0.35
#define INT4_CO                   0.57
#define INT5_CO                   0.7

#define VAL10_CO                 -378800.3788
#define VAL11_CO                  268852.2688
#define VAL12_CO                 -59191.90919
#define VAL13_CO                  2400.244499
#define VAL14_CO                  478.3054784

#define VAL20_CO                 -154761.8985
#define VAL21_CO                  148214.28
#define VAL22_CO                 -47402.37923
#define VAL23_CO                  5129.999827

#define VAL30_CO                  115314.9946
#define VAL31_CO                 -214077.6352
#define VAL32_CO                  148074.1214
#define VAL33_CO                 -45434.25497
#define VAL34_CO                  5271.257255

#define VAL40_CO                  399.2345651
#define VAL41_CO                 -575.9302905
#define VAL42_CO                  208.6949146




/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
PUBLIC float DriverREDsensor_getro(float resvalue);
PUBLIC float DriverREDsensor_getppm(float resvalue,float COppm);
PUBLIC int16 DriverREDsensor_i16Analogue(uint8 u8Adc, uint16 u16AdcRead);
PUBLIC float fastlog2 (float x);
PUBLIC float fastpow2 (float p);
PUBLIC float fastpow (float x,float p);

#endif /* REDSENSOR_H_ */
