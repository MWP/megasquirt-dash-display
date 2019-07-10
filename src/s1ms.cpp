/* ===========================================================================

 5-inch MEGASQUIRT ECU CANBus Dashboard Display
 Simple Display Screen

 Mark Williams (2019-07-01)
 Distributed under the MIT License (see licence.txt)

=========================================================================== */
#include <Arduino.h>
#include "proj_config.h"
#include "ft81x.h"
#include "gauge_bars.h"
#include "gauge_rdial.h"
#include "ms_can.h"

//MS INI data in this example is for MS3-pre1.5.2beta4 firmware

void screen1ms(void)
{
	//top RPM bar
	//rpm = scalar, U16, 6, "RPM", 1.000, 0.0
	float RPM = msCAN_U16(msCAN_Data[0].U16[3]);
	Gauge_HDashBar(20, 10, 800-20, 80,
		8, 15,
		COLOR_RGB(10,10,30),
		COLOR_RGB(0,255,0),
		COLOR_RGB(255,153,0), 6400,
		COLOR_RGB(255,0,0), 7000,
		BARS_SHOWTEXT,
		0, 7200, RPM,
		140, 22,
		"", "%0.0f", 65, 31);

	//AFR gauge
	//called AFR0 in TunerStudio, but AFR1 in MS ini
	//afr1 = scalar, U08,  252, "AFR", 0.1, 0.0
	float AFR = msCAN_Data[31].U08[0] * 0.1f;
	Gauge_RDial(
		110,220, 100,
		225, 135,
		10, 20,
		3,
		2, 80, 2,
		0.5, 85, 0.8,
		65, "%0.0f", 1, 26,
		2.5, AFR,
		"AFR", 28,
		"%0.1f", 1, 29
	);

	//MAP gauge
	//map = scalar, S16,   18, "kPa", 0.100, 0.0
	float MAP = msCAN_S16(msCAN_Data[2].S16[1]) * 0.1f;
	//convert from KPA to PSI
	MAP /= 6.895f;
	Gauge_RDial(
		400,220, 100,
		225, 135,
		-10, 25,
		3,
		5, 80, 2,
		1, 85, 0.8,
		65, "%0.0f", 1, 26,
		2.5, MAP,
		"MAP", 28,
		"%0.1f", 1, 29
	);

	//Oil Pressure
	//I have set this up as a generic input on sensor7
	//sensor07 = scalar, S16,  116, "", 0.1000, 0.0
	float OILP = msCAN_S16(msCAN_Data[14].S16[2]) * 0.1f;
	//it's in KPA, convert to PSI
	OILP /= 6.895f;
	Gauge_RDial(
		690,220, 100,
		0, 270,
		0, 80,
		3,
		10, 80, 2,
		5, 85, 0.8,
		65, "%0.0f", 1, 26,
		2.5, OILP,
		"OIL-P", 28,
		"%0.0f", 1, 29
	);

	//Coolant Temperature (celcius)
	//coolant = scalar, S16,   22, "°C",  0.05555, -320.0
	float CLT = (msCAN_S16(msCAN_Data[2].S16[3]) - 320) * 0.05555f;
	Gauge_RDial(
		255,370, 100,
		0, 270,
		0, 100,
		3,
		10, 80, 2,
		5, 85, 0.8,
		65, "%0.0f", 1, 26,
		2.5, CLT,
		"CL-T", 28,
		"%0.0f", 1, 29
	);

	//Coolant Pressure
	//I have set this up as a generic input on sensor9
	//sensor09 = scalar, S16,  120, "",   0.1000, 0.0
	float CLP = msCAN_S16(msCAN_Data[15].S16[0]) * 0.1f;
	//it's in KPA, convert to PSI
	CLP /= 6.895f;
	Gauge_RDial(
		545,370, 100,
		0, 270,
		0, 25,
		3,
		5, 80, 2,
		1, 85, 0.8,
		65, "%0.0f", 1, 26,
		2.5, CLP,
		"CL-P", 28,
		"%0.1f", 1, 29
	);

}
