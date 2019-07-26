/* ===========================================================================

 5-inch MEGASQUIRT ECU CANBus Dashboard Display
 Project Configuration

 Mark Williams (2019-07-01)
 Distributed under the GNU GENERAL PUBLIC LICENSE v3.0 (see LICENCE file)

=========================================================================== */
#ifndef __PROJCFG_H
#define __PROJCFG_H

//PROJECT CONFIGURATION
#include <Arduino.h>

//project options
#define NODEMCU_ESP32S
#define CFAF800480E0_050SC_A1_1

//led bar
#define LEDBAR_LEDNUM		(16)
#define LEDBAR_PIN			(32)

#define EVE2_PIN_INT		(4)
#define EVE2_PIN_PD			(0)
#define EVE2_PIN_CS			(5)
#define SPI_SPEED       	(20000000L) /* FT81x up to 30Mhz */
#define EVE2_WORKBUFSIZE	(32)

//display options
#ifdef CFAF800480E0_050SC_A1_1
#define FT813_ENABLE	(1)
#define EVE2_GT911		(1)

// Horizontal timing (minimum values from ILI6122_SPEC_V008.pdf page 45)
#define HPX   (800)    // Horizontal Pixel Width
#define HSW   (1)      // Horizontal Sync Width (1~40)
#define HBP   (46-HSW) // Horizontal Back Porch (must be 46, includes HSW)
#define HFP   (16)     // Horizontal Front Porch (16~210~354)
#define HPP   (116)    // Horizontal Pixel Padding (tot=863: 862~1056~1200)
                       // FTDI needs at least 1 here
// Vertical timing (minimum values from ILI6122_SPEC_V008.pdf page 46)
#define VLH   (480)   // Vertical Line Height
#define VS    (1)     // Vertical Sync (in lines)  (1~20)
#define VBP   (23-VS) // Vertical Back Porch (must be 23, includes VS)
#define VFP   (7)     // Vertical Front Porch (7~22~147)
#define VLP   (1)     // Vertical Line Padding (tot=511: 510~525~650)
                      // FTDI needs at least 1 here

#define EVE2_DWIDTH		(HPX)
#define EVE2_DHEIGHT	(VLH)
#define EVE2_HCYCLE		(HPX+HFP+HSW+HBP+HPP)
#define EVE2_HOFFSET	(HFP+HSW+HBP)
#define EVE2_HSYNC0		(HFP)
#define EVE2_HSYNC1		(HFP+HSW)
#define EVE2_VCYCLE		(VLH+VFP+VS+VBP+VLP)
#define EVE2_VOFFSET	(VFP+VS+VBP)
#define EVE2_VSYNC0		(VFP)
#define EVE2_VSYNC1		(VFP+VS)
#define EVE2_PCLK		2L
#define EVE2_SWIZZLE	0L
#define EVE2_PCLK_POL	1L
#define EVE2_CSPREAD	0L
#define EVE2_DITHER		1L
#endif

//extra
#define DPRINT_SIZE 	(64)
extern char _debugbuf[DPRINT_SIZE];
#define DPRINTF(...) {sprintf(_debugbuf,__VA_ARGS__); Serial.print(_debugbuf);}
#define DPRINT(...) {Serial.print(__VA_ARGS__);}

#endif
