/* ===========================================================================

 5-inch MEGASQUIRT ECU CANBus Dashboard Display
 Simple graphic renderers

 Mark Williams (2019-07-01)
 Distributed under the MIT License (see licence.txt)

=========================================================================== */

#include <Arduino.h>
#include "gauge_bars.h"
#include "proj_config.h"
#include "ft81x.h"

static void Gauge_UpdateColor(uint32_t color)
{
	//only update color if different to last
	static uint32_t last = 0;
	if (color != last)
	{
		FT81x_SendCommand(color);
		last = color;
	}
}

void Gauge_HDashBar(uint16_t x, uint16_t y, uint16_t bar_width, uint16_t bar_height,
	uint8_t bar_lwidth, uint8_t bar_lgap,
	uint32_t offcolor,
	uint32_t acolor,
	uint32_t bcolor, float bvalue,
	uint32_t ccolor, float cvalue,
	uint16_t options,
	float min_value, float max_value, float value,
	uint16_t label_box_width, uint16_t label_box_height,
	const char *gauge_name_label, const char *value_format, float value_multiplier, uint8_t value_font)
{
	//calculate bar position
	uint16_t bar_pos;
	if (value < min_value)
		bar_pos = 0;
	else
		bar_pos = (value - min_value) / max_value * bar_width;
	uint16_t bstart  = (bvalue - min_value) / max_value * bar_width;
	uint16_t cstart  = (cvalue - min_value) / max_value * bar_width;

	//bar
	uint16_t xpos = 0;
	FT81x_SendCommand(BEGIN(LINES));
	FT81x_SendCommand(COLOR_A(255));
	FT81x_SendCommand(LINE_WIDTH(bar_lwidth * 16));
	Gauge_UpdateColor(acolor);
	while (xpos < bar_width)
	{
		//check for color change
		if (xpos >= bar_pos)
		{
			Gauge_UpdateColor(offcolor);
		}
		else if (xpos >= cstart)
		{
			Gauge_UpdateColor(ccolor);
		}
		else if (xpos >= bstart)
		{
			Gauge_UpdateColor(bcolor);
		}
		//line
		FT81x_SendCommand(VERTEX2F((x + xpos) * 16, y * 16));
		FT81x_SendCommand(VERTEX2F((x + xpos) * 16, (y + bar_height) * 16));
		xpos += bar_lwidth + bar_lgap;
	}

	//text
	if (options & BARS_SHOWTEXT)
	{
		char tf[16];
		char tv[16];
		if (gauge_name_label[0] != 0)
		{
			//has label
			sprintf(tv, value_format, value * value_multiplier);
			sprintf(tf, "%s %s", gauge_name_label, tv);
		}
		else
		{
			//empty label
			sprintf(tf, value_format, value * value_multiplier);
		}

		uint16_t box_midx, box_midy;
		box_midx = x + (bar_width/2);
		box_midy = y+(bar_height/2); //-(label_box_height/2)-10;

		FT81x_SendCommand(BEGIN(LINES));
		FT81x_SendCommand(COLOR_A(200));
		Gauge_UpdateColor(COLOR_RGB(0,0,0));
		FT81x_SendCommand(LINE_WIDTH(label_box_height*16));
		FT81x_SendCommand(VERTEX2F((box_midx-(label_box_width/2))*16, box_midy * 16));
		FT81x_SendCommand(VERTEX2F((box_midx+(label_box_width/2))*16, box_midy * 16));

		FT81x_SendCommand(COLOR_A(255));
		Gauge_UpdateColor(COLOR_RGB(255,255,255));
		FT81x_Text(box_midx, box_midy, value_font, OPT_CENTER, tf);
	}
}

void Gauge_VDashBar(uint16_t x, uint16_t y, uint16_t bar_width, uint16_t bar_height,
	uint8_t bar_lheight, uint8_t bar_lgap,
	uint32_t offcolor,
	uint32_t acolor,
	uint32_t bcolor, float bvalue,
	uint32_t ccolor, float cvalue,
	uint16_t options,
	float min_value, float max_value, float value,
	uint16_t label_box_width, uint16_t label_box_height,
	const char *gauge_name_label, const char *value_format, float value_multiplier, uint8_t value_font)
{
	//calculate bar position
	uint16_t bar_pos;
	if (value < min_value)
		bar_pos = 0;
	else
		bar_pos = (value - min_value) / max_value * bar_height;
	uint16_t bstart  = (bvalue - min_value) / max_value * bar_height;
	uint16_t cstart  = (cvalue - min_value) / max_value * bar_height;

	//bar
	uint16_t ypos = 0;
	FT81x_SendCommand(BEGIN(LINES));
	FT81x_SendCommand(COLOR_A(255));
	FT81x_SendCommand(LINE_WIDTH(bar_lheight * 16));
	Gauge_UpdateColor(acolor);
	while (ypos < bar_height)
	{
		//check for color change
		if (ypos >= bar_pos)
		{
			Gauge_UpdateColor(offcolor);
		}
		else if (ypos >= cstart)
		{
			Gauge_UpdateColor(ccolor);
		}
		else if (ypos >= bstart)
		{
			Gauge_UpdateColor(bcolor);
		}
		//line
		uint16_t nypos = bar_height - ypos;
		FT81x_SendCommand(VERTEX2F(x * 16, (y + nypos) * 16));
		FT81x_SendCommand(VERTEX2F((x + bar_width) * 16, (y + nypos) * 16));
		ypos += bar_lheight + bar_lgap;
	}

	//text
	if (options & BARS_SHOWTEXT)
	{
		char tf[16];
		char tv[16];
		if (gauge_name_label[0] != 0)
		{
			//has label
			sprintf(tv, value_format, value * value_multiplier);
			sprintf(tf, "%s %s", gauge_name_label, tv);
		}
		else
		{
			//empty label
			sprintf(tf, value_format, value * value_multiplier);
		}

		uint16_t box_midx, box_midy;
		box_midx = x + (bar_width / 2);
		box_midy = y + bar_height - label_box_height - 10; //-(label_box_height/2)-10;

		FT81x_SendCommand(BEGIN(LINES));
		FT81x_SendCommand(COLOR_A(200));
		FT81x_SendCommand(COLOR_RGB(0,0,0));
		FT81x_SendCommand(LINE_WIDTH(label_box_height*16));
		FT81x_SendCommand(VERTEX2F((box_midx-(label_box_width/2))*16, box_midy * 16));
		FT81x_SendCommand(VERTEX2F((box_midx+(label_box_width/2))*16, box_midy * 16));

		FT81x_SendCommand(COLOR_A(255));
		FT81x_SendCommand(COLOR_RGB(255,255,255));
		FT81x_Text(box_midx, box_midy, value_font, OPT_CENTER, tf);
	}
}
