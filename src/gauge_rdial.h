/* ===========================================================================

 5-inch MEGASQUIRT ECU CANBus Dashboard Display
 Simple graphic renderers

 Mark Williams (2019-07-01)
 Distributed under the MIT License (see licence.txt)

=========================================================================== */
#ifndef __GAUGERDIAL_H
#define __GAUGERDIAL_H

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
);

#endif
