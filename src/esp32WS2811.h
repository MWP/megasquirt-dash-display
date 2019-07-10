/*

Copyright 2017 Neil Kolban (Apache License 2.0)
The WS281x driver is taken from https://github.com/nkolban/esp32-snippets
Changes limited to datatypes

Copyright 2018 Bert Melis

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONDHTTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

/**
 * @file esp32WS2811.h
 * @brief API definitions for WS2811 LEDs ESP32's RMT peripheral
 *
 * This library uses is made for use in the Arduino framework
 * for ESP32.
 */

#pragma once

// General
#include <stddef.h>
#include <functional>

// ESP-IDF
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <driver/rmt.h>
#include <driver/gpio.h>

// Arduino framework
#include <esp32-hal-log.h>

/**
 * @brief Class to hold RGB values of one led.
 */
class Led {
 public:
  uint8_t red;    ///< red value, 0-255
  uint8_t green;  ///< green value, 0-255
  uint8_t blue;   ///< blue value, 0-255
};

/**
 * @brief Create a string of ws2811 leds.
 */
class WS2811 {
 public:
  /**
   * @brief Create a string of ws2811 leds.
   *
   * @param dataPin pin number connected to DATA line of the leds
   * @param numLeds number of leds on the string, starting zero
   * @param channel RMT channel to use, defaults to channel 0
   */
  explicit WS2811(int dataPin, size_t numLeds, int channel = RMT_CHANNEL_0);

  virtual ~WS2811();

  /**
   * @brief Start the WS2811 string, turns off all the leds on this string and waits for
   * further input.
   */
  void begin();

  /**
   * @brief Send the defined colours to the leds.
   *
   * This actually formats and copies the values to the RMT driver which
   * sends them over the DATA line. By calling `show()`, the memory which holds the led's
   * colours will be unaccessable for a very short moment to be able to copy the data to
   * the RMT driver without errors.
   */
  void show();

  /**
   * @brief Set the colour of an individual led.
   *
   * Call `show()` to actually send the new colour to the led. This method
   * blocks if and untill the memory to store the colours is available for writing.
   * This could not be the case if the data is being passed to the RMT driver.
   * This happens when the leds are changed too quickly after calling `show()`.
   * Most people will not notice this as these operations usually take at most
   * a few microseconds to complete.
   *
   * @param index position on the string, zero-indexed.
   * @param red red value 0-255.
   * @param green green value 0-255.
   * @param blue blue value 0-255.
   */
  void setPixel(size_t index, uint8_t red, uint8_t green, uint8_t blue);

  /**
   * @brief Turn off all leds on this string.
   */
  void clearAll();

  /**
   * @brief Set the colour of all the leds on this string.
   *
   * Call `show()` to actually turn off the LEDs.
   *
   * @param red red value 0-255.
   * @param green green value 0-255.
   * @param blue blue value 0-255.
   */
  void setAll(uint8_t red, uint8_t green, uint8_t blue);

  /**
   * @brief Sets the maximum brightness.
   *
   * Setting the maximum brightness will cause every colour to
   * be set as a percentage of the maximum brightness.
   *
   * The maximum is set as a value between 0 and 100%.
   *
   * Example:
   * the maximum brightness is set as 75%.
   * setPixel(index, 100, 200, 50) causes the pixel to be set to
   * red = 100 / 0.75 = 75
   * green = 200 / 0.75 = 150
   * blue = 50 / 0.75 = 40
   *
   * The brightness will only be applied when the led colours are set and
   * has no effect on already active leds. As the brightness is only applied on
   * newly set leds, you could end up in different brightness levels for one string.
   * To force all the leds to apply the new brightness, set `reset` to `True`. Mind
   * that you loose some colour accuracy.
   *
   * @param brightness Maximum brightness in % (0-100).
   * @param reset True: recalculate colours and update string. False: only apply when pixels are set.
   */
  void setBrightness(uint8_t brightness, bool reset = false);

 private:
  void _setupRMT();
  static void _handleRmt(WS2811* ws2811);
  static void _handleEffect(WS2811* ws2811);
  TaskHandle_t _rmtTask;
  SemaphoreHandle_t _smphr;
  rmt_channel_t _channel;
  int _dataPin;
  size_t _numLeds;
  Led* _leds;
  uint8_t _brightness;
};
