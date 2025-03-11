/*
 * protocol.h
 *
 *  Created on: 1 oct. 2021
 *      Author: F. CRISON
 */

#ifndef SRC_PROTOCOL_H_
#define SRC_PROTOCOL_H_

void writeByte(int8_t p_byte );
void writeShort(int16_t p_short);
void writeInt(int32_t p_int);
void writeFloat(float p_float);
void writeFrameStart(void);
void writeFrameStop(void);

//Frames
void writeFrame(uint8_t p_code);
void writeFrameByte(uint8_t p_code, int8_t * p_bytes, int p_size);
void writeFrameShort(uint8_t p_code, int16_t * p_short, int p_size);
void writeFrameInt(uint8_t p_code, int32_t * p_int, int p_size);
void writeFrameFloat(uint8_t p_code, float * p_float, int p_size);

//Protocol management
void onReceiveCar(uint8_t p_car);

//On new data frame
void onNewFrameCommand(uint8_t p_frameCode);
void onNewFrameByte(uint8_t p_frameCode,int8_t * p_byte,int p_dataCount);
void onNewFrameShort(uint8_t p_frameCode,int16_t * p_short,int p_dataCount);
void onNewFrameInt(uint8_t p_frameCode,int32_t * p_int,int p_dataCount);
void onNewFrameFloat(uint8_t p_frameCode,float * p_float,int p_dataCount);

#endif /* SRC_PROTOCOL_H_ */
