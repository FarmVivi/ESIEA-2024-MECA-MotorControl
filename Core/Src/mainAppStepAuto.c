/*
 * mainAppStepAuto.c
 *
 *      Author: F. CRISON
 *      Version: 1.0.0
 */
#include "main.h"
#include "stm32l0xx_it.h"

int8_t g_startStep = 0;
int16_t g_motorStep = 0;

#define MAX_DATA1 200
int16_t g_data1Cnt = 0;
int16_t g_data1[MAX_DATA1 + 1];


 int8_t Ad = 1;
 int l_motor;
 int l_epsilon;

//============================================================
//            PID controller
//============================================================
int controlLoop(int p_refValueIn,int p_motorOut)
{

  l_epsilon = (p_refValueIn - p_motorOut);
  l_motor = l_epsilon * Ad ;



  return l_motor;
}
//============================================================





//On new data frame
void onNewFrameCommand(uint8_t p_frameCode)
{
}
void onNewFrameByte(uint8_t p_frameCode, int8_t *p_byte, int p_dataCount)
{
}
void onNewFrameShort(uint8_t p_frameCode, int16_t *p_short, int p_dataCount)
{
  switch (p_frameCode)
  {
    case 'A':  // A + param1=motor_command
      g_startStep = 1;
      g_motorStep = p_short[0];
      g_data1Cnt = 0;
      break;
    case 'P':
      Ad = p_short[0];
      break;
  }
}
void onNewFrameInt(uint8_t p_frameCode, int32_t *p_int, int p_dataCount)
{
}
void onNewFrameFloat(uint8_t p_frameCodet, float *p_float, int p_dataCount)
{
}

//called once every 10 ms
void mainApp10ms(void)
{

  static int l_refValue = 2048;
  static int l_stepDelay=0;
  int l_motor = 0;
  int l_adcValue;

  //Auto step
  l_stepDelay++;
  if (l_stepDelay==400) //every 4s
  {
    l_stepDelay=0;
    if (g_motorStep==1048)
    {
      g_startStep = 1;
      g_motorStep=3048;
    }
    else
    {
      g_startStep = 1;
      g_motorStep=1048;
    }
  }


  //Toggle LED state (PA5)
  LL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);

  //Read potentiometer value (0-4095)
  //(1 LSB=3.3V/4096)
  l_adcValue = readADC();

  //l_motor step
  if (g_startStep == 1)
  {
    l_refValue = g_motorStep;
    g_data1Cnt = 0;
    g_startStep++;
  }
  if (g_startStep == 2)
  {
    g_data1[g_data1Cnt] = l_adcValue;
    g_data1Cnt++;
    if (g_data1Cnt >= MAX_DATA1)
    {
      g_startStep = 0;
      writeFrameShort('A', g_data1, g_data1Cnt);
      g_startStep = 0;
    }

  }

  //Calculate new l_motor voltage
  l_motor=controlLoop(l_refValue,l_adcValue);
    //set motor voltage
  setMotor(l_motor);
}

