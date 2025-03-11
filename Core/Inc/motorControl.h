/*
 * protocol.h
 *
 *  Created on: 3 nov. 2021
 *      Author: F. CRISON
 */

#ifndef SRC_MOTORCONTROL_H_
#define SRC_MOTORCONTROL_H_

#define MAX_MOTOR 1000 //100%=maximum voltage

//Return ADC converter value (0-4095) (1 LSB=3.3V/4096)
uint16_t readADC(void);

//Set motor voltage ratio (-MAX_MOTOR/-12V to +MAX_MOTOR/+12V)
void setMotor(int16_t p_value);

//Protocol main function
void protocolMain(void);

#endif /* SRC_MOTORCONTROL_H_ */
