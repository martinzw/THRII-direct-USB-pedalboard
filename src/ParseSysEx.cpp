/*THR30II Pedal using Teensy 3.6
* Martin Zwerschke 04/2021
*
*
* ParseSysEx.cpp
*  last modified 09/2021
*  Author: Martin Zwerschke 
*/

//#include <stdexcept>   //exceptions are problematic on Arduino / Teensy we avoid them
#include "THR30II_Pedal.h"
#include "Globals.h"

//Function walks through an incoming MIDI-SysEx-Message and parses it's meaning
//cur[] : the buffer
//cur_len : length of the buffer
//returns a message String, describing the result (for writing to Serial Monitor)
//If a valid message is found, appropriate actions are taken (setting values, responding messages to follow the protocol)
String THR30II_Settings::ParseSysEx(const byte cur[], int cur_len)  
{
    if (cur_len < 5)
    {
        return "SysEx too short!";
    }

    std::map<String,uint16_t> & glob = Constants::glo ;  //Reference to the symbol table (received in the forefield)

    sendChangestoTHR = false;  //do not send THR-caused setting changes back to THR!

    //parsing a valid message

    String result;
    String txt;
    
    if (cur[2] == 0x01 && cur[3] == 0x0c && cur[4] == 0x24)   //Valid SysEx start : THR30II (LINE6 MIDI-ID )
    {
        //todo: perhaps check here, if counter is 0x00 or number of last received THR30II-SysEx

        result+="\n\rTHR30_II:";
        uint16_t familyID = (uint16_t)(((uint16_t)cur[5] << 8) + (uint16_t)cur[6]); //should be 0x024d
        byte writeOrrequest = cur[7]; //01 for request 00 for write
        byte sameFrameCounter = cur[9];
        uint16_t payloadSize = (uint16_t)(cur[10] * 16 + cur[11] + 1u);
        byte blockCount = (byte)(payloadSize / 7);  //e.g. 21/7 = 3 Blocks with 7 Bytes
        byte remainder = (byte)(payloadSize % 7);   //e.g. 20%7 = 6 Bytes
        byte msgbytes[payloadSize];
        size_t msgValsLength=payloadSize / 4 + (payloadSize % 4 != 0 ? 1 : 0);
        uint32_t msgVals[msgValsLength];

        if (familyID == 0x024d)  //Valid Message for THRII received
        {
            //Any valid incoming message proofs, that THR30II MIDI-Interface activated!
            MIDI_Activated = true;
            //result+="\n\rMIDI activated\n\r";
            //Unpack Bitbucket encoding
            for (int i = 0; i <= blockCount; i++)
            {
                if (12 + 8 * i >= cur_len)
                {
                    //Just to set a breakpoint for debugging
                }
                byte bitbucket = cur[12 + 8 * i];

                int last = (i == blockCount) ? remainder : 7;

                for (int j = 0; j < last; j++)
                {
                    if (13 + 8 * i + j >= cur_len)
                    {
                        //Just to set a breakpoint for debugging
                    }
                    byte v1 = cur[13 + 8 * i + j];  //fetch next byte from message
                    byte bucketmask = (byte)(1 << (6 - j));
                    byte res = (byte)(bitbucket & bucketmask);

                    msgbytes[j + 7 * i] = res != 0 ? (byte)(v1 | 0x80) : v1;
                }
            }

            //Split message into 4-Byte valuesSeconds
            for (int i = 0; i < payloadSize; i += 4)
            {
                if (payloadSize > i + 3) //At least 4 Bytes remaining 
                {
                    msgVals[i / 4] =  *( (uint32_t*) (msgbytes+i));  //read out 4-Byte-Value as an int
                }
                else  //no complete 4-Byte-value left in block
                {   //construct a single value from remaining bytes (combine 1,2,3 bytes to one value)
                    msgVals[i / 4] = ( (uint32_t)msgbytes[i] << 24 )
                                    + ((i + 1) < payloadSize ? msgbytes[i + 1] << 16 : 0x00u)
                                    + ((i + 2) < payloadSize ? msgbytes[i + 2] << 8  : 0x00u);
                }
            }
        } // von if familyID == 0x024d
        else  //frame begins like a normal THR30II-Answer but not the normal 02 4d follows (should be the 2nd answer to universal inquiry)
        {        //cur[5], cur[6]
            if (familyID == 0x027E)  //02 7e  +(7f 06 02)  (not a family ID but the 2nd answer frame to identity request)
            {
                if (cur[7] == 0x7F && cur[8] == 0x06 && cur[9] == 0x02)  // is it really the 2nd Answer to univ. identity request?
                {
                    int i = 10;

                    for (i = 10; i < 100;)    //copy Identity string part 1
                    {
                        if (cur[i] == 0)
                        {
                            txt+=("\r\n\t");
                            break;
                        }

                        txt+=((char)cur[i++]);
                    }

                    for (i++; i < 100;)      //copy Identity string part 2
                    {
                        if (cur[i] == 0)
                            break;

                        txt+=((char)cur[i++]);
                    }

                    result+=" : " + txt;

                    outqueue.getHeadPtr()->_answered = true;  //mark question as answered
                }

            }
            payloadSize = 0xffff;  //marker for unknown msg-Type

        }  //end of "not a normal THR30II-message but perhaps response to univ. inquiry)

        if ((payloadSize <= 24) && dumpInProgress)  //a normal length frame can't belong to a dump (?) => finish it, if running
        {                                           //not sure about this!   Does not work for 1.40.0a any more 
                                                    //because last frame of symbol table dump is only 0x0c in length
            result+="\n\rFinished dump.\n\r";
            //dumpInProgress = false;
            //dumpFrameNumber = 0;
        }
        
        //result+="PayloadSize: "+String(payloadSize)+"\r\n";

        switch (payloadSize)  //switch Message-Type by length of payload (perhaps not the best method?)
        {
            case 9:   //Answer-Message
                result+=" 9-Byte-Message";
                //msgVals: The Message: [0]:Opcode, [1]:Length, [2]:Block-Key/TargetType
                if (msgVals[0] == 0x00000001 && msgVals[1] == 0x00000001)   //seems to be an answer
                {
                    switch (msgVals[2])  //The 3rd Data-Value of the 9-Byte Message is just one byte in the SysEx-Data
                    {                    //but here it is packed to 4-Byte msgVal as the MSB!
                        case 0x00000000:
                        case 0x01000000:
                        {   
                            int id = 0;
                            if (outqueue.itemCount() > 0)
                            {
                                id = outqueue.getHeadPtr()->_id;

                                if (id == 7)  //only react, if it was the question #7 from the boot-up dialog ("have user settings changed?")
                                {
                                    outqueue.getHeadPtr()->_answered = true;

                                    //here we react depending on the answer: If settings have changed: We need the actual settings
                                    //and the five user presets
                                    //If settings have not changed (a User-Preset is active and not modified):
                                    //We only need the 5 User Presets and ask which one is active

                                    if (msgVals[2] != 0)  //If settings have changed: We need the actual settings    
                                    {
                                        userSettingsHaveChanged = true;
                                        activeUserSetting = -1; //So -no- User Preset is active right now
                                    }
                                    else  //an unchanged User-Setting is active
                                    {
                                        userSettingsHaveChanged = false;
                                        //We request the number of the active user setting later (#S10)!
                                    }

                                    //#S8   Request actual user settings (Expect several frames - settings dump)
                                    //answer will be the settings dump for actual settings
                                    outqueue.enqueue(Outmessage(SysExMessage( (const byte[29]) { 0xf0, 0x00, 0x01, 0x0c, 0x22, 0x02, 0x4d, 0x01, 0x02, 0x00, 0x00, 0x0b, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x3c, 0x00, 0x7f, 0x7f, 0x7f, 0x7f, 0x00, 0x00, 0xf7 } ,29), 8, false, true));

                                    //#S9  Header for a System Question
                                    //it is a header, no ack, no answer
                                    outqueue.enqueue(Outmessage(SysExMessage((const byte[29]) { 0xf0, 0x00, 0x01, 0x0c, 0x22, 0x02, 0x4d, 0x00, 0x04, 0x00, 0x00, 0x07, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf7 },29), 9, false, false));

                                    //#S10   System Question body: Opcode "0x00" number of  -active-  User-Setting
                                    //Answer will be the  n u m b e r  of the active user setting
                                    outqueue.enqueue(Outmessage(SysExMessage( (const byte[21]) { 0xf0, 0x00, 0x01, 0x0c, 0x22, 0x02, 0x4d, 0x00, 0x05, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf7 },21), 10, false, true));

                                    //#S11  Request name of User-Setting #1
                                    //answer will be the name of user-setting 1 
                                    outqueue.enqueue(Outmessage(SysExMessage((const byte[29]) { 0xf0, 0x00, 0x01, 0x0c, 0x22, 0x02, 0x4d, 0x01, 0x03, 0x00, 0x00, 0x0b, 0x00, 0x06, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf7 },29), 11, false, true));

                                    //#S12  Request name of User-Setting #2
                                    //answer will be the name of user-setting 2
                                    outqueue.enqueue(Outmessage(SysExMessage((const byte[29]) { 0xf0, 0x00, 0x01, 0x0c, 0x22, 0x02, 0x4d, 0x01, 0x04, 0x00, 0x00, 0x0b, 0x00, 0x06, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf7 },29), 12, false, true));

                                    //#S13  Request name of User-Setting #3
                                    //answer will be the name of user-setting 3
                                    outqueue.enqueue(Outmessage(SysExMessage((const byte[29]){ 0xf0, 0x00, 0x01, 0x0c, 0x22, 0x02, 0x4d, 0x01, 0x05, 0x00, 0x00, 0x0b, 0x00, 0x06, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf7 },29), 13, false, true));

                                    //#S14  Request name of User-Setting #4
                                    //answer will be the name of user-setting 4
                                    outqueue.enqueue(Outmessage(SysExMessage((const byte[29]) { 0xf0, 0x00, 0x01, 0x0c, 0x22, 0x02, 0x4d, 0x01, 0x06, 0x00, 0x00, 0x0b, 0x00, 0x06, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf7 },29),14,false,true));
                                    
                                    //#S15 Request name of User-Setting #5
                                    //answer will be the name of user-setting 4
                                    outqueue.enqueue(Outmessage(SysExMessage((const byte[29]) { 0xf0, 0x00, 0x01, 0x0c, 0x22, 0x02, 0x4d, 0x01, 0x07, 0x00, 0x00, 0x0b, 0x00, 0x06, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf7 },29), 15, false, true));

                                    //request user settings 1..5 (?)

                                     //#S16 Header 0D for Syst.Read G10T             
                                    outqueue.enqueue(Outmessage(SysExMessage((const byte[29]) { 0xf0, 0x00, 0x01, 0x0c, 0x24, 0x02, 0x4d, 0x00, 0x08, 0x00, 0x00, 0x07, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf7 },29), 16, false, false)); //Header!

                                    //#S17  Syst. Read G10T Value 0B  (after Header 0D)
                                    outqueue.enqueue(Outmessage(SysExMessage((const byte[21]) { 0xf0, 0x00, 0x01, 0x0c, 0x24, 0x02, 0x4d, 0x00, 0x09, 0x00, 0x00, 0x03, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf7 },21),17, false, true)); //answer will be 01, 0c, 00, 02, 00=extracted / 02=plugged in

                                    //#S18 Header 0D for Syst.Read Front-LED state
                                    outqueue.enqueue(Outmessage(SysExMessage((const byte[29]) { 0xf0, 0x00, 0x01, 0x0c, 0x24, 0x02, 0x4d, 0x00, 0x2a, 0x00, 0x00, 0x07, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf7 },29), 18, false, false)); //Header!;

                                    //#S19  Syst. Read Front-LED Value 02  (after Header 0D)
                                    outqueue.enqueue(Outmessage(SysExMessage((const byte[29]) { 0xf0, 0x00, 0x01, 0x0c, 0x24, 0x02, 0x4d, 0x00, 0x2b, 0x00, 0x00, 0x03, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf7 },29), 19, false, true)); 
                                    
                                    //#S20   Header 09 for Global Param Read, 8 Byte follow
                                    outqueue.enqueue(Outmessage(SysExMessage((const byte[29]) { 0xf0, 0x00, 0x01, 0x0c, 0x24, 0x02, 0x4d, 0x00, 0x06, 0x00, 0x00, 0x07, 0x00, 0x09, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf7 },29), 20, false, false)); //Header!

                                    //#S21  Global Param Read TunerEnable Value FFFFFFFF 0000014D  (after Header 09)
                                    uint16_t key = glob["TunerEnable"];
                                    char tmp [29]= { 0xf0, 0x00, 0x01, 0x0c, 0x24, 0x02, 0x4d, 0x00, 0x07, 0x00, 0x00, 0x07, 0x78, 0x7f, 0x7f, 0x7f, 0x7f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf7 };
                                    tmp[17]= (byte)(key % 256); //insert key for "TunerEnable" into message data
                                    tmp[18]= (byte)(key / 256);
                                    SysExMessage s ((byte*) tmp,29); 
                                    outqueue.enqueue(Outmessage(s, 21, false, true)); //Tuner enabled? Answer will be 01, 0c, 00, 03, 00=inactive/01=enabled;

                                    //THR: 									    #R19
                                    //f0 00 01 0c 24 02 4d 00 05 00 01 03 00 01 00 00 00 0c 00 00 00 00 | 00 00 00 00 03 00 00 00 00 00 00 00 00 | 00 f7       //

                                    //#S22 Header 0D for Syst.Read Speaker-Tuner state
                                    outqueue.enqueue(Outmessage(SysExMessage((const byte[29]) { 0xf0, 0x00, 0x01, 0x0c, 0x24, 0x02, 0x4d, 0x00, 0x2c, 0x00, 0x00, 0x07, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf7 },29), 22, false, false)); //Header!

                                    //#S23  Syst. Read Speaker-Tuner Value 0e  (after Header 0D)                                                0e=Speaker-Tuner
                                    outqueue.enqueue(Outmessage(SysExMessage((const byte[21]) { 0xf0, 0x00, 0x01, 0x0c, 0x24, 0x02, 0x4d, 0x00, 0x2d, 0x00, 0x00, 0x03, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf7 },21), 23, false, true)); //Answer will be 01, 0c, 00, 02, 00=open/01=focus;

                                    //expect THR-reply:
                                    //0000   f0 00 01 0c 24 02 4d 00 14 00 01 03 00 01 00 00   01 = Reply
                                    //0010   00 0c 00 00 00 00|00 00 00 00 02 00 00 00 00 01   02 = type enum   01 = Focus
                                    //0020   00 00 00|00 f7

                                    //#S24   Header 09 for Global Param Read, 8 Byte follow
                                    
                                    outqueue.enqueue(Outmessage(SysExMessage((const byte[29]) { 0xf0, 0x00, 0x01, 0x0c, 0x24, 0x02, 0x4d, 0x00, 0x06, 0x00, 0x00, 0x07, 0x00, 0x09, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf7 },29), 24, false, false)); //Header!;
                                    
                                    //#S25  Global Param Read GuitarVolume Value FFFFFFFF 00000155  (after Header 09)
                                    key = glob["GuitarVolume"];
                                    byte tmp1[29]= { 0xf0, 0x00, 0x01, 0x0c, 0x24, 0x02, 0x4d, 0x00, 0x07, 0x00, 0x00, 0x07, 0x78, 0x7f, 0x7f, 0x7f, 0x7f, 0x55, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf7 };
                                    tmp1[17]=(byte)(key % 256); 
                                    tmp1[18]= (byte)(key / 256);
                                    SysExMessage s1 ( tmp1, 29 );
                                    outqueue.enqueue(Outmessage(s1, 25, false, true)); //GuitarVolume? Answer will be 01, 0c, 00, 04, GuitarVolume
                                     
                                    //#S26   Header 09 for Global Param Read, 8 Byte follow
                                    outqueue.enqueue(Outmessage(SysExMessage((const byte[29]) { 0xf0, 0x00, 0x01, 0x0c, 0x24, 0x02, 0x4d, 0x00, 0x06, 0x00, 0x00, 0x07, 0x00, 0x09, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf7 },29), 26, false, false)); //Header!

                                    //#S27  Global Param Read AudioVolume Value FFFFFFFF 0000014B  (after Header 09)
                                    key = glob["AudioVolume"];
                                    char tmp2[29]= { 0xf0, 0x00, 0x01, 0x0c, 0x24, 0x02, 0x4d, 0x00, 0x07, 0x00, 0x00, 0x07, 0x78, 0x7f, 0x7f, 0x7f, 0x7f, 0x4b,0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf7 };
                                    tmp2[17]=(byte)(key % 256);
                                    tmp2[18]= (byte)(key / 256);
                                    SysExMessage s2((byte*) tmp2,29);
                                    outqueue.enqueue(Outmessage(s2, 27, false, true)); //AudioVolume? Answer will be 01, 0c, 00, 04, AudioVolume
                                    
                                }//of "only react, if it was the question #7 from the boot-up dialog ("have user settings changed?")"

                            } //if item_count in outqueue >0

                            result+=" Answer to 0F-00-Question #"+String(id)+": User-Setting was "+String(msgVals[2] == 0 ? "not " : "")+"changed.";
                        }
                        break;

                        default:
                            result+=" -unknown value "+String(msgVals[2])+".";
                        break;
                    } // of "switch msgVals[2]"
                } //of "if seems to be an answer"
                
            break;  //of "message 9 bytes"

            case 12:  //e.g. Acknowledge
                result+=" 12-Byte-Message";
                if (msgVals[0] == 0x0001 && msgVals[1] == 0x0004)
                {
                    switch (msgVals[2]) //Block-Key / TargetTypeKey
                    {
                        int id;
                        case 0x00000000ul:    //this is an acknowledge message
                        {  
                            id = outqueue.getHeadPtr()->_id;
                            result+=" Acknowledge for Message #"+String(id);
                            outqueue.getHeadPtr()->_acknowledged = true;
                            //function on_ack_reactions(uint16t id)

                            std::tuple<uint16_t, Outmessage, bool * , bool > tp;
                            
                            while(!on_ack_queue.isEmpty())  //are there "action packs" on the list?
                            {  
                                tp = *on_ack_queue.getHeadPtr();                                
                                if(std::get<0>(tp) == id)   //do they belong to the last outmessage, that is acknowledged now?
                                {
                                     Serial.println("working o on_ack_queue, because upload of actual setting was acknowledged. "); 

                                    if(std::get<1>(tp)._msg.getSize() != 0) //is there a message to send out?
                                    {
                                       Serial.println("Sending out an outmessage from on_ack_queue. ");
                                       outqueue.enqueue(std::get<1>(tp));
                                    }

                                    if(std::get<2>(tp)!=nullptr) //is there a  flag to set?
                                    {
                                       Serial.println("Setting a flag from on_ack_queue. ");
                                        *std::get<2>(tp)=std::get<3>(tp);  //set the value of the flag
                                    }
                                    
                                    //now read actual settings
                                    on_ack_queue.dequeue();    
                                }
                            }

                            if(id==999)
                            {
                                Serial.println(" Switch User Setting acknowledged. (999)"); 
                                //now read actual settings

                                //#S8   Request actual user settings (Expect several frames - settings dump)
                                outqueue.enqueue(Outmessage(SysExMessage((const byte[29]) { 0xf0, 0x00, 0x01, 0x0c, 0x22, 0x02, 0x4d, 0x01, 0x02, 0x00, 0x00, 0x0b, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x3c, 0x00, 0x7f, 0x7f, 0x7f, 0x7f, 0x00, 0x00, 0xf7 },29), 8, false, true)); //answer will be the settings dump for actual settings
                            }
                        }
                        break;
                        
                        case 0x00000080ul:   //Message with unknown meaning from Init-Dialog
                        {
                            id = outqueue.getHeadPtr()->_id;
                            outqueue.getHeadPtr()->_answered = true;
                            result+=" Answer "+String(msgVals[2],HEX)+" to 05-Msg #"+String(id)+" : ??";
                        }
                        break;
                        case 0x01300063ul:   //Very very old firmware version 1.30.0c
                        case 0x0131006Bul:   //very old firmware 1.31.0k
                        case 0x01400061ul:   //old firmware 1.40.0a
                        case 0x01420067ul:     //actual Firmware 1.42.0g
                            id = outqueue.getHeadPtr()->_id;
                            result+=" Answer to 01-Msg #" + String(id) +": Firmware-Version " + String(msgVals[2],HEX);
                            Firmware = msgVals[2];

                            if (writeOrrequest == 1 && MIDI_Activated)   //followed a "Req. Firmware" with the "01"-Marker
                            {   //use the 2nd "Req. Firmware"
                                //the first one is used for calculating the correct magic key

                                if (id == 5)
                                {
                                    outqueue.getHeadPtr()->_answered = true;  //this request is answered
                                }

                                //Now ask for the symbol table
                                //Answer will be the symbol dump, we can not go on without it - await answer
                                //Question requires no following frame
                                result+="\n\rAsking for Symbol table:\n\r";
                                outqueue.enqueue(Outmessage(SysExMessage((const byte[29]){ 0xf0, 0x00, 0x01, 0x0c, 0x24, 0x02, 0x4d, 0x00, 0x03, 0x00, 0x00, 0x07, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf7},29), 777, false, true));
                            }
                            else if (writeOrrequest == 0) //followed a "Req. Firmware" with the "00"-Marker
                            {
                                if (id == 2)  //was the answer to 1st "ask firmware version" in boot-Up
                                {
                                    outqueue.getHeadPtr()->_answered = true; //this request is answered
                                }

                                //#S3 + #S4 invoke #R4 (seems always the same) "MIDI activate" (works only after ID_Req!)
                                //it is a header, so no ack and no answer
                                outqueue.enqueue(Outmessage(SysExMessage((const byte[29]) { 0xf0, 0x00, 0x01, 0x0c, 0x24, 0x02, 0x4d, 0x00, 0x01, 0x00, 0x00, 0x07, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf7 },29), 3, false, false));
                                
                                //Sure there will be a formula to calculate the magic keys from the received firmware version - but I don't know it :-(
                                //#S4  (at least continue to this frame, to activate THR30II MIDI-Interface )
                                if (Firmware == 0x01300063)  //old firmware 1.30.0c
                                {
                                    //Firmware 1.30.0c 60 6b 3e 6f 68
                                    //send Midi activation and await ack
                                    outqueue.enqueue(Outmessage(SysExMessage( (const byte[21]){ 0xf0, 0x00, 0x01, 0x0c, 0x24, 0x02, 0x4d, 0x00, 0x02, 0x00, 0x00, 0x03, 0x60, 0x6b, 0x3e, 0x6f, 0x68, 0x00, 0x00, 0x00, 0xf7 },21), 4, true, false));
                                }
                                else if (Firmware == 0x0131006B) //new firmware 1.31.1k
                                {
                                    //Firmware 1.31.0k  28 24 6b 09 18
                                    //send Midi activation and await ack
                                    outqueue.enqueue(Outmessage(SysExMessage( (const byte[21]) { 0xf0, 0x00, 0x01, 0x0c, 0x24, 0x02, 0x4d, 0x00, 0x02, 0x00, 0x00, 0x03, 0x28, 0x24, 0x6b, 0x09, 0x18, 0x00, 0x00, 0x00, 0xf7 },21), 4, true, false));
                                }
                                else if (Firmware == 0x01400061) //new firmware 1.40.0a
                                {
                                    //Firmware 1.40.0a   10 5c 61 06 79
                                    //send Midi activation and await ack
                                    outqueue.enqueue(Outmessage(SysExMessage( (const byte[21]) { 0xf0, 0x00, 0x01, 0x0c, 0x24, 0x02, 0x4d, 0x00, 0x02, 0x00, 0x00, 0x03, 0x10, 0x5c, 0x61, 0x06, 0x79, 0x00, 0x00, 0x00, 0xf7 },21), 4, true, false));
                                }
                                else if (Firmware == 0x01420067) //newest firmware 1.42.0g  
                                {
                                    
                                    //Firmware 1.42.0g  28 72 4d 54 5d
                                    //send Midi activation and await ack
                                    outqueue.enqueue(Outmessage(SysExMessage( (const byte[21]) { 0xf0, 0x00, 0x01, 0x0c, 0x24, 0x02, 0x4d, 0x00, 0x02, 0x00, 0x00, 0x03, 0x28, 0x72, 0x4d, 0x54, 0x5d, 0x00, 0x00, 0x00, 0xf7 },21), 4, true, false)); 
                                }

                                //#S5  Request Firmware-Version(?) (Answ. always the same)  like #S2, but with "01" in head
                                //Answer is firmware version (this is "1" -version of the question message)
                                outqueue.enqueue(Outmessage(SysExMessage((const byte[29]){ 0xf0, 0x00, 0x01, 0x0c, 0x22, 0x02, 0x4d, 0x01, 0x00, 0x00, 0x00, 0x07, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf7 },29), 5, false, true));
                            }
                            break;
                        case (uint32_t)0xFFFFFFF9ul:
                            result+=" New firmware FFFFFFF9-Not Acknowledge (-7 = wrong MIDI-activate-Code)";
                            //what to do here ???
                            break;
                        case (uint32_t)0xFFFFFFFFul:
                            result+=" Not Acknowledge (-1)";
                            //what to do here ???
                            break;
                        default:
                            result+=" -unknown "+String(msgVals[2]);
                            break;
                    }
                }
            break; //of "message 12 Bytes"
            
            case 16:  //AMP- and UNIT-Mode changes
                result+=" 16-Byte-Message";
                if (cur_len > 0x1a)
                {
                    //msgVals: The Message: [0]:Opcode, [1]:Length, [2]:Block-Key/TargetType
                    if (msgVals[0] == 0x00000003 && msgVals[1] == 0x00000008)
                    {
                        if (msgVals[2] == THR30II_UNITS_VALS[CONTROL].key)//== 0x010a  "Amp"
                        {
                            col_amp ca {CLASSIC, CLEAN};
                        
                            if(msgVals[3]==THR30IIAmpKeys[ col_amp{CLASSIC, CLEAN} ]) //0x004A:  "THR10C_Deluxe"
                            {
                                result+=" CLEAN CLASSIC";
                                ca = THR30IIAmpKey_ToColAmp((uint16_t)msgVals[3]);
                                SetColAmp( ca.c, ca.a );
                            }
                            else if (msgVals[3]== THR30IIAmpKeys[col_amp{BOUTIQUE, CRUNCH} ]) //0x0071: "THR10C_Mini"
                            {
                                result+=" CRUNCH BOUTIQUE";
                                ca = THR30IIAmpKey_ToColAmp((uint16_t)msgVals[3]);
                                SetColAmp(ca.c, ca.a);
                            }
                            else if(msgVals[3] == THR30IIAmpKeys[col_amp{CLASSIC, SPECIAL}]) //0x0078:  "THR10X_Brown1"
                            {    result+=" SPECIAL  CLASSIC";
                                ca = THR30IIAmpKey_ToColAmp((uint16_t)msgVals[3]);
                                SetColAmp(ca.c, ca.a);
                            }
                            else if(msgVals[3] == THR30IIAmpKeys[col_amp{CLASSIC, HI_GAIN}]) //0x007F:  "THR10_Modern"
                            {    result+=" HIGHGAIN Classic";
                                ca = THR30IIAmpKey_ToColAmp((uint16_t)msgVals[3]);
                                SetColAmp(ca.c, ca.a);
                            }
                            else if(msgVals[3] == THR30IIAmpKeys[col_amp{BOUTIQUE, SPECIAL}])//0x0084: "THR10X_South"
                            {    result+=" SPECIAL BOUTIQUE";
                                ca = THR30IIAmpKey_ToColAmp((uint16_t)msgVals[3]);
                                SetColAmp(ca.c, ca.a);
                            }
                            else if(msgVals[3] == THR30IIAmpKeys[col_amp{CLASSIC, CRUNCH}])//0x0088:  "THR10C_DC30"
                            {    result+=" CRUNCH CLASSIC";
                                ca = THR30IIAmpKey_ToColAmp((uint16_t)msgVals[3]);
                                SetColAmp(ca.c, ca.a);
                            }
                            else if(msgVals[3] == THR30IIAmpKeys[col_amp{MODERN, HI_GAIN}])//0x008B:  "THR10X_Brown2"
                            {    result+=" HIGAIN MODERN";
                                ca = THR30IIAmpKey_ToColAmp((uint16_t)msgVals[3]);
                                SetColAmp(ca.c, ca.a);
                            }
                            else if(msgVals[3] ==  THR30IIAmpKeys[col_amp{CLASSIC, FLAT}])//0x008F:  "THR10_Flat"
                            {    result+=" FLAT CLASSIC";
                                ca = THR30IIAmpKey_ToColAmp((uint16_t)msgVals[3]);
                                SetColAmp(ca.c, ca.a);
                            }
                            else if(msgVals[3] == THR30IIAmpKeys[col_amp{CLASSIC, ACO}])//0x0093:  "THR10_Aco_Condenser1"
                            {    result+=" ACOUSTIC CLASSIC";
                                ca = THR30IIAmpKey_ToColAmp((uint16_t)msgVals[3]);
                                SetColAmp(ca.c, ca.a);
                            }
                            else if(msgVals[3] == THR30IIAmpKeys[col_amp{MODERN, ACO}])//0x0097:  "THR10_Aco_Dynamic1"
                            {    result+=" ACOUSTIC MODERN";
                                ca = THR30IIAmpKey_ToColAmp((uint16_t)msgVals[3]);
                                SetColAmp(ca.c, ca.a);
                            }
                            else if(msgVals[3]  == THR30IIAmpKeys[col_amp{BOUTIQUE, ACO}])//0x0098:  "THR10_Aco_Tube1"
                            {    result+=" ACOUSTIC BOUTIQUE";
                                ca = THR30IIAmpKey_ToColAmp((uint16_t)msgVals[3]);
                                SetColAmp(ca.c, ca.a);
                            }
                            else if(msgVals[3]  == THR30IIAmpKeys[col_amp{CLASSIC, LEAD}])//0x0099:  "THR10_Lead"
                            {    result+=" LEAD CLASSIC";
                                ca = THR30IIAmpKey_ToColAmp((uint16_t)msgVals[3]);
                                SetColAmp(ca.c, ca.a);
                            }
                            else if(msgVals[3]  == THR30IIAmpKeys[col_amp{MODERN, SPECIAL}])//0x009C:  "THR30_Stealth"
                            {    result+=" SPECIAL MODERN";
                                ca = THR30IIAmpKey_ToColAmp((uint16_t)msgVals[3]);
                                SetColAmp(ca.c, ca.a);
                            }
                            else if(msgVals[3]  == THR30IIAmpKeys[col_amp{MODERN, CRUNCH}]) //0x00A3:  "THR30_SR101"
                            {    result+=" CRUNCH MODERN";
                                ca = THR30IIAmpKey_ToColAmp((uint16_t)msgVals[3]);
                                SetColAmp(ca.c, ca.a);
                            }
                            else if(msgVals[3]  == THR30IIAmpKeys[col_amp{BOUTIQUE, LEAD}])//0x00A8:  "THR30_Blondie"
                            {    result+=(" LEAD BOUTIQUE");
                                ca = THR30IIAmpKey_ToColAmp((uint16_t)msgVals[3]);
                                SetColAmp(ca.c, ca.a);
                            }
                            else if(msgVals[3]  == THR30IIAmpKeys[col_amp{BOUTIQUE, BASS}])//0x000AB: "THR10_Bass_Mesa"
                            {    result+=" BASS-Model BOUTIQUE";
                                ca = THR30IIAmpKey_ToColAmp((uint16_t)msgVals[3]);
                                SetColAmp(ca.c, ca.a);
                            }
                            else if(msgVals[3]  == THR30IIAmpKeys[col_amp{BOUTIQUE, HI_GAIN}])//0x00AF: "THR30_FLead"
                            {    result+=(" HIGHGAIN BOUTIQUE");
                                ca = THR30IIAmpKey_ToColAmp((uint16_t)msgVals[3]);
                                SetColAmp(ca.c, ca.a);
                            }
                            else if(msgVals[3]  == THR30IIAmpKeys[col_amp{MODERN, CLEAN}])//0x00B2: "THR30_Carmen"
                            {    result+=" CLEAN MODERN";
                                ca = THR30IIAmpKey_ToColAmp((uint16_t)msgVals[3]);
                                SetColAmp(ca.c, ca.a);
                            }
                            else if(msgVals[3]  == THR30IIAmpKeys[col_amp{BOUTIQUE, FLAT}])//0x00B4: "THR10_Flat_B"
                            {    result+=" FLAT BOUTIQUE";
                                ca = THR30IIAmpKey_ToColAmp((uint16_t)msgVals[3]);
                                SetColAmp(ca.c, ca.a);
                            }
                            else if(msgVals[3]  == THR30IIAmpKeys[col_amp{MODERN, FLAT}])//0x00B5: "THR10_Flat_V"
                            {    result+=" FLAT MODERN";
                                ca = THR30IIAmpKey_ToColAmp((uint16_t)msgVals[3]);
                                SetColAmp(ca.c, ca.a);
                            }
                            else if(msgVals[3]  == THR30IIAmpKeys[col_amp{BOUTIQUE, CLEAN}])//0x00B6: "THR10C_BJunior2"
                            {    result+=" CLEAN BOUTIQUE";
                                ca = THR30IIAmpKey_ToColAmp((uint16_t)msgVals[3]);
                                SetColAmp(ca.c, ca.a);
                            }
                            else if(msgVals[3]  == THR30IIAmpKeys[col_amp{CLASSIC, BASS}])//0x00B7:  "THR10_Bass_Eden_Marcus"
                            {    result+=" BASS-Model CLASSIC";
                                ca = THR30IIAmpKey_ToColAmp((uint16_t)msgVals[3]);
                                SetColAmp(ca.c, ca.a);
                            }
                            else if(msgVals[3]  == THR30IIAmpKeys[col_amp{MODERN, BASS}])//0x00B8: "THR30_JKBass2"
                            {    result+=" BASS-Model MODERN";
                                ca = THR30IIAmpKey_ToColAmp((uint16_t)msgVals[3]);
                                SetColAmp(ca.c, ca.a);
                            }
                            else if(msgVals[3]  == THR30IIAmpKeys[col_amp{MODERN, LEAD}])//0x00BB: "THR10_Brit"
                            {    result+=" LEAD MODERN";
                                ca = THR30IIAmpKey_ToColAmp((uint16_t)msgVals[3]);
                                SetColAmp(ca.c, ca.a);
                            }
                            else
                            {
                                result+=" "+String(msgVals[3])+" in AMP-Message ";
                            }
                        }  //of Block AMP
                        else if (msgVals[2] == THR30II_UNITS_VALS[REVERB].key)//== 0x0112  Block Reverb ("FX4")
                        {
                            if(msgVals[3] == THR30II_REV_TYPES_VALS[SPRING].key) //0x00F1:  "StandardSpring"
                            {
                                result+=" Reverb: Mode SPRING selected ";
                                ReverbSelect(SPRING);
                            }
                            else if(msgVals[3] == THR30II_REV_TYPES_VALS[HALL].key) //0x00FC:  "ReallyLargeHall"
                            {
                                result+=" Reverb: Mode HALL selected ";
                                ReverbSelect(HALL);
                            }
                            else if(msgVals[3] == THR30II_REV_TYPES_VALS[PLATE].key) //0x00F7 "LargePlate1"
                            {
                                result+=(" Reverb: Mode PLATE selected ");
                                ReverbSelect(PLATE);
                            }
                            else if(msgVals[3]==THR30II_REV_TYPES_VALS[ROOM].key) //0x00FE "SmallRoom1"
                            {
                                result+=(" Reverb: Mode Room selected ");
                                ReverbSelect(ROOM);
                            }
                            else
                            {
                                result+=String(cur[0x1a],HEX)+" Reverb: unknown Mode selected"; 
                            }
                        }//of Block Reverb
                        else if (msgVals[2] == THR30II_UNITS_VALS[EFFECT].key) //== 0x010c  Block Effect  ("FX2")
                        {
                            if(msgVals[3] == THR30II_EFF_TYPES_VALS[PHASER].key)//0x00D0:  "Phaser"
                            {   result+=(" Effect: Mode PHASER selected ");
                                EffectSelect(PHASER);
                            }
                            else if(msgVals[3] == THR30II_EFF_TYPES_VALS[TREMOLO].key)// 0x00DC:  "BiasTremolo"
                            {   result+=(" Effect: Mode TREMOLO selected ");
                                EffectSelect(TREMOLO);
                            }
                            else if(msgVals[3] == THR30II_EFF_TYPES_VALS[FLANGER].key)//0x00E2:  "L6Flanger"
                            {   result+=(" Effect: Mode FLANGER selected ");
                                EffectSelect(FLANGER);
                            }
                            else if(msgVals[3] == THR30II_EFF_TYPES_VALS[CHORUS].key)//0x00E6:  "StereoSquareChorus"
                            {   result+=(" Effect: Mode CHORUS selected ");
                                EffectSelect(CHORUS);
                            }
                            else 
                            {   
                                 result+=(" Effect: unknown Mode selected  0x"+String(msgVals[3],HEX));
                            }
                        }//of Block Effect
                        else if (msgVals[2] == THR30II_UNITS_VALS[COMPRESSOR].key) //== 0x0107 Block Compressor ("FX1")
                        {                                                                 // Normally this should not occure
                            if(msgVals[3] == glob["RedComp"] )                            //(because compressor can only be configured by the App)
                            {
                                result+=" Compressor: Mode RedComp selected ";
                                //at the moment there is no other Compressor unit anyway!
                            }
                            else if(msgVals[3] == glob["Red Comp"] )       //There is one global key "RedComp" and another "Red Comp" with a space!
                            {
                                result+=" Compressor: Mode Red Comp selected ";
                                //at the moment there is no other Compressor unit anyway!
                            }
                            else
                            {
                                result+=" Compressor: unknown "; 
                            }
                        }
                        else if (msgVals[2] == THR30II_UNITS_VALS[GATE].key) //== 0x013C  // Block "GuitarProc"
                        {
                                                               // Normally this should not occure
                            if(msgVals[3] == glob["Gate"] )    //(because Noise Gate can only be configured by the App)     
                            {
                                result+=" GuitarProc: Mode Gate selected ";
                                //at the moment there is no other subunit anyway!
                            }
                            else
                            {
                                result+=" Gate: unknown "; 
                            }                                                                                              
                        }  //since firmware 1.40.0a Block Echo could eventually appear (Types TapeEcho/DigitalDelay)
                        else if (msgVals[2] == THR30II_UNITS_VALS[ECHO].key)  //0x010F  Block ECHO  ("FX3")
                        {    //But it seems not to be sent out by THR
                            if(msgVals[3] == glob["TapeEcho"] ) 
                            {
                                result+=" UnitEcho: Mode TapeEcho selected ";
                            }
                            else if (msgVals[3] == glob["L6DigitalDelay"] )
                            {
                                result+=" UnitEcho: Mode DigitalDelay selected ";
                            }
                            else
                            {
                                result+=" Echo: unknown "; 
                            }
                        }
                        else
                        {
                              result+=(" UNIT: unknown Block selected  0x")+String(msgVals[2],HEX);
                        }
                    }//of OPCODE 0x03, Length=0x08
                    else
                    {
                        result+=(" OPCODE 3 but not expected LEN 8  0x"+String(msgVals[3],HEX));
                    }
                }
            break;  //of "message 16 Bytes"
            
            case 20:  //Answers to System Mess-Questions and Status Messages
                result+=(" 20-Byte-Message ");
                //msgVals: The Message: [0]:Opcode, [1]:Length, [2]:Block-Key/TargetType
                if (msgVals[0] == 1 && msgVals[1] == 0x0c)   //Answer to System-Question
                {
                    result+=(" (Answer) ");
                    int  id = outqueue.getHeadPtr()->_id;
                    switch (msgVals[3])
                    {
                        case 0x0002:
                            result+=" Answer to System Question (enum): 0x"+String(msgVals[4],HEX);
                            if (outqueue.itemCount() > 0)
                            {
                                if (id == 10) // only react, if it came from boot-up-question #S10 (number of active user setting)
                                {
                                    if (!userSettingsHaveChanged)
                                    {
                                        activeUserSetting = (int)msgVals[4];
                                    }
                                    outqueue.getHeadPtr()->_answered = true;
                                    result+=("\n\r Mark Question #"+String(id)+" (Nr. of active Preset) as answered." );
                                }
                                else if (id == 17)  //this reaction only, if it came from boot-up-question #S17 (G10T-state)
                                {//G10T Status Answer
                                    result+=( "(G10T) 3:0x"+String(msgVals[3],HEX)+" 4:0x"+String(msgVals[4],HEX));

                                    if (msgVals[4] == 0x02)
                                    {
                                        result+=("Inserted. ");
                                       //perhaps add representation in GUI here
                                    }
                                    if (msgVals[4] == 0x00)
                                    {
                                        result+=("Extracted. ");
                                       //perhaps add representation in GUI here
                                    }
                                    outqueue.getHeadPtr()->_answered = true;
                                    result+=(" Mark Question #"+String(id)+" (G10T-State) as answered.");

                                }
                                else if (id == 19)  //this reaction only, if it came from boot-up-question #S19 (Front-LED-State)
                                { //Front-LED Status Answer
                                    result+=("(Front-LED) 3:0x"+String(msgVals[3],HEX)+" 4:0x"+String( msgVals[4],HEX));

                                    if (msgVals[4] == 0x7f)
                                    {
                                        result+=(" is on.");
                                        //Light = true;  //Perhaps add state var "Light" in class for GUI
                                    }
                                    if (msgVals[4] == 0x00)
                                    {
                                        result+=(" is off.");
                                        //Light = false; //Perhaps add state var "Light" in class for GUI
                                    }
                                    outqueue.getHeadPtr()->_answered = true;
                                    result+=(" Mark Question #"+String(id)+" (Front-LED-State) as answered.");
                                }
                                else if (id == 23)  //this reaction only, if it came from boot-up-question #S23 (Speaker-Tuning-State)
                                {//Speaker-Tuner Status Answer
                                    result+=("(Speaker-Tuning) 3:0x"+String(msgVals[3],HEX)+" 4:0x"+String(msgVals[4],HEX));

                                    if (msgVals[4] == 0x01)
                                    {
                                        result+=(" Focus.");
                                        //SpeakerTuning = true;  //Perhaps add state var "SpeakerTuning" in class for GUI
                                    }
                                    if (msgVals[4] == 0x00)
                                    {
                                        result+=(" Open.");
                                        //SpeakerTuning = false; //Perhaps add state var "SpeakerTuning" in class for GUI
                                        
                                    }
                                    outqueue.getHeadPtr()->_answered = true;
                                    result+=(" Mark Question #"+String(id)+ " (Speaker-Tuning-State) as answered.");
                                }
                            }//of outqueue item count > 0
                            break; 
                        case 0x0003:
                            if (outqueue.item_count() > 0)
                            {
                                int id = outqueue.getHeadPtr()->_id;
                                result+=("(Tuner Enabled?) 3:0x"+String(msgVals[3],HEX)+ " 4:0x"+String(msgVals[4],HEX));
                                if (id == 21) //this reaction only, if it came from boot-up-question #S21 (Glob.Read:TunerEnabled)
                                {   //otherwise we get a 24-Byte glob.param change report, no 20-Byte answer!
                                    
                                    //THR: 									    #R19
                                    //f0 00 01 0c 24 02 4d 00 05 00 01 03 00 01 00 00 00 0c 00 00 00 00 | 00 00 00 00 03 00 00 00 00 00 00 00 00 | 00 f7    
                                    if (msgValsLength > 4)
                                    {
                                        if (msgVals[4] != 0)
                                        {
                                            //Perhaps show TunerState "ON" in GUI here
                                        }
                                        else
                                        {
                                            //Perhaps show TunerState "OFF" in GUI here
                                        }
                                    }
                                    outqueue.getHeadPtr()->_answered = true;
                                    result+=("\n\rMark Question #"+String(id)+ " (Tuner-State) as answered.");
                                }
                            }
                            else
                            {
                                result+="\n\rAnswer to Global Read (bool): "+String( (msgVals[4] != 0 ? "Yes" : "No") );
                            }
                            
                            break;
                        case 0x0004:
                            result+=("\n\rAnswer to System Question (int): "+String(NumberToVal(msgVals[4]) ));
                            if (outqueue.item_count() > 0)
                            {
                                int id = outqueue.getHeadPtr()->_id;
                                double val;
                                if(id == 25) // only react, if it came from boot-up-question #S25 (GuitarVolume)
                                {//GuitarVolume Answer
                                    result+=(" GUITAR-VOLUME ");
                                    val = NumberToVal(msgVals[4]);
                                    result+=String(val,1);
                                    //Perhaps show this in GUI
                                    outqueue.getHeadPtr()->_answered = true;
                                    result+=("\n\r Mark Question #"+String(id)+" (GuitarVolume) as answered.");
                                }
                                else if(id == 27) //only react, if it came from boot-up-question #S27 (AudioVolume)
                                {//AudioVolume Answer
                                    result+=(" AUDIO-VOLUME ");
                                    val = NumberToVal(msgVals[4]);
                                    result+=String(val,1);
                                    //Perhaps show this in GUI
                                    outqueue.getHeadPtr()->_answered = true;
                                    result+=("\n\rMark Question #"+String(id)+" (AudioVolume) as answered.");
                                }
                            }
                            break;
                        default:
                            result+=("\n\rAnswer to System Question (unknown): 0x"+String( msgVals[4],HEX) );
                            break;
                    }
                }
                //msgVals: The Message: [0]:Opcode, [1]:Length, [2]:Block-Key/TargetType
                if (msgVals[0] == 6 && msgVals[1] == 0x0c)   //Status Message
                {
                    result+=(" (Status) ");
                    if (msgVals[2] == 0x0001)  //Normal for the "Ready-Message"
                    {
                        switch (msgVals[3])
                        {
                            case 0x0002:
                                result+=("(enum): "+String((msgVals[4] == 0x0001 ? "ready" : "unknown")) );
                                break;
                            default:
                                result+=("(unknown type): 0x"+String( msgVals[4],HEX));
                                break;
                        }
                    }
                    else if(msgVals[2]==0x000B) //G10T Status Message
                    {
                        result+=("(G10T) 3:0x"+String(msgVals[3],HEX)+" 4:0x"+String(msgVals[4],HEX));
                        if (msgVals[4] == 0x02)
                        {
                            result+=(" Inserted. ");
                        }
                        if (msgVals[4] == 0x00)
                        {
                            result+=(" Extracted. ");
                        }
                    }
                    else
                    {
                        result+=("(unknown) 2:0x"+String( msgVals[2],HEX)+" 3:0x"+String( msgVals[3],HEX)+" 4:0x"+String(msgVals[4],HEX));
                    }
                }//Status message
            break; //of message 20 Bytes
            
            case 24: //Message for Parameter changes

                if (cur_len >= 0x1a)  // 00 01 07 - messages have to be longer than 0x2c bytes (failure otherwise)
                {
                    result+=(" 24-Byte-Message "); //Message for Parameter changes

                    double val = 0.0;
                    //msgVals: The Message: [0]:Opcode, [1]:Length, [2]:Block-Key/TargetType
                    if (msgVals[0] == 2 && msgVals[1] == 0x10)  //User-Setting change report
                    {
                            if(msgVals[3]<5)   //a user setting was changed on THR (pressed one of knobs "1"..."5")
                            {
                                result+=("\n\rUSER Setting "+String(msgVals[3] + 1)+" is activated. Following values: 0x"+String(msgVals[4],HEX)+" and 0x"+String(msgVals[5],HEX));
                                activeUserSetting = msgVals[3]>4?-1:(int)msgVals[3]; //if value would be ushort FFFF (actual setting) we have to cast to int -1
                                userSettingsHaveChanged =false;
                                //THR-Remote asks "have user settings changed?" in this case (why?) and requests actual settings, if they did not change.
                                //We request actual settings without asking !
                                
                                //Request actual user settings (Expect several frames - settings dump)
                                
                                //ZWE:->should be a different id than #8 here, because in this case the request does not come from settings dialogue
                                
                                outqueue.enqueue(Outmessage(SysExMessage((const byte[29]){ 0xf0, 0x00, 0x01, 0x0c, 0x24, 0x02, 0x4d, 0x01, 0x02, 0x00, 0x00, 0x0b, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x3c, 0x00, 0x7f, 0x7f, 0x7f, 0x7f, 0x00, 0x00, 0xf7 },29), 88, false, true)); //answer will be the settings dump for actual settings
                            }
                            else if(msgVals[3]== 0xFFFFFFFF)  //a user setting was dumped to the PC (followed by a request)
                            {
                                result+=("\n\rActual user setting was dumped to PC. Following values: 0x"+String(msgVals[4],HEX)+" and 0x"+String(msgVals[5],HEX));
                                if (outqueue.itemCount() > 0)
                                {
                                    int id = outqueue.getHeadPtr()->_id;
                                    if (id == 8) //if it was request #S8 from boot-up dialog ("request actual user settings")
                                    {
                                        outqueue.getHeadPtr()->_answered = true;
                                        _uistate=UI_init_act_set;  //State "initial actual settings" reached
                                    } 
                                    if (id == 88) //if it was request #88 from outside boot-up dialog ("request actual user settings")
                                    {
                                        outqueue.getHeadPtr()->_answered = true;
                                        //do not change _uistate, if settings dump is received outside init-sync
                                    } 
                                }
                            }
                            else
                            {
                                result+=("\n\rSelected invalid USER Setting "+String(msgVals[3] + 1));
                            }
                    }
                    else if (msgVals[0] == 4 && msgVals[1] == 0x10)  //Parameter change report
                    {
                        result+=String("\n\rParameter change report: ");
                        //select from   msgVals[2]  //Unit
                        if(msgVals[2] == THR30II_UNITS_VALS[GATE].key) //0x013C: //Block Mix/Gate (GuitarProc))
                        {
                            userSettingsHaveChanged = true;
                            activeUserSetting=-1;
                            //select from  msgVals[3]  (the value - here the parameter key)
                            if(msgVals[3] == THR30II_CAB_COMMAND)//0x0105:
                            {       uint16_t cab =(NumberToVal(msgVals[5]) / 100.0f);  //Values 0...16 come in as floats 
                                    result+=(" CAB: "+THR30II_CAB_NAMES[(THR30II_CAB) constrain(cab, 0x00, 0x10)]);
                                    SetCab((THR30II_CAB)cab);
                            }
                            else if(msgVals[3] == THR30II_INFO_PHAS[PH_MIX].sk) //0x010E:
                            {       result+=(" MIX (EFFECT) ");
                                    val = THR30II_Settings::NumberToVal(msgVals[5]);
                                    result+=String(val,0);
                                    EffectSetting(THR30II_INFO_EFFECT[effecttype]["MIX"].sk, val);
                            }
                            else if(msgVals[3] == THR30II_INFO_TAPE[TA_MIX].sk)  //0x0111:
                            {       result+=(" MIX (ECHO) ");
                                    val = THR30II_Settings::NumberToVal(msgVals[5]);
                                    result+=String(val,0);
                                    EchoSetting(THR30II_INFO_ECHO[echotype]["MIX"].sk, val);
                            }
                            else if(msgVals[3] == THR30II_INFO_SPRI[SP_MIX].sk)//0x0128:
                            {       result+=(" MIX (REVERB) ");
                                    val = THR30II_Settings::NumberToVal(msgVals[5]);
                                    result+=String(val,0);
                                    ReverbSetting(THR30II_INFO_REVERB[reverbtype]["MIX"].sk, val);
                            }

                            else if(msgVals[3]== THR30II_UNIT_ON_OFF_COMMANDS[GATE]) //0x0102:
                            {       result+=(" UNIT GATE "+String((msgVals[5] != 0 ? "On" : "Off")));
                                    Switch_On_Off_Gate_Unit(msgVals[5] != 0);
                            }
                            else if(msgVals[3] == THR30II_UNIT_ON_OFF_COMMANDS[ECHO]) //0x012C:
                            {       result+=(" UNIT ECHO "+String((msgVals[5] != 0 ? "On" : "Off")));
                                    Switch_On_Off_Echo_Unit(msgVals[5] != 0);
                            }
                            else if(msgVals[3]== THR30II_UNIT_ON_OFF_COMMANDS[EFFECT]) //0x012D:
                            {       result+=(" UNIT EFFECT "+String((msgVals[5] != 0 ? "On" : "Off")));
                                    Switch_On_Off_Effect_Unit(msgVals[5] != 0);
                            }
                            else if(msgVals[3]== THR30II_UNIT_ON_OFF_COMMANDS[COMPRESSOR]) //0x012E:
                            {        result+=(" UNIT COMPRESSOR "+String((msgVals[5] != 0 ? "On" : "Off")));
                                    Switch_On_Off_Compressor_Unit(msgVals[5] != 0);
                            }
                            else if(msgVals[3] == THR30II_UNIT_ON_OFF_COMMANDS[REVERB]) //0x0130:
                            {       result+=(" UNIT REVERB "+String((msgVals[5] != 0 ? "On" : "Off")));
                                    Switch_On_Off_Reverb_Unit(msgVals[5] != 0);
                            }

                            else if(msgVals[3]== THR30II_GATE_VALS[GA_THRESHOLD]) //0x0069:
                            {       result+=(" GATE-THRESHOLD ");
                                    val = THR30II_Settings::NumberToVal_Threshold(msgVals[5]);
                                    result+=String(val,0);
                                    GateSetting(GA_THRESHOLD, val);
                            }
                            else if( msgVals[3] == THR30II_GATE_VALS[GA_DECAY]) //0x006B:
                            {       result+=(" GATE-DECAY ");
                                    val = THR30II_Settings::NumberToVal(msgVals[5]);
                                    result+=String(val,0);
                                    GateSetting(GA_DECAY, val);
                            }
                            else
                            {      
                                 result+=("\n\runknown "+String(msgVals[3],HEX)+" in Block Mix/Gate");
                            }
                        }
                        else if(msgVals[2] == THR30II_UNITS_VALS[REVERB].key ) //0x0112:  //Block Reverb
                        {
                            //result+=String("Block Reverb: ");
                            userSettingsHaveChanged = true;
                            activeUserSetting = -1;
                            //select from  msgVals[3]  (the value - here the parameter key)
                            if(msgVals[3] == THR30II_INFO_PLAT[PL_DECAY].sk) //0x006B:
                            {    result+=(" REVERB-DECAY ");
                                val = THR30II_Settings::NumberToVal(msgVals[5]);
                                result+=String(val,0);
                                ReverbSetting(reverbtype, THR30II_INFO_PLAT[PL_DECAY].sk, val); //0x006B, val);
                            }   
                            else if( msgVals[3] == THR30II_INFO_SPRI[SP_REVERB].sk) //0x00ED:
                            {    result+=(" REVERB-REVERB ");
                                val = THR30II_Settings::NumberToVal(msgVals[5]);
                                result+=String(val,0);
                                ReverbSetting(reverbtype, THR30II_INFO_SPRI[SP_REVERB].sk, val);//0x00ED, val);
                            }
                            else if(msgVals[3] == THR30II_INFO_PLAT[PL_TONE].sk) //0x00F4:
                            {    result+=(" REVERB-TONE ");
                                val = THR30II_Settings::NumberToVal(msgVals[5]);
                                result+=String(val,0);
                                ReverbSetting(reverbtype, THR30II_INFO_PLAT[PL_TONE].sk, val);//0x00F4, val);
                            }
                            else if(msgVals[3] == THR30II_INFO_PLAT[PL_PREDELAY].sk) //0x00FA:
                            {    result+=(" REVERB-PRE DELAY ");
                                val = THR30II_Settings::NumberToVal(msgVals[5]);
                                result+=String(val,0);
                                ReverbSetting(reverbtype, THR30II_INFO_PLAT[PL_PREDELAY].sk, val);// 0x00FA, val);
                            }
                            else
                            {
                                    result+=(" REVERB Unknown: "+String(msgVals[5],HEX));
                            }
                        }//of Block Reverb
                        else if(msgVals[2] == THR30II_UNITS_VALS[CONTROL].key) //0x010a:   //Block Amp
                        {
                            //result+=String("Amp: ");
                            userSettingsHaveChanged = true;
                            activeUserSetting = -1;
                            //select from  msgVals[3]  (the value - here the parameter key)

                            if(msgVals[3]  == THR30II_CTRL_VALS[CTRL_GAIN]) //0x58:
                            {   result+=(" GAIN ");
                                val = THR30II_Settings::NumberToVal(msgVals[5]);
                                SetControl(THR30II_CTRL_SET::CTRL_GAIN, val);
                                result+=String(val,0);
                            }
                            else if(msgVals[3]  == THR30II_CTRL_VALS[CTRL_MASTER]) //0x4C:
                            {   result+=(" MASTER ");
                                val = THR30II_Settings::NumberToVal(msgVals[5]);
                                SetControl(CTRL_MASTER, val);
                                result+=String(val,0);
                            }
                            else if(msgVals[3]  == THR30II_CTRL_VALS[CTRL_BASS]) //0x54:
                            {   result+=(" TONE-BASS ");
                                val = THR30II_Settings::NumberToVal(msgVals[5]);
                                result+=String(val,0);
                                SetControl(CTRL_BASS, val);
                            }
                            else if(msgVals[3]  == THR30II_CTRL_VALS[CTRL_MID]) //0x56:
                            {   result+=(" TONE-MID ");
                                val = THR30II_Settings::NumberToVal(msgVals[5]);
                                SetControl(CTRL_MID, val);
                                result+=String(val,0);
                            }
                            else if(msgVals[3]  == THR30II_CTRL_VALS[CTRL_TREBLE]) //0x57:
                            {   result+=(" TONE-TREBLE ");
                                val = THR30II_Settings::NumberToVal(msgVals[5]);
                                SetControl(CTRL_TREBLE, val);
                                result+=String(val,0);
                            }
                            else
                            {
                                result+=(" AMP Unknown "+String(msgVals[5],HEX));
                            }
                        }//of Block Amp
                        else if( msgVals[2]== THR30II_UNITS_VALS[EFFECT].key) //0x010c: //Block Effect
                        {
                            //result+=String("Block Effect: ");
                            userSettingsHaveChanged=true;
                            activeUserSetting = -1;
                            //select from  msgVals[3]  (the value - here the parameter key)
                            if(msgVals[3]  == THR30II_INFO_EFFECT[PHASER]["SPEED"].sk) //0x00D4:
                            {   result+=(" EFFECT SPEED (Phaser/Tremolo) ");
                                val = THR30II_Settings::NumberToVal(msgVals[5]);
                                result+=String(val,0);
                                EffectSetting(effecttype, msgVals[3], val);
                            }
                            else if(msgVals[3]  == THR30II_INFO_EFFECT[PHASER]["FEEDBACK"].sk) //0x00D6:
                            {   result+=(" EFFECT FEEDBACK ");
                                val = THR30II_Settings::NumberToVal(msgVals[5]);
                                result+=String(val,0);
                                EffectSetting(effecttype, msgVals[3], val);
                            }
                            else if(msgVals[3]  == THR30II_INFO_EFFECT[CHORUS]["DEPTH"].sk) //0x00E0:
                            {   result+=(" EFFECT DEPTH (Flanger/Chorus/Tremolo) ");
                                val = THR30II_Settings::NumberToVal(msgVals[5]);
                                result+=String(val,0);
                                EffectSetting(effecttype, msgVals[3], val);
                            }
                            else if(msgVals[3]  == THR30II_INFO_EFFECT[FLANGER]["SPEED"].sk) //0x00E4:
                            {   result+=(" EFFECT SPEED (Flanger / Chorus) ");
                                val = THR30II_Settings::NumberToVal(msgVals[5]);
                                result+=String(val,0);
                                EffectSetting(effecttype, msgVals[3], val);
                            }
                            else if(msgVals[3]  == THR30II_INFO_EFFECT[CHORUS]["PREDELAY"].sk) //0x00E8:
                            {   result+=(" EFFECT PRE-DELAY ");
                                val = THR30II_Settings::NumberToVal(msgVals[5]);
                                result+=String(val,0);
                                EffectSetting(effecttype, msgVals[3], val);
                            }
                            else
                            {
                                result+=(" EFFECT Unknown: "+String(msgVals[3],HEX));
                            }
                        }//of Block Effect
                        else if( msgVals[2]== THR30II_UNITS_VALS[ECHO].key) //0x010f:  //Block Echo
                        {
                            // result+=String("Block Mix/Gate: ");
                            userSettingsHaveChanged=true;
                            activeUserSetting = -1;
                            //select from  msgVals[3]  (the value - here the parameter key)
                            
                            if(msgVals[3]  == THR30II_INFO_ECHO[TAPE_ECHO]["BASS"].sk) //0x0054:
                            {   result+=(" ECHO-BASS ");
                                val = THR30II_Settings::NumberToVal(msgVals[5]);
                                result+=String(val,0);
                                EchoSetting(echotype,msgVals[3], val);
                            }
                            else if(msgVals[3]  == THR30II_INFO_ECHO[TAPE_ECHO]["TREBLE"].sk) //0x0057:
                            {   result+=(" ECHO-TREBLE ");
                                val = THR30II_Settings::NumberToVal(msgVals[5]);
                                result+=String(val,0);
                                EchoSetting(echotype,msgVals[3], val);
                            }
                            else if(msgVals[3]  == THR30II_INFO_ECHO[TAPE_ECHO]["FEEDBACK"].sk) //0x00D6:
                            {   result+=(" Echo FEEDBACK ");
                                val = THR30II_Settings::NumberToVal(msgVals[5]);
                                result+=String(val,0);
                                EchoSetting(echotype,msgVals[3], val);
                            }
                            else if(msgVals[3]  == THR30II_INFO_ECHO[TAPE_ECHO]["TIME"].sk) //0x00ED:
                            {   result+=(" ECHO-TIME ");
                                val = THR30II_Settings::NumberToVal(msgVals[5]);
                                result+=String(val,0);
                                EchoSetting(echotype,msgVals[3], val);
                            }
                            else
                            {
                                result+=(" ECHO Unknown: "+String(msgVals[5],HEX));
                            }
                            
                        }//of "Block Echo"
                        else if( msgVals[2]== 0xFFFFFFFF) //Global Parameter Settings) 
                        {
                            //select from  msgVals[3]  (the value - here the Extended setting's key)
                            
                            if(msgVals[3] == glob["GuitInputGain"]) //0x0147:
                            {
                                result+=(" Guitar Input Gain ");
                                val = NumberToVal(msgVals[5]);
                                result+=(String(val,0));
                                if(val>99.0)
                                {
                                    //Perhaps show this in GUI "GuitarMute Off"
                                }
                                else
                                {
                                    //Perhaps show this in GUI "GuitarMute On"
                                }
        
                            }
                            else if(msgVals[3] == glob["TunerEnable"] )
                            {
                                result+=(" Tuner Enable ");
                                val = NumberToVal(msgVals[5]);
                                result+=(String(val,0));
                                if(val>99.0)
                                {
                                    //Perhaps show this in GUI "Tuner On"
                                }
                                else
                                {
                                    //Perhaps show this in GUI "Tuner Off"
                                }
                            }
                            else if(msgVals[3]  == glob["GuitarVolume"]) //0x0155:
                            {
                                result+=(" Guitar Volume ");
                                val = NumberToVal(msgVals[5]);
                                result+=(String(val,0));
                                //Perhaps show this in GUI
                            }
                            else if(msgVals[3]  == glob["AudioVolume"])
                            {
                                result+=(" Audio Volume ");
                                val = NumberToVal(msgVals[5]);
                                result+=(String(val,0));
                                //Perhaps show this in GUI
                            }
                            else if(msgVals[3]  == glob["GuitProcInputGain"])
                            {
                                result+=(" GuitProcInputGain ");
                                val = NumberToVal(msgVals[5]);
                                result+=(String(val,0));
                                //Perhaps show this in GUI
                            }
                            else if(msgVals[3]  == glob["GuitProcOutputGain"])
                            {
                                result+=(" GuitProcOutputGain ");
                                val = NumberToVal(msgVals[5]);
                                result+=(String(val,0));
                                //Perhaps show this in GUI
                            }
                            else
                            {
                                 result+=(" Global Parameter Settings Unknown: 0x"+String(msgVals[3],HEX)+" 0x"+String(msgVals[5],HEX));
                            }
                        }
                    } //end of if  0x04 0x10 - message
                } //von if cur_len >=0x1a
            break; //of message 24 Bytes                          

            default:  //messagelength other than 9, 12, 16, 20, 24 bytes (dumps)

                if (memcmp(cur, THR30II_IDENTIFY,10)==0)  //is it the identification string?  (not a regular THR-frame)
                {
                    //we reacted to this this already above!!!
                }
                else //Not the identification string
                {
                    if (payloadSize <= 0x100)   //framelen other than 9, 12,16,20, 24 Bytes but shorter than 0x100Bytes (will be a dump)  
                    {
                        if (sameFrameCounter == 0)  //should be the first frame of the dump, in this case
                        {
                            result+=("\n\rChunk 1 of a dump : Size of first chunk's payload= "+String(payloadSize - 1,HEX));
                            //int  id = outqueue.getHeadPtr()->_id;

                            if (msgVals[0] == 0x01 && msgVals[2] == 0x00 && msgVals[3] == 0x00 && msgVals[4] == 0x01) //patch dump
                            {
                                result+=(".. it is a Patch-Dump. ");
                                patchdump = true;
                                symboldump = false;
                                dumpInProgress = true;
                                dumpFrameNumber = 0;
                                dumpByteCount = 0;

                                dumplen = msgVals[1];  //total lenght (including the 4  32-Bit-values 0 0 1 0 )
                                result+=("\n\rTotal patch length: "+String(dumplen));
                                dump = new byte[dumplen - 16];
                                dump_len = dumplen-16;  //netto patch lenght without the 4 leading  32-Bit-values 0 0 1 0
                               
                                if(dump==nullptr)
                                {
                                    Serial.println("\n\rAllocation Error for patch dump!");
                                }

                                if (dumplen <= payloadSize - 8) //complete patch fits in this one message
                                {
                                    dumpInProgress = false;  //because first frame is last frame
                                    patchdump = false;
                                    //perhaps this (very short) patch dump was requested, if an outmessage is pending unanswered then release it
                                    int id=-1;
                                    if (outqueue.item_count() > 0 && outqueue.getHeadPtr()->_needs_answer) 
                                    {
                                       id = outqueue.getHeadPtr()->_id;
                                       outqueue.getHeadPtr()->_answered = true;
                                    }
                                    result+=("\n\rDump finished with Chunk 1. Request #"+String(id)+" is answered");
                                }
                            }
                            else if (msgVals[0] == 0x01 && msgVals[2] == 0x00)  //Patch name dump
                            {
                                result+=(" .. it is a Patch-Name-Dump. ");
                                dumpInProgress = false;  //patch name dump always fits to one frame!
                                patchdump = false;
                                symboldump = false;
                                dumpFrameNumber = 0;
                                dumpByteCount = 0;
                                dumplen = msgVals[1];
                                result+=(" Total patch length: "+String(dumplen));
                                dump = nullptr;
                                //no need to reserve a dump buffer "dump[]" , because we get all data instantly

                                uint32_t len = msgVals[3];
                                
                                if (dumplen >= len + 8)  //must be true for valid patchname message
                                {
                                    char bu[65];
                                    memcpy(bu,msgbytes+16,len);
                                    String patchname= String(bu);
                                    result+=String(" Answer: Patch name is \"")+ patchname+String("\" ");
                                    
                                    //perhaps this one frame patch-name dump was requested; If a outmessage is pending unanswered then release it
                                    int id=-1;
                                    byte pnr = 0; //User-Preset number to which this name belongs
                                    if (outqueue.item_count() > 0 && outqueue.getHeadPtr()->_needs_answer)
                                    {
                                        id=outqueue.getHeadPtr()->_id;
                                        outqueue.getHeadPtr()->_answered = true;
                                        pnr=outqueue.getHeadPtr()->_msg.getData()[0x16];  //fetch user preset nr.
                                        result+="\n\rpnr (raw)= "+String(pnr) + String("\n\r");
                                        if (pnr > 4)  //for "actual" byte [0x16] will be 0x7F
                                        {
                                            pnr = 0;  //0 is "actual"
                                        }
                                        else
                                        {
                                            pnr++;
                                        }
                                        result+=(" Dump finished with Chunk 1.\n\rRequest #"+String(id)+" is answered.\n\r");
                                    }
                                    SetPatchName(patchname, (int)pnr -1 );
                                }
                            }
                            else if ((msgVals[0] == 0x01) && (msgVals[2] != 0x00) && (writeOrrequest == 0x00))  //Symbol table
                            {
                                result+=" It is a symbol table dump. \r\n";
                                patchdump = false;
                                symboldump = true;
                                dumpInProgress = true;
                                dumpFrameNumber = 0;
                                dumpByteCount = 0;
                                dumplen = msgVals[1];  //Length of patch in bytes
                                uint32_t symCount = msgVals[2]; //Number of sybols in this table
                                uint32_t len = msgVals[3];  //In case of the symbol table it should be the same as dumplen 
                                result+=String(symCount)+" symbols in a total patch length of "+String(dumplen)+String(" bytes.\r\n");

                                dump = new byte[dumplen];
                                dump_len=dumplen;
                                if(dump==nullptr)
                                {
                                    Serial.println("\n\rAllocation Error for symbol dump!");
                                }

                                if ((dumplen == len) && (dumplen > 0xFF))  //a symbol table dump is very long!
                                {
                                    result+=" ...seems to be valid\r\n";
                                }
                            }
                            else
                            {
                                result+=" dump has different values in header than expected!! "+String(msgVals[0],HEX)+ String(msgVals[2],HEX) + String(msgVals[3],HEX) + String(msgVals[4],HEX) ;
                            }

                        }  //end of "if first frame of a dump" (sameframeCounter ==0)
                        else if (dumpInProgress && (sameFrameCounter == dumpFrameNumber + 1))  //found next frame of a dump
                        {
                            dumpFrameNumber++;
                            //Serial.println(String(" Chunk "+String(dumpFrameNumber + 1))+String(" of a dump: Size of this chunk's payload= ")+String(payloadSize - 1,HEX));
                            result+=" Chunk "+String(dumpFrameNumber + 1)+" of a dump: Size of this chunk's payload= "+String(payloadSize - 1,HEX);
                        }

                        //for any dump type (except patch-name) and for any of it's chunks we have to copy the content
                        if (dumpInProgress && (dump != nullptr))  
                        {
                            uint16_t i = 0;
                            if (dumpFrameNumber == 0)
                            {
                                if (patchdump)
                                {
                                    i = 6 * 4;  //skip 6 values in first frame 
                                }
                                else if (symboldump)
                                {
                                    i = 2 * 4; //skip 4 values in first frame
                                }
                            }
                            
                            for (; i < payloadSize; i++) //msgbytes.Length
                            {
                                dump[dumpByteCount++] = msgbytes[i];
                            }
                        }

                        if (dumpInProgress && (dumpByteCount >= dump_len)) //fetched enough bytes for complete dump
                        {
                            result+=" Dump finished in chunk "+String(dumpFrameNumber + 1);

                            if (patchdump)
                            {
                                result+="\n\rDoing Patch Set All:\n\r";
                                _state = States::St_idle;
                                //Set constans from symbol table
                                patch_setAll(dump,dump_len); //using dump_len, that does not include the 4  32-Bit-values 0 0 1 0 )
                            }
                            else if (symboldump)
                            {
                                // byte test[98]={
                                //              0x03,0x00,0x00,0x00, 
                                //              0x62,0x00,0x00,0x00,
                                //              0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,
                                //              0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,
                                //              0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,
                                //              0x50,0x4F,0x75,0x74,0x70,0x75,0x74,0x4C,0x69,0x6D,0x69,0x74,0x65,0x72,0x54,0x75,0x62,0x65,0x47,0x61,0x69,0x6E,0x00,
                                //              0x57,0x69,0x64,0x65,0x53,0x74,0x65,0x72,0x65,0x6F,0x57,0x69,0x64,0x74,0x68,0x00,
                                //              0x47,0x75,0x69,0x74,0x61,0x72,0x44,0x49,0x45,0x6E,0x61,0x62,0x6C,0x65,0x00
                                //              };
                                Constants::set_all(dump);  //Now we can get the right keys for this firmware from the table
                                
                                Init_Dictionaries();  //Now use constants in this App's dictionaries
                                
                                Serial.println("\n\rDictionaries initialized:");

                                int id = 0;
                                if (outqueue.itemCount() > 0)
                                {
                                    id = outqueue.getHeadPtr()->_id;
                                    if (id == 777) //if this was the answer to the "Request symbol table from the boot up"
                                    {
                                        outqueue.getHeadPtr()->_answered = true; //this request is now answered
                                        result+=" Request 777 answered.";
                                    }
                                }

                                //#S6 (05-Message "01"-version") Request unknown reason (Answ. always the same: expext 0x00000080)                                            
                                //answer will be a number (seems to be always 0x80)
                                
                                outqueue.enqueue(Outmessage(SysExMessage((const byte[29]) { 0xf0, 0x00, 0x01, 0x0c, 0x22, 0x02, 0x4d, 0x01, 0x01, 0x00, 0x00, 0x07, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf7 },29), 6, false, true));

                                //#S7  (0F-Message)   Ask: Have User-Settings changed?
                                //answer will we changed (1) or not changed(0)
                                //after that we should react dependent of the answer: If settings have changed: We need the actual settings
                                //and the five user presets
                                //If settings have not changed (a User-Preset is active and not modified):
                                //We only need the 5 User Presets
                                outqueue.enqueue(Outmessage(SysExMessage( (const byte[29]) { 0xf0, 0x00, 0x01, 0x0c, 0x22, 0x02, 0x4d, 0x00, 0x03, 0x00, 0x00, 0x07, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf7 },29), 7, false, true));

                            } //end of "is SymbolDump"
                            
                            dumpInProgress = false;  
                            patchdump = false;
                            symboldump = false;

                            delete[] dump;

                        }//end of "fetched enough bytes for complete dump"
                    } //end of "if it seems to be a dump (  24 < len < 0x100  ) 
                    else //No dump
                    {
                        dumpInProgress = false;
                        patchdump = false;
                        symboldump = false;
                        delete[] dump;
                        dumpFrameNumber = 0;
                        result+=" Unknown message payload size 0x"+String(payloadSize - 1,HEX)+"\r\n";
                    }
                }  //Not the identification string (but a dump or unknown long message)
                
            break; // of "message other than 9, 12, 16, 20 or 24 bytes (e.g. a patch)"
        } //switch message payload size

    } //regular THR30II - SysEx beginning with 01 0c 24
    else
    {
        if (memcmp(cur, THR30II_RESP_TO_UNIV_SYSEX_REQ,9)==0)  //0xF0, 0x7E, 0x7F, 0x06, 0x02, 0x00, 0x01, 0x0c, 0x24 (+, 0x00, 0x02, 0x00, 0x63, 0x00, 0x1E, 0x01, 0xF7)
        {
            uint16_t famID =0;
            if(cur_len >9) famID=(cur[8] + 256 * cur[9]);
            uint16_t modNr = 0;
            if(cur_len>11) modNr=cur[10] + 256 * cur[11];
            ConnectedModel = (famID << 16) + modNr;
            result+=" Reply to Universal SysEx-Request: Family-ID "+String(famID,HEX)+", Model-Nr: "+String(modNr,HEX);
            outqueue.getHeadPtr()->_acknowledged = true;
        }
        else
        {
            result+=" Other SysEx: {";

            for (int i =0; i< cur_len; i++)
            {
                result+=cur[i]<16?String(0)+String(cur[i],HEX):String(cur[i],HEX)+String(" ");
            }
            result+="}";
        }
    }

    sendChangestoTHR = true;  //end of section, where changes are not send (back) to THR
    
    return result;
} //Parse SysEx (THR30II)
//                                             bool cut = true  (preset parameter)
double THR30II_Settings::NumberToVal(uint32_t num, bool cut ) //convert the 32Bit parameter value to 0..100 Slider value 
{
    float val;
    
    if (num >= 998277251)  //=0.00392156979069  , cut hard to 0 if too small
    {
        memcpy(&val,&num,4);
        //val = *((float*) &num);
        val = 100.0*val;
    }
    else
    {
        val = 0;
    }
    return cut ? floor(val) : val;   //cut decimals, if requested (default)
}
//                                      bool cut = true  (preset parameter)
double THR30II_Settings::NumberToVal_Threshold(uint32_t num, bool cut ) //convert the 32Bit parameter value to a 0..100 Slider value 
{  
    float val;
    //0xC2C00000 read as float -> -96
    //0xBE500000 read as float -> -0.203125
    if (num >= 0xBE500000 && num < 0xC2C00000)
    {
        //val= *((float*) &num);
        memcpy(&val,&num,4);
        val = 100.0 + 100.0/96 * val;   //change range from [-96dB ... 0dB] to [0...100]
    }
    else if (num >= 0xC2C00000)
    {
        val = 0.0;
    }
    else
    {
        val = 100.0;
    }

    return cut ? floor(val) : val; //cut decimals, if requested (default)
}

uint32_t THR30II_Settings::ValToNumber(double val) //convert a 0..100 Slider value to a 32Bit parameter value 
{
    uint32_t number;
    float v= val/(double)100.0;

    memcpy(&number,&v,4);
    //number= *((uint32_t*) &v);
            
    if (number < 999277251)
    {
        number = 0u;
    }

    return number;
}

uint32_t THR30II_Settings::ValToNumber_Threshold(double val) //convert a 0..100 Slider value to a 32Bit parameter value (for internal -96dB to 0dB range)
{   
    uint32_t number;
    //val 0   -> -96dB
    //val 100 ->   0dB
    //-96       float read as uint32_t -> 0xC2C00000  
    //-0.203125 float read as uint32_t -> 0xBE500000 
    
    float v=-(100.0-val) * 96.0 / 100.0 + 0.4; //change range from [0...100] to [-96dB ... 0dB]           

    memcpy(&number,&v,4);
    //number = *((uint32_t*) &v) ;

    //Limit number to allowed range

    if (number < 0xBE500000)
    {
        number = 0u;
    }
    if (number > 0xC2C00000)
    {
        number = 0xC2C00000;
    }

    return number;
}