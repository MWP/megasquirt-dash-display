/* ===========================================================================

 5-inch MEGASQUIRT ECU CANBus Dashboard Display
 WS2812b led strip driver

 Mark Williams (2019-07-01)
 Distributed under the MIT License (see licence.txt)

=========================================================================== */
#include <Arduino.h>
#include "proj_config.h"
#include "esp32WS2811.h"
#include "ledbar.h"

static WS2811 led_bar(LEDBAR_PIN, 16);

void LEDBar_Init(void)
{
	DPRINT("LEDBAR SETUP\n");

	//setup the GPIO mode
	pinMode(LEDBAR_PIN, OUTPUT);

	//startup the WS2811 lib
	led_bar.begin();
	//all dark white
	led_bar.setAll(0,0,0);
	led_bar.setPixel(0, 2, 2, 2);
	led_bar.setPixel(LEDBAR_LEDNUM-1, 2, 2, 2);
	led_bar.show();

	DPRINT("LEDBAR SETUP DONE\n");
}

void LEDBar_Update(int min_value, int max_value, int value,
	int orange_value, int red_value)
{
	//very simple RPM style bar
	int	green_pos = 0;
	uint8_t full_orange = 0;
	uint8_t full_red = 0;

	//calc state
	if (value > min_value)
	{
		//some leds are on
		if (value < orange_value)
		{
			//calc green bar pos
			green_pos = ((value - min_value) * LEDBAR_LEDNUM) / (orange_value - min_value);
		}
		else
		{
			//orange or red
			if (value > red_value)
				full_red = 1;
			else
				full_orange = 1;
		}
	}

	//update leds
	if (full_red)
	{
		//full red bar
		led_bar.setAll(5,0,0);
	}
	else if (full_orange)
	{
		//full orange bar
		led_bar.setAll(4,2,0);
	}
	else
	{
		//green progression bar
		for (uint8_t p = 0; p < 16; p++)
			if (p < green_pos)
				//set led green
				led_bar.setPixel((LEDBAR_LEDNUM-1)-p, 0, 2, 0);
			else
				//set led blue (=off)
				led_bar.setPixel((LEDBAR_LEDNUM-1)-p, 0, 0, 1);
	}
	//update
	led_bar.show();
}
