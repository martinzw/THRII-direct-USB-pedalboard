/*THR30II Pedal using Teensy 3.6
* Martin Zwerschke 04/2021
*
* Globals.h
* last modified 09/2021
*  Author: Martin Zwerschke
*/

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <map>

class Constants   //Class for holding all the global keys (received from THR30II by request)
{
   public:
     static std::map<String, uint16_t> glo;

     static void set_all(const byte* buf);
};

//Function for making data SysEx-compatible (0x00...0x7f) by packing MSB of a pack of 7 Bytes in an eighth byte ("bitbucket")
//eight: reference to a std::array<byte,n> as the target  (bitbucketed data)
//seven: reference to a std::array<byte,n> as the source  (data, that is to be bitbucketed)
//psevenlast: pointer to the last valid byte in the source buffer (buffer size can exceed used data size in the buffer)
//A template function is needed to allow different sizes of std::array<byte,n> 
//returns a pointer to the last valid byte in target buffer "eight"
template<typename T8, typename T7>
byte *Enbucket(T8 &eight, const T7 &seven, const byte *psevenlast)
{
    memset(&eight[0],0,eight.size()); //set target bytes to 0
    auto psevenfirst = seven.begin();
    size_t sevensize= psevenlast-psevenfirst;

    int groups = sevensize / 7;  //Number of 7-groups we can build from the data
    if (sevensize == 0 || sevensize % 7 > 0) //No data? => At least one group
    {                                          //There are remaining bytes? => one group more
        groups++;
    }
    
    //change endpointer of the buffer for the encoded data
    byte* pneweightlast = eight.begin() +  (size_t)(8 * groups);  

    for (uint16_t g = 0; g < groups; g++) //run through the complete groups
    {
        byte bitbucket = 0x00; //containing the MSBs for the following seven 7-Byte-values

        if (sevensize >= 7u * g )
        {
            for (uint16_t i = 0; i < 7; i++) //for each of the 7 bytes of the group
            {
                uint16_t index_raw = i + 7 * g;  //where to find the raw byte

                if (index_raw < sevensize)
                {
                    bitbucket |= (byte)( (seven[index_raw] & 0b10000000) >> (1 + i));  //get the MSB and place it inside the "bit bucket"
                    eight[g * 8 + i + 1] = (byte)(seven[index_raw] & 0b01111111); //strip off the MSB and place the rest in the target buffer
                }
            }
            eight[0 + 8 * g] = bitbucket;  //Place the "bit bucket" in front of the 8-group
        }
    }

    return pneweightlast;
}

//Function to print out a hex dump of a std::array<byte,n>
//sb:  reference to the buffer to dump
//cnt: count of bytes to be dumped
//A template function is needed to allow different sizes of std::array<byte,n> 
template <typename T>
void hexdump (const T &sb, size_t cnt) 
{
        for(size_t i=0 ; i<cnt; i++ )
        {
            if(i%16==0) 
            {
                Serial.printf("\n\r %04x ", i );
            }
            
            //sprintf(temp+3*i,"%02x ",sb.at(i));
            Serial.printf("%02x ",sb.at(i));
        }
        Serial.println();
};

int freeMemory();  //for development only

#endif
