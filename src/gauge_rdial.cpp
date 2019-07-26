/* ===========================================================================

 5-inch MEGASQUIRT ECU CANBus Dashboard Display
 Simple graphic renderers

 Mark Williams (2019-07-01)
 Distributed under the GNU GENERAL PUBLIC LICENSE v3.0 (see LICENCE file)

=========================================================================== */
#include <Arduino.h>
#include <math.h>
#include "proj_config.h"
#include "ft81x.h"

static const float pi = 3.1415926535f;

void point_at(float cx, float cy, float radius, float angle)
{
	float r = ((angle - 90) / 360.0f) * 2 * pi;
	float x = (radius * cos(r)) + cx;
	float y = (radius * sin(r)) + cy;
	FT81x_SendCommand(VERTEX2F(((int)x) * 16, ((int)y) * 16));
}

void radial_line(float cx, float cy, float start_rad, float end_rad, float angle)
{
	float r = ((angle - 90) / 360.0f) * 2 * pi;
	float x = (start_rad * cos(r)) + cx;
	float y = (start_rad * sin(r)) + cy;
	FT81x_SendCommand(VERTEX2F(((int)x) * 16, ((int)y) * 16));
	x = (end_rad * cos(r)) + cx;
	y = (end_rad * sin(r)) + cy;
	FT81x_SendCommand(VERTEX2F(((int)x) * 16, ((int)y) * 16));
}

void angular_line(float cx, float cy, float radius, float start_angle, float end_angle)
{
	float r = ((start_angle - 90) / 360.0f) * 2 * pi;
	float x = (radius * cos(r)) + cx;
	float y = (radius * sin(r)) + cy;
	FT81x_SendCommand(VERTEX2F(((int)x) * 16, ((int)y) * 16));
	r = ((end_angle - 90) / 360.0f) * 2 * PI;
	x = (radius * cos(r)) + cx;
	y = (radius * sin(r)) + cy;
	FT81x_SendCommand(VERTEX2F(((int)x) * 16, ((int)y) * 16));
}

void text_at(float cx, float cy, float radius, float angle, uint8_t font, char *text)
{
	float r = ((angle - 90) / 360.0f) * 2 * pi;
	float x = (radius * cos(r)) + cx;
	float y = (radius * sin(r)) + cy;
	FT81x_Text(x, y, font, OPT_CENTER, text);
}

void Gauge_RDial(uint16_t cx, uint16_t cy, uint16_t circle_radius,
	float angle_start, float angle_end,
	float value_start, float value_end,
	uint8_t circle_weight,
	float major_spacing, float major_iradius, float major_width,
	float minor_spacing, float minor_iradius, float minor_width,
	float major_num_radius, const char *major_format, float major_multiplier, uint8_t major_font,
	float dial_weight, float value,
	const char *gauge_name_label, uint8_t gauge_label_font,
	const char *value_format, float value_multiplier, uint8_t value_font
)
{
	//create a custom gauge background
	//ported from (ewwwww) PHP
	char tf[16];

	//pre-calc
	float travel = 0;
	if (angle_start < angle_end)
		travel = angle_end - angle_start;
	else
		travel = (360 - angle_start) + angle_end;

	//setup
	FT81x_SendCommand(COLOR_A(255));
	FT81x_SendCommand(COLOR_RGB(255,255,255));

	//major circle
	FT81x_SendCommand(BEGIN(POINTS));
	FT81x_SendCommand(POINT_SIZE(((uint16_t)circle_radius+1) * 16));
	FT81x_SendCommand(VERTEX2F(cx * 16, cy * 16));
	FT81x_SendCommand(COLOR_RGB(0,0,0));
	FT81x_SendCommand(POINT_SIZE(((uint16_t)circle_radius-circle_weight-1) * 16));
	FT81x_SendCommand(VERTEX2F(cx * 16, cy * 16));
	FT81x_SendCommand(COLOR_RGB(255,255,255));

	//minor dashes
	float incr_angle = (travel / (value_end - value_start)) * minor_spacing;
	float steps = (value_end - value_start) / minor_spacing;
	float angle = angle_start;
	uint16_t s;
	FT81x_SendCommand(BEGIN(LINES));
	FT81x_SendCommand(LINE_WIDTH((uint16_t)(minor_width * 16)));
	for (s = 0; s < steps + 1; s++)
	{
		if (angle >= 360)
			angle -= 360;
		radial_line(cx, cy, minor_iradius, circle_radius-circle_weight-1, angle);
		angle += incr_angle;
	}

	//major dashes
	incr_angle = (travel / (value_end - value_start)) * major_spacing;
	steps = (value_end - value_start) / major_spacing;
	angle = angle_start;
	FT81x_SendCommand(COLOR_RGB(255,255,255));
	FT81x_SendCommand(BEGIN(LINES));
	FT81x_SendCommand(LINE_WIDTH((uint16_t)(major_width * 16)));
	for (s = 0; s < steps + 1; s++)
	{
		if (angle >= 360)
			angle -= 360;
		radial_line(cx,cy, major_iradius, circle_radius-circle_weight-1, angle);
		angle += incr_angle;
	}

	//majors text
	angle = angle_start;
	float tvalue = value_start;
	for (s = 0; s < steps + 1; s++)
	{
		if (angle >= 360)
			angle -= 360;
		sprintf(tf, major_format, tvalue * major_multiplier);
		text_at(cx,cy, major_num_radius, angle, major_font, tf);
		angle += incr_angle;
		tvalue += major_spacing;
	}

	//gauge label
	if (gauge_name_label[0] != 0)
		FT81x_Text(cx, cy-(circle_radius/4), gauge_label_font, OPT_CENTER, gauge_name_label);

	//value text
	if (value_format[0] != 0)
	{
		sprintf(tf, value_format, value_multiplier * value);
		FT81x_Text(cx, cy+(circle_radius/4), value_font, OPT_CENTER, tf);
	}

	//dial (center dot)
	if (value > value_end) value = value_end;
	if (value < value_start) value = value_start;
	value = value - value_start;
	float dangle = (travel / (value_end - value_start)) * value;
	dangle += angle_start;
	if (dangle >= 360) dangle -= 360;
	float dr = ((dangle - 90) / 360.0f) * 2 * PI;
	float dx = ((circle_radius-(circle_weight*4)) * cos(dr)) + cx;
	float dy = ((circle_radius-(circle_weight*4)) * sin(dr)) + cy;
	FT81x_SendCommand(COLOR_RGB(255,255,0));
	FT81x_SendCommand(BEGIN(LINES));
	FT81x_SendCommand(LINE_WIDTH((uint8_t)(dial_weight * 16)));
	FT81x_SendCommand(VERTEX2F(cx * 16, cy * 16));
	FT81x_SendCommand(VERTEX2F((int)(dx * 16.0f), (int)(dy * 16.0f)));

	//center circle
	FT81x_SendCommand(BEGIN(POINTS));
	float ccsize = (dial_weight * 3) * 16;
	FT81x_SendCommand(POINT_SIZE((uint8_t)ccsize));
	FT81x_SendCommand(VERTEX2F(cx * 16, cy * 16));
	FT81x_SendCommand(COLOR_RGB(0,0,0));
	FT81x_SendCommand(POINT_SIZE((uint8_t)(ccsize - (dial_weight*16))));
	FT81x_SendCommand(VERTEX2F(cx * 16, cy * 16));
	FT81x_SendCommand(COLOR_RGB(255,255,255));

}
