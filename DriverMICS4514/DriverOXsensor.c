
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
#include "MibNO2status.h"
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

PUBLIC int16 DriverOXsensor_i16Analogue(uint8 u8Adc, uint16 u16AdcRead)
{
	if (u8Adc == E_AHI_ADC_SRC_ADC_1)
	{
		return ((u16AdcRead * 2400)/ADC_FULL_SCALE);
	}
	else
	{
		return(ADC_FULL_SCALE);
	}
}

 /* get the calibrated ro based upon read resistance, and a know ppm */
 PUBLIC float DriverOXsensor_getro(float resvalue)
 {

	return (float)(resvalue / (float) OX_SENSOR_MIN_RSRO );
 }

PUBLIC float DriverOXsensor_getppb(float resvalue,float ro)
{
	  float f = resvalue/ro;
	  float f2 = f*f;
	  float f3 = f*f*f;
	  float f4 = f*f*f*f;
	  float f5 = f*f*f*f;
	  if ((f <= INT2_NO2) && (f >= INT1_NO2))
	  {
		  float NO2ppb =(float)(VAL10_NO2*0.01*f3+VAL11_NO2*f2+VAL12_NO2*f+VAL13_NO2);
		  return NO2ppb;
	  }
	  else if ((f <= INT3_NO2) && (f >= INT2_NO2))
	  {
		  float NO2ppb = (float)(VAL20_NO2*0.0001*f5+VAL21_NO2*0.01*f4+VAL22_NO2*f3+VAL23_NO2*f2+VAL24_NO2*f+VAL25_NO2);
		  return NO2ppb;
	  }
	  else if ((f <= INT4_NO2) && (f >= INT3_NO2))
	  {
		  float NO2ppb =(float)(VAL30_NO2*0.00000001*f5+VAL31_NO2*0.000001*f4+VAL32_NO2*0.001*f3+VAL33_NO2*f2+VAL34_NO2*f+VAL35_NO2);
		  return NO2ppb;
	  }
	  else if ((f <= INT5_NO2) && (f >= INT4_NO2))
	  {
		  float NO2ppb =(float)(VAL40_NO2*0.00000000001*f4+VAL41_NO2*0.0000001*f3+VAL42_NO2*0.0001*f2+VAL43_NO2*f+VAL44_NO2);
		  return NO2ppb;
	  }
	  else return 0;

}



