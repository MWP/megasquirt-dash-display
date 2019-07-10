/* ===========================================================================

 5-inch MEGASQUIRT ECU CANBus Dashboard Display
 Megasquirt CANBus Interface

 Mark Williams (2019-07-01)
 Distributed under the MIT License (see licence.txt)

=========================================================================== */
#ifndef __MSCAN_H
#define __MSCAN_H

#include "proj_config.h"

//MS CAN broadcast base address
#define MSCAN_BASE			1520
//up to 64 following addresses
#define MSCAN_LEN			64

//enable if data needs endianness swapping
#define MSCAN_SWAPBYTES		1

//convienient way of storing a single address of data, 8 bytes
typedef union
{
	uint32_t	U32[2];
	int32_t		S32[2];
	uint16_t	U16[4];
	int16_t		S16[4];
	uint8_t		U08[8];
	int8_t		S08[8];
} ms3_8b_t;

typedef union
{
	uint16_t	U;
	int16_t		S;
} ms_16b;

typedef union
{
	uint32_t	U;
	int32_t		S;
} ms_32b;

//stores can data we recieve
extern ms3_8b_t msCAN_Data[MSCAN_LEN];

void msCAN_Init(void);
void msCAN_Check(void);

uint16_t msCAN_U16(uint16_t in);
uint32_t msCAN_U32(uint32_t in);
int16_t msCAN_S16(int16_t in);
int32_t msCAN_S32(int32_t in);

#endif
