/* ===========================================================================

 5-inch MEGASQUIRT ECU CANBus Dashboard Display
 Simple Display Screen

 Mark Williams (2019-07-01)
 Distributed under the GNU GENERAL PUBLIC LICENSE v3.0 (see LICENCE file)

=========================================================================== */
#include <Arduino.h>
#include "proj_config.h"
#include "ft81x.h"
#include "gauge_bars.h"
#include "gauge_rdial.h"
#include "ms_can.h"

//MS INI data in this example is for MS3-pre1.5.2beta4 firmware

#define VALUEPAD	130
#define FONTSIZE	31
#define FONTHEIGHT	42

static void textDataf(uint16_t x, uint16_t y, float value, const char *label, const char *format,
	float low_warn, float low_err, float high_warn, float high_err)
{
		//render text label & value
		char temps[32];

		FT81x_SendCommand(COLOR_RGB(255, 255, 255));
		FT81x_Text(x, y, FONTSIZE, 0, label);

		if (value < low_err)
			FT81x_SendCommand(COLOR_RGB(255, 50, 50));
		else if (value < low_warn)
			FT81x_SendCommand(COLOR_RGB(255, 255, 50));
		else if (value > high_err)
			FT81x_SendCommand(COLOR_RGB(255, 50, 50));
		else if (value > high_warn)
			FT81x_SendCommand(COLOR_RGB(255, 255, 50));
		else
			FT81x_SendCommand(COLOR_RGB(120, 255, 120));

		sprintf(temps, format, value);
		FT81x_Text(x + VALUEPAD, y, FONTSIZE, 0, temps);
}

static void textDatai(uint16_t x, uint16_t y, int value, const char *label, const char *format,
	int low_warn, int low_err, int high_warn, int high_err)
{
		//render text label & value
		char temps[32];

		FT81x_SendCommand(COLOR_RGB(255, 255, 255));
		FT81x_Text(x, y, FONTSIZE, 0, label);

		if (value < low_err)
			FT81x_SendCommand(COLOR_RGB(255, 50, 50));
		else if (value < low_warn)
			FT81x_SendCommand(COLOR_RGB(255, 255, 50));
		else if (value > high_err)
			FT81x_SendCommand(COLOR_RGB(255, 50, 50));
		else if (value > high_warn)
			FT81x_SendCommand(COLOR_RGB(255, 255, 50));
		else
			FT81x_SendCommand(COLOR_RGB(100, 255, 100));

		sprintf(temps, format, value);
		FT81x_Text(x + VALUEPAD, y, FONTSIZE, 0, temps);
}

void screen2ms(void)
{
	//thin RPM bar
	//rpm = scalar, U16, 6, "RPM", 1.000, 0.0
	float RPM = msCAN_U16(msCAN_Data[0].U16[3]);
	Gauge_HDashBar(20, 10, 800-20, 40,
		4, 9,
		COLOR_RGB(30,30,140),
		COLOR_RGB(0,255,0),
		COLOR_RGB(255,153,0), 6400,
		COLOR_RGB(255,0,0), 7000,
		BARS_SHOWTEXT,
		0, 7200, RPM,
		100, 16,
		"", "%0.0f", 65, 30);

	//y text position
	uint16_t tpos = 20;

	//Seconds
	//seconds = scalar, U16,    0, "s",   1.000, 0.0
	uint16_t Seconds = msCAN_U16(msCAN_Data[0].U16[0]);
	textDatai(10, tpos+=FONTHEIGHT, Seconds, "UpT:", "%d s", 30, 1, 65536, 65536);

	//RPM
	textDatai(10, tpos+=FONTHEIGHT, RPM, "RPM:", "%d", 500, 100, 6400, 7000);

	//MAP gauge
	//map = scalar, S16,   18, "kPa", 0.100, 0.0
	float MAP = msCAN_S16(msCAN_Data[2].S16[1]) * 0.1f;
	textDataf(10, tpos+=FONTHEIGHT, MAP, "MAP:", "%0.1f kPa", 5,5,240,260);

	//TPS
	//tps = scalar, S16,   24, "%",   0.100, 0.0
	float TPS = msCAN_S16(msCAN_Data[3].S16[0]) * 0.1f;
	textDataf(10, tpos+=FONTHEIGHT, TPS, "TPS:", "%0.1f %%", -1,-20,100,500);

	//AFR gauge
	//called AFR0 in TunerStudio, but AFR1 in MS ini
	//afr1 = scalar, U08,  252, "AFR", 0.1, 0.0
	float EGO = msCAN_Data[31].U08[0] * 0.1f;
	textDataf(10, tpos+=FONTHEIGHT, EGO, "EGO:", "%0.1f AFR", 11,10.5,15.5,17);

	//Coolant Temperature (celcius)
	//coolant = scalar, S16,   22, "°C",  0.05555, -320.0
	float CLT = (msCAN_S16(msCAN_Data[2].S16[3]) - 320) * 0.05555f;
	textDataf(10, tpos+=FONTHEIGHT, CLT, "CLT:", "%0.1f C", 0, -10, 100, 120);

	//Manifold Temperature (celcius)
	//mat = scalar, S16,   20, "°C",  0.05555, -320.0
	float MAT = (msCAN_S16(msCAN_Data[2].S16[2]) - 320) * 0.05555f;
	textDataf(10, tpos+=FONTHEIGHT, MAT, "MAT:", "%0.1f C", 0, -10, 60, 80);

	//Injector PW1
	//pulseWidth1 = scalar, U16,    2, "ms",   0.001, 0.0
	float PW1 = msCAN_U16(msCAN_Data[0].U16[1]) * 0.001f;
	textDataf(10, tpos+=FONTHEIGHT, PW1, "PW1:", "%0.3f mS", 0.01, -10, 10, 11);

	//Battery Voltage
	//batteryVoltage = scalar, S16,   26, "v",   0.100, 0.0
	float BATTV = msCAN_S16(msCAN_Data[3].S16[1]) * 0.1f;
	textDataf(10, tpos+=FONTHEIGHT, BATTV, "BATT:", "%0.1f V", 10, 9, 15, 16);

	//next col
	tpos = 20;

	//Ignition Advance
	//advance = scalar, S16,    8, "deg", 0.100, 0.0
	float ADV = msCAN_S16(msCAN_Data[1].S16[0]) * 0.1f;
	textDataf(400, tpos+=FONTHEIGHT, ADV, "ADV:", "%0.1f deg", -50,-50,50,50);

	//AFR Target
	//afrtgt1 = scalar, U08, 12, "AFR", 0.1, 0.0
	float AFRT = msCAN_Data[1].U08[3] * 0.1f;
	textDataf(400, tpos+=FONTHEIGHT, AFRT, "AFRT:", "%0.1f AFR", -50,-50,50,50);

	//EGO Correction %
	//egoCorrection1   = scalar, S16,   34, "%",   0.1000, 0.0
	float EGOCOR = msCAN_S16(msCAN_Data[4].S16[1]) * 0.1f;
	textDataf(400, tpos+=FONTHEIGHT, EGOCOR, "COR:", "%0.1f %%", 75,85,115,125);

	//Baro gauge
	//barometer = scalar, S16,   16, "kPa", 0.100, 0.0
	float BARO = msCAN_S16(msCAN_Data[2].S16[0]) * 0.1f;
	textDataf(400, tpos+=FONTHEIGHT, BARO, "BARO:", "%0.1f kpa", 5,5,240,260);

	//Boost Duty 1
	//boostduty = scalar, U08,  139 , "%",  1.0, 0.0
	//2,2,1,1,2
	float BOOSTDTY = msCAN_S16(msCAN_Data[17].U08[4]) * 0.392f;
	textDataf(400, tpos+=FONTHEIGHT, BOOSTDTY, "BST:", "%0.1f %%",-10,-10,100,100);

	//Idle PWM %
	//idleDC = scalar, S16,   54, "%",    0.392, 0.0
	float IDLE = msCAN_S16(msCAN_Data[6].S16[3]) * 0.392f;
	textDataf(400, tpos+=FONTHEIGHT, IDLE, "IDL:", "%0.1f %%",-10,-10,100,100);

	//VSS1
	//vss1_ms_1  = scalar, U16,  336, "ms-1", 0.1, 0.0
	float VSS1 = msCAN_U16(msCAN_Data[42].U16[0]) * 0.1f;
	//convert meters/sec to kilometers/hour
	VSS1 *= 3.60f;
	textDataf(400, tpos+=FONTHEIGHT, VSS1, "VSS:", "%0.1f kph", 0.1,-10, 150, 200);

	//Ethanol %
	//fuel_pct = scalar, U16,  376,  "%",  0.1000, 0.0
	float ETH = msCAN_U16(msCAN_Data[47].U16[0]) * 0.1f;
	textDataf(400, tpos+=FONTHEIGHT, ETH, "ETH:", "%0.1f %%", 75,-10, 90, 100);

	//MS LoopTime
	//looptime = scalar, U16,   424, "us", 1.0, 0.0
	float LOOPT = msCAN_U16(msCAN_Data[55].U16[0]) * 0.001f;
	textDataf(400, tpos+=FONTHEIGHT, LOOPT, "MSLT:", "%0.3f mS", 0,0, 2,3);



}
