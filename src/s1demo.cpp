/* ===========================================================================

 5-inch MEGASQUIRT ECU CANBus Dashboard Display
 Simple Display Screen Demo

 Mark Williams (2019-07-01)
 Distributed under the MIT License (see licence.txt)

=========================================================================== */
#include <Arduino.h>
#include "proj_config.h"
#include "ft81x.h"
#include "gauge_bars.h"
#include "gauge_rdial.h"

void screen1demo(int pos)
{
	//rpm bar
	Gauge_HDashBar(20, 10, 800-20, 80,
		8, 15,
		COLOR_RGB(10,10,30),
		COLOR_RGB(0,255,0),
		COLOR_RGB(255,153,0), 65,
		COLOR_RGB(255,0,0), 85,
		BARS_SHOWTEXT,
		0, 100, pos,
		140, 22,
		"", "%0.0f", 65, 31);

	//upper
	Gauge_RDial(
		110,220, 100,
		225, 135,
		10, 20,
		3,
		2, 80, 2,
		0.5, 85, 0.8,
		65, "%0.0f", 1, 26,
		2.5, pos,
		"AFR", 28,
		"%0.1f", 1, 29
	);

	Gauge_RDial(
		400,220, 100,
		225, 135,
		-10, 25,
		3,
		5, 80, 2,
		1, 85, 0.8,
		65, "%0.0f", 1, 26,
		2.5, pos,
		"MAP", 28,
		"%0.0f", 1, 29
	);

	Gauge_RDial(
		690,220, 100,
		0, 270,
		0, 80,
		3,
		10, 80, 2,
		5, 85, 0.8,
		65, "%0.0f", 1, 26,
		2.5, pos,
		"OIL-P", 28,
		"%0.0f", 1, 29
	);

	//lower
	Gauge_RDial(
		255,370, 100,
		0, 270,
		0, 100,
		3,
		10, 80, 2,
		5, 85, 0.8,
		65, "%0.0f", 1, 26,
		2.5, pos,
		"CL-T", 28,
		"%0.0f", 1, 29
	);

	Gauge_RDial(
		545,370, 100,
		0, 270,
		0, 25,
		3,
		5, 80, 2,
		1, 85, 0.8,
		65, "%0.0f", 1, 26,
		2.5, pos,
		"CL-P", 28,
		"%0.1f", 1, 29
	);
}
