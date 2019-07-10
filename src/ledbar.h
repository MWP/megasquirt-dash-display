/* ===========================================================================

 5-inch MEGASQUIRT ECU CANBus Dashboard Display
 WS2812b led strip driver

 Mark Williams (2019-07-01)
 Distributed under the MIT License (see licence.txt)

=========================================================================== */
#ifndef __LEDBAR_H
#define __LEDBAR_H

#define LEDBAR_LEDNUM	16

void LEDBar_Init(void);
void LEDBar_Update(int min_value, int max_value, int value,
	int orange_value, int red_value);


#endif
