
 /***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>
#include "AppHardwareApi.h"
#include "DriverREDsensor.h"
#include "Uart.h"
#include <stdio.h>
#include <math.h>
#include "MibAdcStatus.h"
#include <stdlib.h>
#include "MibCOstatus.h"
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

PUBLIC int16 DriverREDsensor_i16Analogue(uint8 u8Adc, uint16 u16AdcRead)
{
	if (u8Adc == E_AHI_ADC_SRC_ADC_2)
	{
		return ((u16AdcRead * 2400)/ADC_FULL_SCALE);
	}
	else
	{
		return(ADC_FULL_SCALE);
	}
}

/* get the calibrated ro based upon read resistance, and a know ppm */
PUBLIC float DriverREDsensor_getro(float resvalue)
 {
	return (float)(resvalue / (float)RED_SENSOR_MAX_RSRO  );
 }

PUBLIC float fastlog2 (float x)
{
  union { float f; uint32 i; } vx = { x };
  union { uint32 i; float f; } mx = { (vx.i & 0x007FFFFF) | (0x7e << 23) };
  float y = vx.i;
  y *= 1.0 / (1 << 23);

  return
    y - 124.22544637f - 1.498030302f * mx.f - 1.72587999f / (0.3520887068f + mx.f);
}

PUBLIC float fastpow2 (float p)
{
  union { float f; uint32 i; } vp = { p };
  int sign = (vp.i >> 31);
  int w = p;
  float z = p - w + sign;
  union { uint32 i; float f; } v = { (1 << 23) * (p + 121.2740838f + 27.7280233f / (4.84252568f - z) - 1.49012907f * z) };
  return v.f;
}

PUBLIC float fastpow (float x,float p)

{
  return fastpow2 (p * fastlog2 (x));
}


PUBLIC float fastexp (float p)
{
  return fastpow2 (1.442695040f * p);
}

PUBLIC float DriverREDsensor_getppm(float resvalue,float ro)
{
  float f = resvalue/(float)ro;
  float f2 = f*f;
  float f3 = f*f*f;
  float f4 = f*f*f*f;
  float f5 = f*f*f*f;
  if ((f<= INT2_CO) && (f>= INT1_CO))
  {
	  float COppm= (float)(VAL10_CO*f4+ VAL11_CO*f3+VAL12_CO*f2+VAL13_CO*f+VAL14_CO);
	  return COppm;
  }
  else if ((f<= INT3_CO) && (f>= INT2_CO))
  {
	  float COppm=(float)(VAL20_CO*f3+VAL21_CO*f2+VAL22_CO*f+VAL23_CO);
	  return COppm;
  }
  else if ((f<= INT4_CO) && (f>= INT3_CO))
  {
	  float COppm=(float)(VAL30_CO*f4+VAL31_CO*f3+VAL32_CO*f2+VAL33_CO*f+VAL34_CO);
	  return COppm;
  }
  else if ((f<= INT5_CO) && (f>= INT4_CO))
  {
	  float COppm=(float)(VAL40_CO*f2+VAL41_CO*f+VAL42_CO);
	  return COppm;
  }
  else return 0;

}



