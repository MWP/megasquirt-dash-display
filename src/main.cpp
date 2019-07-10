/* ===========================================================================

 5-inch MEGASQUIRT ECU CANBus Dashboard Display
 proof of concept

 This project in current form "works", but is rough around the edges.
 It needs a nicer method of screen configuration, and a LOT of optimisation.

 This project is based around:
 - NodeMCU-32s ESP32 (WROOM-32) module
    https://www.amazon.com/s?k=nodemcu32s
 - Crystalfontz 800x480 TFT with Graphic Accelerator Board
    https://www.crystalfontz.com/product/cfaf800480e0050sca11-accelerated-tft-lcd-display-kit
 - Crystalfontz CFA10098 breakout board
    https://www.crystalfontz.com/product/cfa10098-eve-breakout-board
 - WS2812b LEDs (aka Neopixels)
 	https://www.amazon.com/s?k=neopixel+8+stick
 - Any MC2551 CAN bus Tranciever Module setup for 5V
    https://www.amazon.com/s?k=mcp2551+module
 - Any 5V 2-amp (or higher) regulator suitable for automotive use

 I programmed this in the PlatformIO development environment.
 The Arduino IDE could be used with some minor modifications.
 PlatformIO needs to have the NodeMCU-32s board support (using Arduino framework)
 installed.

 Mark Williams (2019-07-01)
 Distributed under the MIT License (see licence.txt)

 Other source-code / libraries taken from various sources with appropriate licences.
 See licence information contained within those files.

=========================================================================== */

#include <Arduino.h>
#include <SPI.h>

#include "proj_config.h"

#include "ft81x_io.h"
#include "ft81x.h"
#include "cf_gt911.h"

#include "ms_can.h"
#include "ledbar.h"

#include "screens.h"

//globals
char _debugbuf[DPRINT_SIZE];

void core()
{
	uint16_t pos = 0;
	uint8_t touched = 0;
	uint8_t touch_check = 0;
	uint8_t screen_num = 2;
	DPRINT("CORE LOOP\n");

	while(1)
	{
		//gather data
		msCAN_Check();

		//check if eve2 command fifo is empty
		FT81x_FIFO_WaitUntilEmpty();
		//is empty, continue

		//check for screen touch
		touched = !(FT81x_R32(REG_CTOUCH_TOUCH_XY + RAM_REG) & 0x8000);

		//start display update
		FT81x_SendCommand(CMD_DLSTART);
		FT81x_SendCommand(CLEAR(1,1,1));

		//draw display
		switch (screen_num)
		{
			case 0: screen1ms(); break;
			case 1: screen2ms(); break;
			case 2: screen1demo(pos); break;
		}

		//finish update
		FT81x_SendCommand(DISPLAY());
		FT81x_SendCommand(CMD_SWAP);
		//triggers EVE2 to process the command buffer
		FT81x_UpdateFIFO();

		//led bar update
		if (screen_num == 2)
			//demo bar
			LEDBar_Update(0, 100, pos, 65, 85); //demo
		else
			//real rpm bar
			LEDBar_Update(0, 7200, msCAN_U16(msCAN_Data[0].U16[3]), 6400, 7000); //RPM

		//flip screens if display was touched
		//debounce
		if (touch_check == 0)
			//has not been touched
			if (touched)
				touch_check = 1;
		if (touch_check == 1)
			//has been touched, wait for release
			if (!touched)
			{
				//rotate screens
				screen_num++;
				if (screen_num >= SCREENS)
					screen_num = 0;
				//reset
				touch_check = 0;
			}

		//demo screen counter
		pos++;
		if (pos > 100)
		{
			DPRINT("LOOP100\n");
			pos = 0;
		}
	}
}

void setup()
{
	//max out the ESP32
	setCpuFrequencyMhz(240);

	//debug output
	Serial.begin(115200);
	DPRINT("\n\nSETUP STARTED\n");

	//led bar
	LEDBar_Init();

	//can setup
	msCAN_Init();

	//lcd io setup
	DPRINT("LCD SETUP\n");
	SPI_Setup();
	FT81x_Init();
	FT81x_SetBacklight(10);
	//increase SPI rate after init
	SPI_FullSpeed();

	//inits done
	DPRINTF("SPI-CLK=%d\n", spiClockDivToFrequency(SPI.getClockDivider()));
	DPRINT("SETUP DONE\n");

	//main loop
	core();
}

void loop(void)
{
	//not used
}
