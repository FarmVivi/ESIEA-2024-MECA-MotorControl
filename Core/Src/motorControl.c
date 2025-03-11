/*
 * protocol.c
 *
 * Motor control libray
 *
 *      Author: F. CRISON
 *      Version: 1.0.0
 */
#include "main.h"
#include "stm32l0xx_it.h"

//#define DISPLAY_V_ADC //Print ADC voltage on serial port
//#define DISPLAY_V_MOTOR //Print motor voltage on serial port

//Return ADC converter value (0-4095) (1 LSB=3.3V/4096)
uint16_t readADC(void)
{
  uint16_t l_value;
  LL_ADC_REG_SetSequencerChannels(ADC1,LL_ADC_CHANNEL_1);
  LL_ADC_REG_StartConversion(ADC1);
  while (LL_ADC_IsActiveFlag_EOC(ADC1)==0);
  l_value=LL_ADC_REG_ReadConversionData12(ADC1);
#ifdef DISPLAY_V_ADC
  {
    int IntPartADC, RemainPartADC;
    IntPartADC=((long)adcValue*33)/40960;
    RemainPartADC=(((((long)adcValue*330))%409600)*10)/4096;
    printf("Vpot=%d.%3.3d\r\n",IntPartADC,RemainPartADC );
  }
#endif //DISPLAY_V_ADC
  return l_value;
}


//Set motor voltage ratio (-MAX_MOTOR to +MAX_MOTOR)
void setMotor(int16_t p_value)
{
  if (p_value>=MAX_MOTOR)
	p_value=MAX_MOTOR;
  if (p_value<=-MAX_MOTOR)
	p_value=-(MAX_MOTOR);
  if (p_value==0)
  {
    LL_TIM_OC_SetCompareCH1(TIM22,0);
    LL_TIM_OC_SetCompareCH2(TIM22,0);
  }
  else if (p_value>0) //>0
  {
    LL_TIM_OC_SetCompareCH1(TIM22,p_value);
    LL_TIM_OC_SetCompareCH2(TIM22,0);
  }
  else  //<0
    {
    LL_TIM_OC_SetCompareCH1(TIM22,0);
  LL_TIM_OC_SetCompareCH2(TIM22,-p_value);
    }


#ifdef DISPLAY_V_MOTOR
  {
    int IntPartADC, RemainPartADC;
    IntPartADC=((long)p_value*120)/(MAX_MOTOR*10);
    RemainPartADC=((((long)p_value*1200))%(MAX_MOTOR*100)*10)/MAX_MOTOR;
    if (RemainPartADC<0) RemainPartADC=-RemainPartADC;
    printf("Vmotor=%d.%3.3d\r",IntPartADC,RemainPartADC );
  }
#endif //DISPLAY_V_MOTOR
}


//Protocol main function
void protocolMain(void)
{
  int l_car;
  l_car = rxFifoRead ();
  if (l_car != -1)
  {
    onReceiveCar (l_car);
  }
}


