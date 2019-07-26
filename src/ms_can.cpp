/* ===========================================================================

 5-inch MEGASQUIRT ECU CANBus Dashboard Display
 Megasquirt CANBus Interface

 Mark Williams (2019-07-01)
 Distributed under the GNU GENERAL PUBLIC LICENSE v3.0 (see LICENCE file)

=========================================================================== */
#include <Arduino.h>
#include <esp32_can.h>
#include "proj_config.h"
#include "ms_can.h"

//types of data contained in CAN packets can be found in the relevant MS.ini file.
//See under [OutputChannels] section

//remember data may need endianness conversion

ms3_8b_t msCAN_Data[MSCAN_LEN];

void printFrame(CAN_FRAME *message)
{
	Serial.print(message->id, HEX);
	if (message->extended)
		Serial.print(" X ");
	else
		Serial.print(" S ");
	Serial.print(message->length, DEC);
	Serial.print(" ");
	for (int i = 0; i < message->length; i++)
	{
		Serial.print(message->data.byte[i], DEC);
		Serial.print(" ");
	}
	Serial.println();
}

void msCAN_Init(void)
{
	DPRINT("CAN SETUP\n");

	//init can
	memset(msCAN_Data, 0x00, MSCAN_LEN * sizeof(ms3_8b_t));
	CAN0.begin(500000); //500k bit CAN
	CAN0.watchFor();

	//first check
	msCAN_Check();

	DPRINT("CAN SETUP DONE\n");
}

void msCAN_Check(void)
{
	CAN_FRAME incoming;

	//check for megasquirt CAN messages
	while (CAN0.read(incoming))
	{
		//check all incoming CAN packets for valid ID range
		if ((incoming.id >= MSCAN_BASE) && (incoming.id < MSCAN_BASE + MSCAN_LEN))
		{
			//possibly got a MS CAN DATA PACKET, keep it
			memset(msCAN_Data[incoming.id - MSCAN_BASE].U08, 0x00, 8);
			memcpy(msCAN_Data[incoming.id - MSCAN_BASE].U08, incoming.data.byte, incoming.length);
			// printFrame(&incoming); //debug output
		}
	}
}

uint16_t msCAN_U16(uint16_t in)
{
#ifdef MSCAN_SWAPBYTES
	return __builtin_bswap16(in);
#else
	return in;
#endif
}

uint32_t msCAN_U32(uint32_t in)
{
#ifdef MSCAN_SWAPBYTES
	return __builtin_bswap32(in);
#else
	return in;
#endif
}

int16_t msCAN_S16(int16_t in)
{
#ifdef MSCAN_SWAPBYTES
	ms_16b t;
	t.S = in;
	t.U = __builtin_bswap16(t.U);
	return t.S;
#else
	return in;
#endif
}

int32_t msCAN_S32(int32_t in)
{
#ifdef MSCAN_SWAPBYTES
	ms_32b t;
	t.S = in;
	t.U = __builtin_bswap32(t.U);
	return t.S;
#else
	return in;
#endif
}
