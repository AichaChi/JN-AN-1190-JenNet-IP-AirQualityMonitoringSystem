
 /***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>
#include "AppHardwareApi.h"
#include "DriverOXsensor.h"
#include "Uart.h"
#include <stdio.h>
#include <math.h>
#include "MibAdcStatus.h"
#include <stdlib.h>
#include "MibH2Sstatus.h"
#include"DriverMQ136.h"
#include"DriverREDsensor.h"
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define ADC_FULL_SCALE            1024

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
/****************************************************************************/
/***        External variables                                            ***/

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/

PUBLIC int16 DriverMQ136_i16Analogue(uint8 u8Adc, uint16 u16AdcRead)
{
	if (u8Adc == E_AHI_ADC_SRC_ADC_4)
	{
		return ((u16AdcRead * 2400)/ADC_FULL_SCALE);
	}
	else
	{
		return(ADC_FULL_SCALE);
	}
}

/* get the calibrated ro based upon read resistance, and a know ppm */
PUBLIC float DriverMQ136_getro(float resvalue)
 {
	return (float)(resvalue / (float)MQ136_SENSOR_MAX_RSRO  );
 }

PUBLIC float DriverMQ136_getppm(float resvalue,float ro)
{
   float f = resvalue/(float)ro;
   float f2 = f*f;
   float f3 = f*f*f;
   float f4 = f*f*f*f;
   float f5 = f*f*f*f;
   if ((f<= INT2_H2S ) && (f>= INT1_H2S ))
	{
		float H2Sppm = (float)(VAL10_H2S*f3+VAL11_H2S*f2+VAL12_H2S*f+VAL13_H2S);
		return H2Sppm;
	}
   else if ((f<= INT3_H2S) && (f>= INT2_H2S))
	{
	     float H2Sppm = (float)(VAL20_H2S*f2+VAL21_H2S*f+VAL22_H2S);
		 return H2Sppm;
	}
   else if ((f<= INT4_H2S) && (f>= INT3_H2S))
	{
		 float H2Sppm = (float)(VAL30_H2S*f4+VAL31_H2S*f3+VAL32_H2S*f2+VAL33_H2S*f+VAL34_H2S);
		 return H2Sppm;
	}
   else if ((f<= INT5_H2S) && (f>= INT4_H2S))
	 {
		float H2Sppm = (float)(VAL40_H2S*f4+VAL41_H2S*f3+VAL42_H2S*f2+VAL43_H2S*f+VAL44_H2S);
		return H2Sppm;
	 }
   else return 0;
}



