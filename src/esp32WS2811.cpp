/*

Copyright 2017 Neil Kolban
The WS2811 driver is taken from https://github.com/nkolban/esp32-snippets

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

#include "esp32WS2811.h"

void setItem1(rmt_item32_t* item) {
  item->level0    = 1;
  item->duration0 = 10;
  item->level1    = 0;
  item->duration1 = 6;
}

void setItem0(rmt_item32_t* item) {
  item->level0    = 1;
  item->duration0 = 4;
  item->level1    = 0;
  item->duration1 = 8;
}

void setTerminator(rmt_item32_t* item) {
  item->level0    = 0;
  item->duration0 = 50;
  item->level1    = 0;
  item->duration1 = 0;
}

WS2811::WS2811(int dataPin, size_t numLeds, int channel) :
  _rmtTask(nullptr),
  _smphr(nullptr),
  _channel(static_cast<rmt_channel_t>(channel)),
  _dataPin(dataPin),
  _numLeds(numLeds),
  _leds(nullptr),
  _brightness(255)
  {
    _leds = new Led[_numLeds];
  }

WS2811::~WS2811() {
  xSemaphoreTake(_smphr, portMAX_DELAY);
    vTaskDelete(_rmtTask);
  vSemaphoreDelete(_smphr);
  delete[] _leds;
}

void WS2811::begin() {
  _setupRMT();
  _smphr = xSemaphoreCreateBinary();
  xSemaphoreGive(_smphr);  // semaphore is created 'empty', so has to be given for first use
  xTaskCreate((TaskFunction_t)&_handleRmt, "rmtTask", 2000, this, 5, &_rmtTask);
}

void WS2811::show() {
  xTaskNotifyGive(_rmtTask);
}

void WS2811::setPixel(size_t index, uint8_t red, uint8_t green, uint8_t blue) {
  if (xSemaphoreTake(_smphr, 100) == pdTRUE) {
    _leds[index].red   = red * _brightness / 100;
    _leds[index].green = green * _brightness / 100;
    _leds[index].blue  = blue * _brightness / 100;
    xSemaphoreGive(_smphr);
  } else {
    log_e("could not set pixel");
  }
}

void WS2811::clearAll() {
  for (size_t i = 0; i < _numLeds; ++i) {
    setPixel(i, 0, 0, 0);
  }
}

void WS2811::setAll(uint8_t red, uint8_t green, uint8_t blue) {
  for (size_t i = 0; i < _numLeds; ++i) {
    setPixel(i, red, green, blue);
  }
}

void WS2811::setBrightness(uint8_t brightness, bool reset) {
  _brightness = brightness;
  if (_brightness > 100) _brightness = 100;
  if (reset) {
    for (size_t i = 0; i < _numLeds; ++i) {
      setPixel(i, _leds[i].red, _leds[i].green, _leds[i].blue);
    }
  }
}

void WS2811::_setupRMT() {
  static rmt_config_t config;
  config.rmt_mode                  = RMT_MODE_TX;
  config.channel                   = _channel;
  config.gpio_num                  = static_cast<gpio_num_t>(_dataPin);
  config.mem_block_num             = 8 - _channel;
  config.clk_div                   = 8;
  config.tx_config.loop_en         = 0;
  config.tx_config.carrier_en      = 0;
  config.tx_config.idle_output_en  = 1;
  config.tx_config.idle_level      = (rmt_idle_level_t)0;
  config.tx_config.carrier_freq_hz = 10000;
  config.tx_config.carrier_level   = (rmt_carrier_level_t)1;
  config.tx_config.carrier_duty_percent = 50;
  rmt_config(&config);
  rmt_driver_install(_channel, 0, 0);
}

void WS2811::_handleRmt(WS2811* ws2811) {
  static rmt_item32_t* rmtItems = new rmt_item32_t[ws2811->_numLeds * 24 + 1];
  static rmt_item32_t* currentItem = &rmtItems[0];

  while (true) {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    currentItem = &rmtItems[0];
    if (xSemaphoreTake(ws2811->_smphr, 100) == pdTRUE) {
      for (size_t i = 0; i < ws2811->_numLeds; ++i) {
        uint32_t currentPixel =
            ws2811->_leds[i].green << 16 |
            ws2811->_leds[i].red << 8 |
            ws2811->_leds[i].blue;
        for (int8_t j = 23; j >= 0; --j) {
          // We have 24 bits of data representing the red, green and blue channels. The value of the
          // 24 bits to output is in the variable current_pixel.  We now need to stream this value
          // through RMT in most significant bit first.  To do this, we iterate through each of the 24
          // bits from MSB to LSB.
          if (currentPixel & (1 << j)) {
            setItem1(currentItem);
          } else {
            setItem0(currentItem);
          }
          ++currentItem;
        }
      }
      setTerminator(currentItem);  // Write the RMT terminator.
      ESP_ERROR_CHECK(rmt_write_items(ws2811->_channel, rmtItems, ws2811->_numLeds * 24, 1 /* wait till done */));
      xSemaphoreGive(ws2811->_smphr);
    } else {
      log_e("could not write RMT data");
    }
  }
}
