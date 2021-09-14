/***************************************************
  2_8_Friendly_t3.cpp
  TFT Display Driver for Teensy3.6  
  
  Works with 
  Matrix 2.8_SPI_Key_TFT (hardware v1.1.1706) FRIENDLY Elec
  www.friendlyarm.com
  240x320 TFT-Display for Raspberry Pi
  
  Adapted for use with Teensy 3.6 
  by Martin Zwerschke 2020-12-08
  
  This is a modified copy of:
  ST77889 TFT Display Driver - ST7789_t3.cpp
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

#include "2_8_Friendly_t3.h"
#include <limits.h>
#include "pins_arduino.h"
#include "wiring_private.h"
#include <SPI.h>


// Constructor when using software SPI.  All output pins are configurable.
ST7789_t3::ST7789_t3(uint8_t cs, uint8_t rs, uint8_t sid, uint8_t sclk, uint8_t rst) :
 Adafruit_GFX(ST7789_TFTWIDTH, ST7789_TFTHEIGHT)
{
	_cs   = cs;
	_rs   = rs;
	_sid  = sid;
	_sclk = sclk;
	_rst  = rst;
	hwSPI = false;
}


// Constructor when using hardware SPI.  Faster, but must use SPI pins
// specific to each board type (e.g. 11,13 for Uno, 51,52 for Mega, etc.)
ST7789_t3::ST7789_t3(uint8_t cs, uint8_t rs, uint8_t rst) :
  Adafruit_GFX(ST7789_TFTWIDTH, ST7789_TFTHEIGHT) {
	_cs   = cs;
	_rs   = rs;
	_rst  = rst;
	hwSPI = true;
	_sid  = _sclk = (uint8_t)-1;
}


/***************************************************************/
/*     Arduino Uno, Leonardo, Mega, Teensy 2.0, etc            */
/***************************************************************/
#if defined(__AVR__ )
inline void ST7789_t3::writebegin()
{
}

inline void ST7789_t3::spiwrite(uint8_t c)
{
	if (hwSPI) {
		SPDR = c;
		while(!(SPSR & _BV(SPIF)));
	} else {
		// Fast SPI bitbang swiped from LPD8806 library
		for(uint8_t bit = 0x80; bit; bit >>= 1) {
			if(c & bit) *dataport |=  datapinmask;
			else        *dataport &= ~datapinmask;
			*clkport |=  clkpinmask;
			*clkport &= ~clkpinmask;
		}
	}
}

void ST7789_t3::writecommand(uint8_t c)
{
	*rsport &= ~rspinmask;
	*csport &= ~cspinmask;
	spiwrite(c);
	*csport |= cspinmask;
}

void ST7789_t3::writedata(uint8_t c)
{
	*rsport |=  rspinmask;
	*csport &= ~cspinmask;
	spiwrite(c);
	*csport |= cspinmask;
}

void ST7789_t3::writedata16(uint16_t d)
{
	*rsport |=  rspinmask;
	*csport &= ~cspinmask;
	spiwrite(d >> 8);
	spiwrite(d);
	*csport |= cspinmask;
}

void ST7789_t3::setBitrate(uint32_t n)
{
	if (n >= 8000000) {
		SPI.setClockDivider(SPI_CLOCK_DIV2);
	} else if (n >= 4000000) {
		SPI.setClockDivider(SPI_CLOCK_DIV4);
	} else if (n >= 2000000) {
		SPI.setClockDivider(SPI_CLOCK_DIV8);
	} else {
		SPI.setClockDivider(SPI_CLOCK_DIV16);
	}
}

/***************************************************************/
/*     Teensy 3.0, 3.1, 3.2, 3.5, 3.6                          */
/***************************************************************/
#elif defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__)

inline void ST7789_t3::writebegin()
{
}

inline void ST7789_t3::spiwrite(uint8_t c)
{
	for (uint8_t bit = 0x80; bit; bit >>= 1) {
		*datapin = ((c & bit) ? 1 : 0);
		*clkpin = 1;
		*clkpin = 0;
	}
}

void ST7789_t3::writecommand(uint8_t c)
{
	if (hwSPI) {
		KINETISK_SPI0.PUSHR = c | (pcs_command << 16) | SPI_PUSHR_CTAS(0);
		while (((KINETISK_SPI0.SR) & (15 << 12)) > (3 << 12)) ; // wait if FIFO full
	} else {
		*rspin = 0;
		*cspin = 0;
		spiwrite(c);
		*cspin = 1;
	}
}

void ST7789_t3::writedata(uint8_t c)
{
	if (hwSPI) {
		KINETISK_SPI0.PUSHR = c | (pcs_data << 16) | SPI_PUSHR_CTAS(0);
		while (((KINETISK_SPI0.SR) & (15 << 12)) > (3 << 12)) ; // wait if FIFO full
	} else {
		*rspin = 1;
		*cspin = 0;
		spiwrite(c);
		*cspin = 1;
	}
}

void ST7789_t3::writedata16(uint16_t d)
{
	if (hwSPI) {
		KINETISK_SPI0.PUSHR = d | (pcs_data << 16) | SPI_PUSHR_CTAS(1);
		while (((KINETISK_SPI0.SR) & (15 << 12)) > (3 << 12)) ; // wait if FIFO full
	} else {
		*rspin = 1;
		*cspin = 0;
		spiwrite(d >> 8);
		spiwrite(d);
		*cspin = 1;
	}
}

static bool spi_pin_is_cs(uint8_t pin)
{
	if (pin == 2 || pin == 6 || pin == 9) return true;
	if (pin == 10 || pin == 15) return true;
	if (pin >= 20 && pin <= 23) return true;
	return false;
}

static uint8_t spi_configure_cs_pin(uint8_t pin)
{
        switch (pin) {
                case 10: CORE_PIN10_CONFIG = PORT_PCR_MUX(2); return 0x01; // PTC4
                case 2:  CORE_PIN2_CONFIG  = PORT_PCR_MUX(2); return 0x01; // PTD0
                case 9:  CORE_PIN9_CONFIG  = PORT_PCR_MUX(2); return 0x02; // PTC3
                case 6:  CORE_PIN6_CONFIG  = PORT_PCR_MUX(2); return 0x02; // PTD4
                case 20: CORE_PIN20_CONFIG = PORT_PCR_MUX(2); return 0x04; // PTD5
                case 23: CORE_PIN23_CONFIG = PORT_PCR_MUX(2); return 0x04; // PTC2
                case 21: CORE_PIN21_CONFIG = PORT_PCR_MUX(2); return 0x08; // PTD6
                case 22: CORE_PIN22_CONFIG = PORT_PCR_MUX(2); return 0x08; // PTC1
                case 15: CORE_PIN15_CONFIG = PORT_PCR_MUX(2); return 0x10; // PTC0
        }
        return 0;
}

#define CTAR_24MHz   (SPI_CTAR_PBR(0) | SPI_CTAR_BR(0) | SPI_CTAR_CSSCK(0) | SPI_CTAR_DBR)
#define CTAR_16MHz   (SPI_CTAR_PBR(1) | SPI_CTAR_BR(0) | SPI_CTAR_CSSCK(0) | SPI_CTAR_DBR)
#define CTAR_12MHz   (SPI_CTAR_PBR(0) | SPI_CTAR_BR(0) | SPI_CTAR_CSSCK(0))
#define CTAR_8MHz    (SPI_CTAR_PBR(1) | SPI_CTAR_BR(0) | SPI_CTAR_CSSCK(0))
#define CTAR_6MHz    (SPI_CTAR_PBR(0) | SPI_CTAR_BR(1) | SPI_CTAR_CSSCK(1))
#define CTAR_4MHz    (SPI_CTAR_PBR(1) | SPI_CTAR_BR(1) | SPI_CTAR_CSSCK(1))

void ST7789_t3::setBitrate(uint32_t n)
{
	if (n >= 24000000) {
		ctar = CTAR_24MHz;
	} else if (n >= 16000000) {
		ctar = CTAR_16MHz;
	} else if (n >= 12000000) {
		ctar = CTAR_12MHz;
	} else if (n >= 8000000) {
		ctar = CTAR_8MHz;
	} else if (n >= 6000000) {
		ctar = CTAR_6MHz;
	} else {
		ctar = CTAR_4MHz;
	}
	SIM_SCGC6 |= SIM_SCGC6_SPI0;
	KINETISK_SPI0.MCR = SPI_MCR_MDIS | SPI_MCR_HALT;
	KINETISK_SPI0.CTAR0 = ctar | SPI_CTAR_FMSZ(7);
	KINETISK_SPI0.CTAR1 = ctar | SPI_CTAR_FMSZ(15);
	KINETISK_SPI0.MCR = SPI_MCR_MSTR | SPI_MCR_PCSIS(0x1F) | SPI_MCR_CLR_TXF | SPI_MCR_CLR_RXF;
}

#endif


// ST7789 initialization commands and arguments table stored in PROGMEM.
static const uint8_t PROGMEM
    cmd_240x240[] = {                 		// Initialization commands for 7789 screens
      10,                       				  // 9 commands in list:
      ST7789_SWRESET,   ST_CMD_DELAY,  		// 1: Software reset, no args, w/delay
        150,                     				  //   150 ms delay
      ST7789_SLPOUT ,   ST_CMD_DELAY,  		// 2: Out of sleep mode, no args, w/delay
        255,                    				  //   255 = 500 ms delay
      ST7789_COLMOD , 1+ST_CMD_DELAY,  		// 3: Set color mode, 1 arg + delay:
        0x55,                   				  //   16-bit color
        10,                     				  //   10 ms delay
      ST7789_MADCTL , 1,  					      // 4: Memory access ctrl (directions), 1 arg:
        0x00,                   				  //   Row addr/col addr, bottom to top refresh
      ST7789_CASET  , 4,  					      // 5: Column addr set, 4 args, no delay:
        0x00, ST7789_XSTART,              //   XSTART = 0
  	  (240+ST7789_XSTART) >> 8,
  	  (240+ST7789_XSTART) & 0xFF,         //   XEND = 240
      ST7789_RASET  , 4,  					      // 6: Row addr set, 4 args, no delay:
        0x00, ST7789_YSTART,              //   YSTART = 0
        (240+ST7789_YSTART) >> 8,
  	  (240+ST7789_YSTART) & 0xFF,	        //   YEND = 240
      ST7789_INVON ,   ST_CMD_DELAY,  		// 7: Inversion ON
        10,
      ST7789_NORON  ,   ST_CMD_DELAY,  		// 8: Normal display on, no args, w/delay
        10,                     				  //   10 ms delay
      ST7789_DISPON ,   ST_CMD_DELAY,  		// 9: Main screen turn on, no args, w/delay
    255 };

static const uint8_t PROGMEM
    cmd_240x320[] = {                 		// Initialization commands for 7789 screens
      10,                       				  // 9 commands in list:
      ST7789_SWRESET,   ST_CMD_DELAY,  		// 1: Software reset, no args, w/delay
        150,                     				  //   150 ms delay
      ST7789_SLPOUT ,   ST_CMD_DELAY,  		// 2: Out of sleep mode, no args, w/delay
        255,                    				  //   255 = 500 ms delay
      ST7789_COLMOD , 1+ST_CMD_DELAY,  		// 3: Set color mode, 1 arg + delay:
        0x05,                   				  //   16-bit color
        10,                     				  //   10 ms delay
      ST7789_MADCTL , 1,  					      // 4: Memory access ctrl (directions), 1 arg:
        0x00,                   				  //   Row addr/col addr, bottom to top refresh
      ST7789_CASET  , 4,  					      // 5: Column addr set, 4 args, no delay:
        0x00, ST7789_XSTART,              //   XSTART = 0
  	  (240+ST7789_XSTART) >> 8,
  	  (240+ST7789_XSTART) & 0xFF,         //   XEND = 240
      ST7789_RASET  , 4,  					      // 6: Row addr set, 4 args, no delay:
        0x00, ST7789_YSTART,              //   YSTART = 0
        (320+ST7789_YSTART) >> 8,
  	  (320+ST7789_YSTART) & 0xFF,	        //   YEND = 320
      ST7789_INVON ,   ST_CMD_DELAY,  		// 7: Inversion ON
        10,
      ST7789_NORON  ,   ST_CMD_DELAY,  		// 8: Normal display on, no args, w/delay
        10,                     				  //   10 ms delay
      ST7789_DISPON ,   ST_CMD_DELAY,  		// 9: Main screen turn on, no args, w/delay
    255 };	

void ST7789_t3::commandList(const uint8_t *addr)
{
	uint8_t  numCommands, numArgs;
	uint16_t ms;

	writebegin();
	numCommands = pgm_read_byte(addr++);		// Number of commands to follow
	while(numCommands--) {				// For each command...
		writecommand(pgm_read_byte(addr++));	//   Read, issue command
		numArgs  = pgm_read_byte(addr++);	//   Number of args to follow
		ms       = numArgs & ST_CMD_DELAY;		//   If hibit set, delay follows args
		numArgs &= ~ST_CMD_DELAY;			//   Mask out delay bit
		while(numArgs--) {			//   For each argument...
			writedata(pgm_read_byte(addr++)); //   Read, issue argument
		}

		if(ms) {
			ms = pgm_read_byte(addr++);	// Read post-command delay time (ms)
			if(ms == 255) ms = 500;		// If 255, delay for 500 ms
			delay(ms);
		}
	}
}


void ST7789_t3::commonInit(const uint8_t *cmdList)
{
	colstart  = rowstart = 0; // May be overridden in init func

#ifdef __AVR__
	pinMode(_rs, OUTPUT);
	pinMode(_cs, OUTPUT);
	csport    = portOutputRegister(digitalPinToPort(_cs));
	rsport    = portOutputRegister(digitalPinToPort(_rs));
	cspinmask = digitalPinToBitMask(_cs);
	rspinmask = digitalPinToBitMask(_rs);

	if(hwSPI) { // Using hardware SPI
		SPI.begin();
		SPI.setClockDivider(SPI_CLOCK_DIV4); // 4 MHz (half speed)
		SPI.setBitOrder(MSBFIRST);
		SPI.setDataMode(SPI_MODE0);
	} else {
		pinMode(_sclk, OUTPUT);
		pinMode(_sid , OUTPUT);
		clkport     = portOutputRegister(digitalPinToPort(_sclk));
		dataport    = portOutputRegister(digitalPinToPort(_sid));
		clkpinmask  = digitalPinToBitMask(_sclk);
		datapinmask = digitalPinToBitMask(_sid);
		*clkport   &= ~clkpinmask;
		*dataport  &= ~datapinmask;
	}
	// toggle RST low to reset; CS low so it'll listen to us
	*csport &= ~cspinmask;

#elif defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__)
	if (_sid == (uint8_t)-1) _sid = 11;
	if (_sclk == (uint8_t)-1) _sclk = 13;
	if ( spi_pin_is_cs(_cs) && spi_pin_is_cs(_rs)
	 && (_sid == 7 || _sid == 11)
	 && (_sclk == 13 || _sclk == 14)
	 && !(_cs ==  2 && _rs == 10) && !(_rs ==  2 && _cs == 10)
	 && !(_cs ==  6 && _rs ==  9) && !(_rs ==  6 && _cs ==  9)
	 && !(_cs == 20 && _rs == 23) && !(_rs == 20 && _cs == 23)
	 && !(_cs == 21 && _rs == 22) && !(_rs == 21 && _cs == 22) ) {
		hwSPI = true;
		if (_sclk == 13) {
			CORE_PIN13_CONFIG = PORT_PCR_MUX(2) | PORT_PCR_DSE;
			SPCR.setSCK(13);
		} else {
			CORE_PIN14_CONFIG = PORT_PCR_MUX(2);
			SPCR.setSCK(14);
		}
		if (_sid == 11) {
			CORE_PIN11_CONFIG = PORT_PCR_MUX(2);
			SPCR.setMOSI(11);
		} else {
			CORE_PIN7_CONFIG = PORT_PCR_MUX(2);
			SPCR.setMOSI(7);
		}
		ctar = CTAR_12MHz;
		pcs_data = spi_configure_cs_pin(_cs);
		pcs_command = pcs_data | spi_configure_cs_pin(_rs);
		SIM_SCGC6 |= SIM_SCGC6_SPI0;
		KINETISK_SPI0.MCR = SPI_MCR_MDIS | SPI_MCR_HALT;
		KINETISK_SPI0.CTAR0 = ctar | SPI_CTAR_FMSZ(7);
		KINETISK_SPI0.CTAR1 = ctar | SPI_CTAR_FMSZ(15);
		KINETISK_SPI0.MCR = SPI_MCR_MSTR | SPI_MCR_PCSIS(0x1F) | SPI_MCR_CLR_TXF | SPI_MCR_CLR_RXF;
	} else {
		hwSPI = false;
		cspin = portOutputRegister(digitalPinToPort(_cs));
		rspin = portOutputRegister(digitalPinToPort(_rs));
		clkpin = portOutputRegister(digitalPinToPort(_sclk));
		datapin = portOutputRegister(digitalPinToPort(_sid));
		*cspin = 1;
		*rspin = 0;
		*clkpin = 0;
		*datapin = 0;
		pinMode(_cs, OUTPUT);
		pinMode(_rs, OUTPUT);
		pinMode(_sclk, OUTPUT);
		pinMode(_sid, OUTPUT);
	}
#endif

	if (_rst) {
		pinMode(_rst, OUTPUT);
		digitalWrite(_rst, HIGH);
		delay(5);
		digitalWrite(_rst, LOW);
		delay(100);
		digitalWrite(_rst, HIGH);
		delay(5);
	}

	if(cmdList) commandList(cmdList);
}



void ST7789_t3::init(uint16_t width, uint16_t height)
{
    colstart = ST7789_XSTART;
    rowstart = ST7789_YSTART;
    _height = height;
    _width = width;
	commonInit(cmd_240x320);
	setRotation(2);
}





void ST7789_t3::setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	writecommand(ST7789_CASET); // Column addr set
	writedata16(x0+colstart);   // XSTART
	writedata16(x1+colstart);   // XEND
	writecommand(ST7789_RASET); // Row addr set
	writedata16(y0+rowstart);   // YSTART
	writedata16(y1+rowstart);   // YEND
	writecommand(ST7789_RAMWR); // write to RAM
}


void ST7789_t3::pushColor(uint16_t color)
{
	writebegin();
	writedata16(color);
}

void ST7789_t3::drawPixel(int16_t x, int16_t y, uint16_t color)
{
	if ((x < 0) ||(x >= _width) || (y < 0) || (y >= _height)) return;
	setAddrWindow(x,y,x+1,y+1);
	writedata16(color);
}


void ST7789_t3::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
	// Rudimentary clipping
	if ((x >= _width) || (y >= _height)) return;
	if ((y+h-1) >= _height) h = _height-y;
	setAddrWindow(x, y, x, y+h-1);
	while (h--) {
		writedata16(color);
	}
}


void ST7789_t3::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
	// Rudimentary clipping
	if ((x >= _width) || (y >= _height)) return;
	if ((x+w-1) >= _width)  w = _width-x;
	setAddrWindow(x, y, x+w-1, y);
	while (w--) {
		writedata16(color);
	}
}



void ST7789_t3::fillScreen(uint16_t color)
{
	fillRect(0, 0,  _width, _height, color);
}



// fill a rectangle
void ST7789_t3::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
	// rudimentary clipping (drawChar w/big text requires this)
	if ((x >= _width) || (y >= _height)) return;
	if ((x + w - 1) >= _width)  w = _width  - x;
	if ((y + h - 1) >= _height) h = _height - y;
	setAddrWindow(x, y, x+w-1, y+h-1);
	for (y=h; y>0; y--) {
		for(x=w; x>0; x--) {
			writedata16(color);
		}
	}
}


void ST7789_t3::setRotation(uint8_t m)
{
	writecommand(ST7789_MADCTL);
	rotation = m % 4; // can't be higher than 3
	switch (rotation) {
	case 0:
		writedata(ST_MADCTL_MX | ST_MADCTL_MY | ST_MADCTL_ML | ST_MADCTL_MH | ST_MADCTL_RGB);
		_width  = ST7789_TFTWIDTH;
   	    _height = ST7789_TFTHEIGHT;
		break;
	case 1:
		writedata(ST_MADCTL_MV | ST_MADCTL_MY | ST_MADCTL_ML | ST_MADCTL_RGB);
		_width = ST7789_TFTHEIGHT;
		_height = ST7789_TFTWIDTH;
		break;
	case 2:
		writedata(ST_MADCTL_RGB);
		_width  = ST7789_TFTWIDTH;
		_height = ST7789_TFTHEIGHT;
		break;
	case 3:
		//writedata(ST_MADCTL_MX | ST_MADCTL_MV | ST_MADCTL_RGB);
		writedata(ST_MADCTL_MX | ST_MADCTL_MV | ST_MADCTL_MH | ST_MADCTL_RGB);
		_width = ST7789_TFTHEIGHT;
		_height = ST7789_TFTWIDTH;
		break;
	}
}


void ST7789_t3::invertDisplay(boolean i)
{
	writecommand(i ? ST7789_INVON : ST7789_INVOFF);
}
