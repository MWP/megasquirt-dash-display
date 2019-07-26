/* ===========================================================================

 5-inch MEGASQUIRT ECU CANBus Dashboard Display
 WS2812b led strip driver

 Mark Williams (2019-07-01)
 Distributed under the GNU GENERAL PUBLIC LICENSE v3.0 (see LICENCE file)

=========================================================================== */
#ifndef __LEDBAR_H
#define __LEDBAR_H

void LEDBar_Init(void);
void LEDBar_Update(int min_value, int max_value, int value,
	int orange_value, int red_value);


#endif
