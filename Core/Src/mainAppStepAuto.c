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


float Ad = 0.1514; // Facteur de proportionnalité du régulateur
float Ab = 3.0388; // Facteur de la chaîne de retour

//============================================================
//            PID controller
//============================================================
int controlLoop(int p_refValueIn,int p_motorOut)
{
	// Soustracteur
	float l_epsilon = (p_refValueIn - p_motorOut * Ab);

	// Gain proportionnel
	float l_ADe = l_epsilon;
	float l_ADs = l_ADe * Ad;

	return l_ADs; // Retourne la commande moteur calculée
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
  static int l_refValue = 2048; // Valeur de consigne initiale
  static int l_stepDelay=0;
  int l_motor = 0;
  int l_adcValue;

  // Gestion de l'alternance de la consigne moteur
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

  // Gestion de la séquence de step moteur
  //l_motor step
  if (g_startStep == 1)
  {
    l_refValue = g_motorStep; // Mise à jour de la consigne
    g_data1Cnt = 0;
    g_startStep++;
  }
  if (g_startStep == 2)
  {
    g_data1[g_data1Cnt] = l_adcValue; // Enregistrement de la valeur ADC
    g_data1Cnt++;
    if (g_data1Cnt >= MAX_DATA1)
    {
      g_startStep = 0;
      writeFrameShort('A', g_data1, g_data1Cnt); // Envoi des données
      g_startStep = 0;
    }

  }

  // Calcul de la commande moteur
  //Calculate new l_motor voltage
  l_motor=controlLoop(l_refValue,l_adcValue);
  // Application de la tension moteur
    //set motor voltage
  setMotor(l_motor);
}

