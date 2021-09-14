/*THR30II Pedal using Teensy 3.6
* Martin Zwerschke 04/2021
*
*
* TFT_Help.h
* last modified 09/2021
* Author: Martin Zwerschke
*/

#ifndef __TFT_HELP_
#define __TFT_HELP_

#include <2_8_Friendly_t3.h>

extern int16_t xx1, yy1;
extern uint16_t w, h;

void printAt(ST7789_t3 & tft,const int x, const int y, String s);

uint16_t getStringWidth(ST7789_t3 & tft, const String s,const int x=0, const int y=0);


#endif