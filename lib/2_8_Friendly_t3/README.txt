ST77889 TFT Display Driver
  Requires Adafruit GFX Driver (Tested with version 1.3.6)
  Optimized for Teensy 3.x MCUs
  Uses fast SPI IO when used with Arduino MCUs (__AVR__)
  Works with the Adafruit 1.54" 240x240 TFT IPS SPI Display

Modified for the ST7789 by Brian L. Clevenger 2019-02-22

Based on the Teensy 3.x optimized ST7735 display driver 
  version 1.0.0 by Paul Stoffregen.

Originally written by Limor Fried for Adafruit Industries 
  and released under the MIT distribution license.

This ST7789_t3 is a modified copy of Paul Stoffregen's Teensy 3 
optimized ST7735_t3 library for ST7735 displays specifically modified 
for the ST7789 and Adafruit's 1.54" 240x240 IPS TFT Display.  All
support for the ST7735 has been removed and only Aduino __AVR__ 
and Teensy 3.x support is implemented making the code more 
straightforward.  Teensy LC support is not implemented for clarity 
but would be easy to implement in the driver model.

The ST7735_t3 library from Paul Stoffregen is a copy of 
Adafruit's 1.8" & 1.44" ST7735 library optimized for use 
with Teensy 3.x boards.

----------------------------

This is a library for the Adafruit 1.8" SPI display.

This library works with the Adafruit 1.8" TFT Breakout w/SD card
  ----> http://www.adafruit.com/products/358
The 1.8" TFT shield
  ----> https://www.adafruit.com/product/802
The 1.44" TFT breakout
  ----> https://www.adafruit.com/product/2088
as well as Adafruit raw 1.8" TFT display
  ----> http://www.adafruit.com/products/618
 
Check out the links above for our tutorials and wiring diagrams.
These displays use SPI to communicate, 4 or 5 pins are required
to interface (RST is optional).
Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Limor Fried/Ladyada for Adafruit Industries.
MIT license, all text above must be included in any redistribution

To download. click the DOWNLOADS button in the top right corner, rename the uncompressed folder Adafruit_ST7735. Check that the Adafruit_ST7735 folder contains Adafruit_ST7735.cpp and Adafruit_ST7735.

Place the Adafruit_ST7735 library folder your <arduinosketchfolder>/libraries/ folder. You may need to create the libraries subfolder if its your first library. Restart the IDE

Also requires the Adafruit_GFX library for Arduino.
