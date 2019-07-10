/* ===========================================================================

 5-inch MEGASQUIRT ECU CANBus Dashboard Display
 Simple graphic renderers

 Mark Williams (2019-07-01)
 Distributed under the MIT License (see licence.txt)

=========================================================================== */
#ifndef __GAUGEBARS_H
#define __GAUGEBARS_H

#define BARS_SHOWTEXT	(1<<0)

void Gauge_HDashBar(uint16_t x, uint16_t y, uint16_t bar_width, uint16_t bar_height,
	uint8_t bar_lwidth, uint8_t bar_lgap,
	uint32_t offcolor,
	uint32_t acolor,
	uint32_t bcolor, float bvalue,
	uint32_t ccolor, float cvalue,
	uint16_t options,
	float min_value, float max_value, float value,
	uint16_t label_box_width, uint16_t label_box_height,
	const char *gauge_name_label, const char *value_format, float value_multiplier, uint8_t value_font);

void Gauge_VDashBar(uint16_t x, uint16_t y, uint16_t bar_width, uint16_t bar_height,
	uint8_t bar_lheight, uint8_t bar_lgap,
	uint32_t offcolor,
	uint32_t acolor,
	uint32_t bcolor, float bvalue,
	uint32_t ccolor, float cvalue,
	uint16_t options,
	float min_value, float max_value, float value,
	uint16_t label_box_width, uint16_t label_box_height,
	const char *gauge_name_label, const char *value_format, float value_multiplier, uint8_t value_font);

#endif
