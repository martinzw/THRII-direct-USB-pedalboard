/***************************************************
  2_8_Friendly_t3.h
  TFT Display Driver for Teensy3.6  
  
  Works with 
  Matrix 2.8_SPI_Key_TFT (hardware v1.1.1706) FRIENDLY Elec
  www.friendlyarm.com
  240x320 TFT-Display for Raspberry Pi
  
  Adapted for use with Teensy 3.6 
  by Martin Zwerschke 2020-12-08
  
  This is a modified copy of:
  
  ST77889 TFT Display Driver - ST7789_t3.h
    Requires Adafruit GFX Driver (Tested with version 1.3.6)
    Optimized for Teensy 3.x MCUs
    Uses fast SPI IO when used with Arduino MCUs (__AVR__)
    Works with the Adafruit 1.54" 240x240 TFT IPS SPI Display

  Modified for the ST7789 by Brian L. Clevenger 2019-02-22

  Based on the Teensy 3.x optimized ST7735 display driver
    version 1.0.0 by Paul Stoffregen.

  Originally written by Limor Fried for Adafruit Industries
    and released under the MIT distribution license.

 ****************************************************/

#ifndef __ST7789_t3_H_
#define __ST7789_t3_H_

#include "Arduino.h"
#include <Adafruit_GFX.h>

#define ST7789_TFTWIDTH   240
#define ST7789_TFTHEIGHT  320

#define ST7789_XSTART 0
#define ST7789_YSTART 0

#define ST_CMD_DELAY  0x80  // Delay signifier for command lists

// ST Orientiation bit masks
#define ST_MADCTL_MY  0x80  // 1 = Rows are bottom to top
#define ST_MADCTL_MX  0x40  // 1 = Columns are right to left
#define ST_MADCTL_MV  0x20  // 1 = Column, Row order (reverse)
#define ST_MADCTL_ML  0x10  // 1 = Refresh LCD bottom to top
#define ST_MADCTL_BGR 0x08  // 1 = Color format is BGR
#define ST_MADCTL_MH  0x04  // 1 = Refresh LCD right to left
#define ST_MADCTL_RGB 0x00  // 0 = Color format is RGB (base mask)

#define ST7789_NOP     0x00
#define ST7789_SWRESET 0x01
#define ST7789_RDDID   0x04
#define ST7789_RDDST   0x09

#define ST7789_SLPIN   0x10
#define ST7789_SLPOUT  0x11
#define ST7789_PTLON   0x12
#define ST7789_NORON   0x13

#define ST7789_INVOFF  0x20
#define ST7789_INVON   0x21
#define ST7789_DISPOFF 0x28
#define ST7789_DISPON  0x29
#define ST7789_CASET   0x2A
#define ST7789_RASET   0x2B
#define ST7789_RAMWR   0x2C
#define ST7789_RAMRD   0x2E

#define ST7789_PTLAR   0x30
#define ST7789_COLMOD  0x3A
#define ST7789_MADCTL  0x36

#define ST7789_RDID1   0xDA
#define ST7789_RDID2   0xDB
#define ST7789_RDID3   0xDC

// Color definitions
#define	ST7789_BLACK   0x0000
#define	ST7789_BLUE    0x001F
#define	ST7789_RED     0xF800
#define	ST7789_GREEN   0x07E0
#define ST7789_CYAN    0x07FF
#define ST7789_MAGENTA 0xF81F
#define ST7789_YELLOW  0xFFE0
#define ST7789_WHITE   0xFFFF


class ST7789_t3 : public Adafruit_GFX {

 public:

  ST7789_t3(uint8_t CS, uint8_t RS, uint8_t SID, uint8_t SCLK, uint8_t RST = -1);
  ST7789_t3(uint8_t CS, uint8_t RS, uint8_t RST = -1);

  void     init(uint16_t width, uint16_t height),
           setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1),
           pushColor(uint16_t color),
           fillScreen(uint16_t color),
           drawPixel(int16_t x, int16_t y, uint16_t color),
           drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color),
           drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color),
           fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color),
           setRotation(uint8_t r),
           invertDisplay(boolean i);

  //inline uint16_t Color565(uint8_t r, uint8_t g, uint8_t b) {return ((b & 0xF8) << 8) | ((g & 0xFC) << 3) | (r >> 3);}
  inline uint16_t Color565(uint8_t r, uint8_t g, uint8_t b) {return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);}
  void setBitrate(uint32_t n);


 private:
  void     writebegin(),
           spiwrite(uint8_t),
           writecommand(uint8_t c),
           writedata(uint8_t d),
           writedata16(uint16_t d),
           commandList(const uint8_t *addr),
           commonInit(const uint8_t *cmdList);


  boolean  hwSPI;

  #if defined(__AVR__)
    volatile uint8_t *dataport, *clkport, *csport, *rsport;
    uint8_t  _cs, _rs, _rst, _sid, _sclk,
           datapinmask, clkpinmask, cspinmask, rspinmask,
           colstart, rowstart; // some displays need this changed
  #endif //  #ifdef __AVR__

//    MK20DX128 = Teensy3.0    MK20DX256 = Teensy3.1&3.2  MK64FX512 = Teensy3.5     MK66FX1M0 = Teensy3.6
  #if defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__)
    uint8_t  _cs, _rs, _rst, _sid, _sclk;
    uint8_t colstart, rowstart;
    uint8_t pcs_data, pcs_command;
    uint32_t ctar;
    volatile uint8_t *datapin, *clkpin, *cspin, *rspin;
  #endif

};

#endif
