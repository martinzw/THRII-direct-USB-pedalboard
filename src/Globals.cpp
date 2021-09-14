/*THR30II Pedal using Teensy 3.6
* Martin Zwerschke 04/2021
*
* Globals.cpp
*  last modified 09/2021
*  Author: Martin Zwerschke 
*/

#include <Arduino.h>
#include "Globals.h"
#include <stddef.h>
#include <string.h>

//Normal TRACE/DEBUG
#define TRACE_THR30IIPEDAL(x) x
//#define TRACE_THR30IIPEDAL(x)

//Verbose TRACE/DEBUG
//#define TRACE_V_THR30IIPEDAL(x)	x
#define TRACE_V_THR30IIPEDAL(x)

void Constants::set_all(const byte* buf)
{
  uint32_t vals = *((uint32_t*) buf);       //how many values are in the symbol table
  uint32_t len =  *((uint32_t*) (buf+4));   //how many bytes build the symbol table

  TRACE_THR30IIPEDAL(Serial.printf("%d symbols found.\n\r",vals);
                      Serial.printf("%d file length.\n\r",len);
                    )
  int symStart = (int)(12 * vals + 8);   //Where is the start of the symbol names?    
  
  TRACE_THR30IIPEDAL(Serial.printf("Start of symbols: %d\n\r"+symStart);)
  
  // print out for analysis only
  // char tmp[60];

  // for(uint16_t i =0; i < len; i++ )
  // {
  //     if(i %16==0)
  //     {
  //         sprintf(tmp,"\r\n%04X : ",i);
  //     }
  //     Serial.print(tmp);
  //     sprintf(tmp,"%02X ",buf[i]);
  //     Serial.print(tmp);
  // }

  Serial.println();
  glo.clear();

  char *beg= (char*) (buf+symStart);
  char *p=beg;
  
  for(uint16_t keynr=0; keynr< vals; keynr++ )
  {
      glo.emplace(String(p), keynr);
      TRACE_V_THR30IIPEDAL(Serial.printf("%s : %d\n\r",p, keynr);)
      while( *(p++)!='\0');
  }
};

//Get count of free memory (for development only)
#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__
 
int freeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}