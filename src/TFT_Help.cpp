/*THR30II Pedal using Teensy 3.6
* Martin Zwerschke 04/2021
*
* TFT_Help.cpp
* last modified 09/2021
* Author: Martin Zwerschke
*/

#include <2_8_Friendly_t3.h>
#include "TFT_Help.h"

int16_t xx1=0, yy1=0;
uint16_t w=0, h=0;

void printAt(ST7789_t3 & tft,const int x, const int y, String s)
{
  tft.getTextBounds("|",x,y,&xx1,&yy1,&w,&h);  //use the highest char for getting the "FontHeight"
  tft.setCursor(x,y+h);
  tft.print(s);
} 

uint16_t getStringWidth(ST7789_t3 & tft, const String s,const int x, const int y)
{
   tft.getTextBounds(s,x,y,&xx1,&yy1,&w,&h);
   uint16_t w1=w;
   tft.getTextBounds("|",x,y,&xx1,&yy1,&w,&h);  //use highest char to determine h=Font Height 
   w=w1;
   return w;
}