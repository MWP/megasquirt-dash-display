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
 Distributed under the GNU GENERAL PUBLIC LICENSE v3.0 (see LICENCE file)

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

		//check for screen touch
		touched = !(FT81x_R32(REG_CTOUCH_TOUCH_XY + RAM_REG) & 0x8000);

		//The EVE2 has a display list (a list of commands that it processes on every
		//scan line update, this runs continuously), and a co-processor command-list.
		//Either can be used to draw to the display, but using the co-processor has some advantages.
		//When signaled, the co-processor executes the command list in its RAM FIFO
		//buffer, and writes to the display list.

		//for detailed information on using the EVE2 (FT8xx) graphics Accelerator
		//see: https://www.ftdichip.com/Support/Documents/ProgramGuides/FT800%20Programmers%20Guide.pdf

		//check if EVE2 co-processor command list fifo is empty
		//we write commands into the EVE2 co-processor's fifo, which we then signal
		//it to process later. All FT81x_SendCommand() commands go onto the fifo.
		//They are not processed by the EVE2 co-processor until FT81x_UpdateFIFO() is called.
		FT81x_FIFO_WaitUntilEmpty();
		//co-processor fifo is empty, continue

		//tell the co-processor we are starting a new list of commands
		FT81x_SendCommand(CMD_DLSTART);
		//clear the screen
		FT81x_SendCommand(CLEAR(1,1,1));

		//draw display
		//see screen1demo (s1demo.cpp) for detailed comments
		switch (screen_num)
		{
			case 0: screen1ms(); break;
			case 1: screen2ms(); break;
			case 2: screen1demo(pos); break;
		}

		//add to the fifo an end-of-commands marker
		FT81x_SendCommand(DISPLAY());
		//when the co-processor engine executes CMD_SWAP, it requests a display list swap
		//immediately after current display list (current frame) is scanned out.
		FT81x_SendCommand(CMD_SWAP);
		//all done, nothing more to add to the co-processor FIFO, tell the co-processor to
		//start excuting the commands in the FIFO.
		FT81x_UpdateFIFO();
		//while its doing this, we go onto doing other things

		//led bar update
		if (screen_num == 2)
			//demo bar
			LEDBar_Update(0, 100, pos, 65, 85); //demo
		else
			//real rpm bar
			LEDBar_Update(0, 7200, msCAN_U16(msCAN_Data[0].U16[3]), 6400, 7000); //RPM

		//flip screens if display was touched
		//debounce
		if (!touch_check && touched)
			//screen has been touched, but was not touched on last loop
			touch_check = 1;
		if (touch_check && !touched)
		{
			//no longer touched, but was on last loop = touched and released
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
