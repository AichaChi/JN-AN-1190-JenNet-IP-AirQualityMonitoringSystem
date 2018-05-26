
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
#include"DriverMICS2614.h"
#include "DriverREDsensor.h"
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

PUBLIC int16 DriverMICS2614_i16Analogue(uint8 u8Adc, uint16 u16AdcRead)
{
	if (u8Adc == E_AHI_ADC_SRC_ADC_3)
	{
		return ((u16AdcRead * 2400)/ADC_FULL_SCALE);
	}
	else
	{
		return(ADC_FULL_SCALE);
	}
}

/* get the calibrated ro based upon read resistance, and a know ppm */
PUBLIC float DriverMICS2614_getro(float resvalue)
 {
	return (float)(resvalue / (float)MICS2614_SENSOR_MIN_RSRO  );
 }

PUBLIC float DriverMICS2614_getppb(float resvalue,float ro)
{
	  float f = resvalue/(float)ro;
	  float f2 = f*f;
	  float f3 = f*f*f;
	  float f4 = f*f*f*f;
      float f5 = f*f*f*f;
	  if ((f<= INT2_O3) && (f>= INT1_O3))
	  {
		  float O3ppb= (float)(VAL10_O3*f5+VAL11_O3*f4+VAL12_O3*f3+VAL13_O3*f2+VAL14_O3*f+VAL15_O3);
		  return O3ppb;
	  }
	  else if ((f<= INT3_O3) && (f>= INT2_O3))
	  {
		  float O3ppb= (float)(VAL20_O3*f3+VAL21_O3*f2+VAL22_O3*f+VAL23_O3);
		  return O3ppb;
	  }
	  else if ((f<= INT4_O3) && (f>= INT3_O3))
	  {
		  float O3ppb=(float)(VAL30_O3*f3+VAL31_O3*f2+VAL32_O3*f+VAL33_O3);
		  return O3ppb;
	  }
	  else return 0;
}



