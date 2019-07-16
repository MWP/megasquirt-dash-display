
# 5-inch MEGASQUIRT ECU CANBus Dashboard Display

*proof of concept*

 This project in current form "works", but is rough around the edges.  
 It needs a nicer method of screen configuration, and a lot of optimisation.

 This project is based around:
 - NodeMCU-32s ESP32 (WROOM-32) module  
https://www.amazon.com/s?k=nodemcu32s
 - Crystalfontz 800x480 TFT with Graphic Accelerator Board  
https://www.crystalfontz.com/product/cfaf800480e0050sca11
 - Crystalfontz CFA10098 breakout board  
https://www.crystalfontz.com/product/cfa10098
 - WS2812b LEDs (aka Neopixels)  
https://www.amazon.com/s?k=neopixel+8+stick
 - Any MC2551 CAN bus Tranciever Module setup for 5V  
https://www.amazon.com/s?k=mcp2551+module
 - Any 5V 2-amp (or higher) regulator suitable for automotive use

 I programmed this in the PlatformIO development environment.
 The Arduino IDE could be used with some minor modifications.
 PlatformIO (or Arduino IDE) needs to have the NodeMCU-32s board support (using Arduino framework)
 installed.

 Mark Williams (2019-07-01)  
 Distributed under the Attribution-NonCommercial-ShareAlike 4.0 International licence (see LICENCE file)

 Other source-code / libraries taken from various sources with appropriate licences.  
 See licence information contained within those files.
