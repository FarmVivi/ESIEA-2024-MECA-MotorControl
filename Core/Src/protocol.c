/*
 * protocol.c
 *
 *      Author: F. CRISON
 *      Version: 1.0.0
 */
#include "main.h"
#include "stm32l0xx_it.h"


// Procotol byte order: little-endian
//                      ==========
//Numbers in "pseudo hexa" ASCII format   e.g: 0=0x30  1=0x31  ... E=0x3E F=0x3F ...)

//============================================================
//Data frame structure
// [  1 byte   ]  [  1 byte  ] [  1 byte  ]  [ 0 to N byte ]  [ 1 byte  ]
// [FRAME_START]  [frame code] [frame_type]  [....data.....]  [FRAME_END]
//
// [frame code] : code of the frame : 'A', 'B', ...;
//
//frame_type  :
//    'e' (0x65) = empty (no data -> command frame)
//    'b' (0x62) = integer 8 bits
//    's' (0x73) = integer 16 bits
//    'i' (0x6C) = integer 32 bits
//    'f' (0x66) = float 32 bits
//
// DATA: Numbers in "pseudo hexa" ASCII format   e.g: 0=0x30  1=0x31  ... E=0x3E F=0x3F ...
//    example: number=0x3AB9  ==>  data= [0x33 0x3A 0x3B 0x39]
#define FRAME_START 0x55
#define FRAME_STOP  0xAA
//============================================================

int8_t testProtocol=0; // only for test


typedef union
{
  int8_t i8;
  int16_t i16;
  int32_t i32;
  float f32;
  uint8_t bytes[4];
}convNumByte;

// Max data parameters according to  type
#define MAX_DATA_i8 64
#define MAX_DATA_i16 MAX_DATA_i8/2
#define MAX_DATA_i32 MAX_DATA_i8/4
#define MAX_DATA_f32 MAX_DATA_i8/4
typedef union
{
  int8_t i8[MAX_DATA_i8];
  int16_t i16[MAX_DATA_i16];
  int32_t i32[MAX_DATA_i32];
  float f32[MAX_DATA_f32];
}dataParameter;

//Write a byte in "pseudo hexa" ascii format
void writeByte(int8_t p_byte )
{
  serialWrite(0x30 | ((p_byte>>4)&0x0F));
  serialWrite(0x30 | (p_byte&0x0F));
}
//Write a short in "pseudo hexa" ascii format - little-endian
void writeShort(int16_t p_short)
{
  convNumByte l_conv;
  l_conv.i16=p_short;
  writeByte(l_conv.bytes[0]); //Little-indian
  writeByte(l_conv.bytes[1]);
}
//Write an int in "pseudo hexa" ascii format - little-endian
void writeInt(int32_t p_int)
{
  convNumByte l_conv;
  l_conv.i32=p_int;
  writeByte(l_conv.bytes[0]);
  writeByte(l_conv.bytes[1]);
  writeByte(l_conv.bytes[2]);
  writeByte(l_conv.bytes[3]);
}
//Write a float in "pseudo hexa" ascii format - little-endian
void writeFloat(float p_float)
{
  convNumByte l_conv;
  l_conv.f32=p_float;
  writeByte(l_conv.bytes[0]);
  writeByte(l_conv.bytes[1]);
  writeByte(l_conv.bytes[2]);
  writeByte(l_conv.bytes[3]);
}
//Write start frame byte
void writeFrameStart(void)
{
  serialWrite(FRAME_START);
}
//Write end frame byte
void writeFrameStop(void)
{
  serialWrite(FRAME_STOP);
}

//Write a frame without any data
void writeFrame(uint8_t p_code)
{
  writeFrameStart();
  serialWrite(p_code);
  serialWrite('e');
  writeFrameStop();
}
//Write a frame with p_size bytes
void writeFrameByte(uint8_t p_code, int8_t * p_bytes, int p_size)
{
  int i;
  writeFrameStart();
  serialWrite(p_code);
  serialWrite('b');
  for(i=0;i<p_size;i++)
  {
    writeByte(p_bytes[i]);
  }
  writeFrameStop();
}
//Write a frame with p_size shorts
void writeFrameShort(uint8_t p_code, int16_t * p_short, int p_size)
{
  int i;
  writeFrameStart();
  serialWrite(p_code);
  serialWrite('s');
  for(i=0;i<p_size;i++)
  {
    writeShort(p_short[i]);
  }
  writeFrameStop();
}
//Write a frame with p_size ints
void writeFrameInt(uint8_t p_code, int32_t * p_int, int p_size)
{
  int i;
  writeFrameStart();
  serialWrite(p_code);
  serialWrite('i');
  for(i=0;i<p_size;i++)
  {
    writeInt(p_int[i]);
  }
  writeFrameStop();
}
//Write a frame with p_size ints
void writeFrameFloat(uint8_t p_code, float * p_float, int p_size)
{
  int i;
  writeFrameStart();
  serialWrite(p_code);
  serialWrite('f');
  for(i=0;i<p_size;i++)
  {
    writeFloat(p_float[i]);
  }
  writeFrameStop();
}


typedef enum
{
  IDLE,
  R_FRAME_START,
  R_FRAME_CODE,
  R_FRAME_TYPE,
  R_FRAME_STOP
}rxState;


//Protocol management
void onReceiveCar(uint8_t p_car)
{
  static rxState l_stateRx=IDLE; //wait for FRAME_START
  static uint8_t l_frameCode=0;
  static uint8_t l_frameType=0;
  static convNumByte l_conv;
  static uint8_t l_receiveCount=0;
  static dataParameter l_data;
  static uint8_t l_dataCount=0;
  static uint8_t l_pseudoHexa[2];
  static uint8_t l_pseudoHexaCnt=0;


  if (p_car==FRAME_STOP)
  {
      l_stateRx=R_FRAME_STOP;
  }

  switch(l_stateRx)
  {
    case R_FRAME_STOP:  //end of frame
      switch(l_frameType)
      {
	//    'e' (0x65) = command frame
	case 'e':
	  //mode test
	  if (l_frameCode == 254)
	  {
	    testProtocol=1;  //library test mode
	  }
	  else if (l_frameCode == 255)
	  {
	    testProtocol=0;  //library test mode end
	  }
	  else if (testProtocol==0) //test mode?
	    onNewFrameCommand(l_frameCode);
	  else
	    writeFrame(l_frameCode); //echo
	  break;
	//    'b' (0x62) = integer 8 bits
	case 'b':
	  if (testProtocol==0) //test mode?
	    onNewFrameByte(l_frameCode,l_data.i8,l_dataCount);
	  else
	    writeFrameByte(l_frameCode,l_data.i8,l_dataCount); //echo for test
	  break;
	//    's' (0x73) = integer 16 bits
	case 's':
	  if (testProtocol==0) //test mode?
	    onNewFrameShort(l_frameCode,l_data.i16,l_dataCount);
	  else
	    writeFrameShort(l_frameCode,l_data.i16,l_dataCount); //echo for test
	  break;
	//    'i' (0x6C) = integer 32 bits
	case 'i':
	  if (testProtocol==0) //test mode?
	    onNewFrameInt(l_frameCode,l_data.i32,l_dataCount);
	  else
	    writeFrameInt(l_frameCode,l_data.i32,l_dataCount); //echo for test
	  break;
	//    'f' (0x66) = float 32 bits
	case 'f':
	  if (testProtocol==0) //test mode?
	    onNewFrameFloat(l_frameCode,l_data.f32,l_dataCount);
	  else
	    writeFrameFloat(l_frameCode,l_data.f32,l_dataCount); //echo for test
	  break;
      }
      l_receiveCount=0;
      l_dataCount=0;
      l_pseudoHexaCnt=0;
      l_stateRx=IDLE;
      break;
    case IDLE:
      if (p_car==FRAME_START)
	l_stateRx=R_FRAME_START;
      break;
    case R_FRAME_START:  //receive frame code
      l_frameCode=p_car;
      l_stateRx=R_FRAME_CODE;
      break;
    case R_FRAME_CODE:  //receive frame type
      l_frameType=p_car;
      l_stateRx=R_FRAME_TYPE;
      break;
    case R_FRAME_TYPE:  //receive data
      if (l_pseudoHexaCnt==0)
      {
	l_pseudoHexaCnt=1;
	l_pseudoHexa[0]=p_car;
	break;
      }
      else
      {
	l_pseudoHexa[1]=p_car;
	l_pseudoHexaCnt=0;
	//pseudo hexa to char conv
	p_car=((l_pseudoHexa[0]&0x0F)<<4) | (l_pseudoHexa[1]&0x0F);
      }

      l_conv.bytes[l_receiveCount]=p_car;
      l_receiveCount++;
      switch(l_frameType)
      {
	//switch frame_type

	//    'b' (0x62) = integer 8 bits
	case 'b':
	  if (l_dataCount==MAX_DATA_i8) // rejects the parameter: too many data parameters
	    break;
	  l_data.i8[l_dataCount]=p_car;
	  l_dataCount++;
	  l_receiveCount=0;
	  break;

	//    's' (0x73) = integer 16 bits
	case 's':
	  if (l_dataCount==MAX_DATA_i16) // rejects the parameter: too many data parameters
	    break;
	  if (l_receiveCount==2)
	  {
	    l_data.i16[l_dataCount]=l_conv.i16;
	    l_dataCount++;
	    l_receiveCount=0;
	  }
	  break;

	//    'i' (0x6C) = integer 32 bits
	case 'i':
	  if (l_dataCount==MAX_DATA_i32) // rejects the parameter: too many data parameters
	    break;
	  if (l_receiveCount==4)
	  {
	    l_data.i32[l_dataCount]=l_conv.i32;;
	    l_dataCount++;
	    l_receiveCount=0;
	  }
	  break;

	//    'f' (0x66) = float 32 bits
	case 'f':
	  if (l_dataCount==MAX_DATA_f32) // rejects the parameter: too many data parameters
	    break;
	  if (l_receiveCount==4)
	  {
	    l_data.f32[l_dataCount]=l_conv.f32;;
	    l_dataCount++;
	    l_receiveCount=0;
	  }
	  break;
      }
      break;
  }

}


