/*THR30II Pedal using Teensy 3.6
* Martin Zwerschke 04/2021
*/

/*
 * THR30II_Pedal.h
 *
 * Last modified: 2. May 2023 
 *  Author: Martin Zwerschke
 *
 */ 

#ifndef _THR30II_PEDAL_H_
#define _THR30II_PEDAL_H_

#include <Arduino.h>
#include <ArduinoQueue.h>
#include "THR30II.h"

void OnSysEx(const uint8_t *data, uint16_t length, bool complete);

//uint16_t dePack(uint8_t *raw, uint8_t *clean, uint16_t received); //not needed on Teensy
void drawStatusMask(uint8_t x, uint8_t y);
void send_patch(uint8_t patch_id);
void drawPatchName(String pana,uint16_t color,uint8_t y);
void send_dump_request();
void solo_deactivate(bool restore);
void solo_activate();
void patch_deactivate();
void patch_activate(uint16_t pnr);
void do_volume_patch();
void undo_volume_patch();
void send_init();         //Try to activate THR30II MIDI 
void WorkingTimer_Tick();

extern String preSelName; //Name of the pre selected patch

enum UIStates { UI_idle, UI_init_act_set, UI_act_vol_sol, UI_patch, UI_ded_sol, UI_pat_vol_sol}; //States for the patch/solo activation user interface

extern UIStates _uistate;

extern ArduinoQueue<Outmessage> outqueue;
//Messages, that have to be sent out,
// and flags to change, if an awaited acknowledge comes in (adressed by their ID)
extern ArduinoQueue <std::tuple<uint16_t, Outmessage, bool *, bool> > on_ack_queue;
extern ArduinoQueue<SysExMessage> inqueue;

// TFT Write Directions
const byte TFT_DIR_BOTTOM_UP =0;
const byte TFT_DIR_LEFT_RIGHT =1;
const byte TFT_DIR_UP_DOWN =2;
const byte TFT_DIR_RIGHT_LEFT =3;

// Color definitions (RGB565)
#define ST7789_ALICEBLUE 0xF7DF 
#define ST7789_ANTIQUEWHITE 0xFF5A 
#define ST7789_AQUA 0x07FF 
#define ST7789_AQUAMARINE 0x7FFA 
#define ST7789_AZURE 0xF7FF 
#define ST7789_BEIGE 0xF7BB 
#define ST7789_BISQUE 0xFF38 
#define ST7789_BLACK 0x0000 
#define ST7789_BLANCHEDALMOND 0xFF59 
#define ST7789_BLUE 0x001F 
#define ST7789_BLUEVIOLET 0x895C 
#define ST7789_BROWN 0xA145 
#define ST7789_BURLYWOOD 0xDDD0 
#define ST7789_CADETBLUE 0x5CF4 
#define ST7789_CHARTREUSE 0x7FE0 
#define ST7789_CHOCOLATE 0xD343 
#define ST7789_CORAL 0xFBEA 
#define ST7789_CORNFLOWERBLUE 0x64BD 
#define ST7789_CORNSILK 0xFFDB 
#define ST7789_CRIMSON 0xD8A7 
#define ST7789_CYAN 0x07FF 
#define ST7789_DARKBLUE 0x0011 
#define ST7789_DARKCYAN 0x0451 
#define ST7789_DARKGOLDENROD 0xBC21 
#define ST7789_DARKGRAY 0xAD55 
#define ST7789_DARKGREEN 0x0320 
#define ST7789_DARKKHAKI 0xBDAD 
#define ST7789_DARKMAGENTA 0x8811 
#define ST7789_DARKOLIVEGREEN 0x5345 
#define ST7789_DARKORANGE 0xFC60 
#define ST7789_DARKORCHID 0x9999 
#define ST7789_DARKRED 0x8800 
#define ST7789_DARKSALMON 0xECAF 
#define ST7789_DARKSEAGREEN 0x8DF1 
#define ST7789_DARKSLATEBLUE 0x49F1 
#define ST7789_DARKSLATEGRAY 0x2A69 
#define ST7789_DARKTURQUOISE 0x067A 
#define ST7789_DARKVIOLET 0x901A 
#define ST7789_DEEPPINK 0xF8B2 
#define ST7789_DEEPSKYBLUE 0x05FF 
#define ST7789_DIMGRAY 0x6B4D 
#define ST7789_DODGERBLUE 0x1C9F 
#define ST7789_FIREBRICK 0xB104 
#define ST7789_FLORALWHITE 0xFFDE 
#define ST7789_FORESTGREEN 0x2444 
#define ST7789_FUCHSIA 0xF81F 
#define ST7789_GAINSBORO 0xDEFB 
#define ST7789_GHOSTWHITE 0xFFDF 
#define ST7789_GOLD 0xFEA0 
#define ST7789_GOLDENROD 0xDD24 
#define ST7789_GRAY 0x8410 
//#define ST7789_GREEN 0x0400 
#define ST7789_GREENYELLOW 0xAFE5 
#define ST7789_HONEYDEW 0xF7FE 
#define ST7789_HOTPINK 0xFB56 
#define ST7789_INDIANRED 0xCAEB 
#define ST7789_INDIGO 0x4810 
#define ST7789_IVORY 0xFFFE 
#define ST7789_KHAKI 0xF731 
#define ST7789_LAVENDER 0xE73F 
#define ST7789_LAVENDERBLUSH 0xFF9E 
#define ST7789_LAWNGREEN 0x7FE0 
#define ST7789_LEMONCHIFFON 0xFFD9 
#define ST7789_LIGHTBLUE 0xAEDC 
#define ST7789_LIGHTCORAL 0xF410 
#define ST7789_LIGHTCYAN 0xE7FF 
#define ST7789_LIGHTGOLDENRODYELLOW 0xFFDA 
#define ST7789_LIGHTGREEN 0x9772 
#define ST7789_LIGHTGREY 0xD69A 
#define ST7789_LIGHTPINK 0xFDB8 
#define ST7789_LIGHTSALMON 0xFD0F 
#define ST7789_LIGHTSEAGREEN 0x2595 
#define ST7789_LIGHTSKYBLUE 0x867F 
#define ST7789_LIGHTSLATEGRAY 0x7453 
#define ST7789_LIGHTSTEELBLUE 0xB63B 
#define ST7789_LIGHTYELLOW 0xFFFC 
#define ST7789_LIME 0x07E0 
#define ST7789_LIMEGREEN 0x3666 
#define ST7789_LINEN 0xFF9C 
#define ST7789_MAGENTA 0xF81F 
#define ST7789_MAROON 0x8000 
#define ST7789_MEDIUMAQUAMARINE 0x6675 
#define ST7789_MEDIUMBLUE 0x0019 
#define ST7789_MEDIUMORCHID 0xBABA 
#define ST7789_MEDIUMPURPLE 0x939B 
#define ST7789_MEDIUMSEAGREEN 0x3D8E 
#define ST7789_MEDIUMSLATEBLUE 0x7B5D 
#define ST7789_MEDIUMSPRINGGREEN 0x07D3 
#define ST7789_MEDIUMTURQUOISE 0x4E99 
#define ST7789_MEDIUMVIOLETRED 0xC0B0 
#define ST7789_MIDNIGHTBLUE 0x18CE 
#define ST7789_MINTCREAM 0xF7FF 
#define ST7789_MISTYROSE 0xFF3C 
#define ST7789_MOCCASIN 0xFF36 
#define ST7789_NAVAJOWHITE 0xFEF5 
#define ST7789_NAVY 0x0010 
#define ST7789_OLDLACE 0xFFBC 
#define ST7789_OLIVE 0x8400 
#define ST7789_OLIVEDRAB 0x6C64 
#define ST7789_ORANGE 0xFD20 
#define ST7789_ORANGERED 0xFA20 
#define ST7789_ORCHID 0xDB9A 
#define ST7789_PALEGOLDENROD 0xEF55 
#define ST7789_PALEGREEN 0x9FD3 
#define ST7789_PALETURQUOISE 0xAF7D 
#define ST7789_PALEVIOLETRED 0xDB92 
#define ST7789_PAPAYAWHIP 0xFF7A 
#define ST7789_PEACHPUFF 0xFED7 
#define ST7789_PERU 0xCC27 
#define ST7789_PINK 0xFE19 
#define ST7789_PLUM 0xDD1B 
#define ST7789_POWDERBLUE 0xB71C 
#define ST7789_PURPLE 0x8010 
#define ST7789_RED 0xF800 
#define ST7789_ROSYBROWN 0xBC71 
#define ST7789_ROYALBLUE 0x435C 
#define ST7789_SADDLEBROWN 0x8A22 
#define ST7789_SALMON 0xFC0E 
#define ST7789_SANDYBROWN 0xF52C 
#define ST7789_SEAGREEN 0x2C4A 
#define ST7789_SEASHELL 0xFFBD 
#define ST7789_SIENNA 0xA285 
#define ST7789_SILVER 0xC618 
#define ST7789_SKYBLUE 0x867D 
#define ST7789_SLATEBLUE 0x6AD9 
#define ST7789_SLATEGRAY 0x7412 
#define ST7789_SNOW 0xFFDF 
#define ST7789_SPRINGGREEN 0x07EF 
#define ST7789_STEELBLUE 0x4416 
#define ST7789_TAN 0xD5B1 
#define ST7789_TEAL 0x0410 
#define ST7789_THISTLE 0xDDFB 
#define ST7789_TOMATO 0xFB08 
#define ST7789_TURQUOISE 0x471A 
#define ST7789_VIOLET 0xEC1D 
#define ST7789_WHEAT 0xF6F6 
#define ST7789_WHITE 0xFFFF 
#define ST7789_WHITESMOKE 0xF7BE 
#define ST7789_YELLOW 0xFFE0 
#define ST7789_YELLOWGREEN 0x9E66

#endif
