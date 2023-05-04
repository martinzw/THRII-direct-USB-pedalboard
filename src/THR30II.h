/*THR30II Pedal using Teensy 3.6
* Martin Zwerschke 04/2021
*/

/*
 * THR30II.h
 *
 * Last modified: 5. May 2023 
 *  Author: Martin Zwerschke
 *
 */ 

#include <Arduino.h>
#include "Globals.h"

#undef max     //we need another version of max / min, not this one
#undef min

#include <map>
#include <vector>
#include <array>
#include <ArduinoJson.h>   //For patches stored in JSON (.thrl6p) format
#include <ArduinoQueue.h>  //For message queuing in and out

#ifndef THR30_H_
#define THR30_H_

//MIDI SYSEX data demarcation
#define SYSEX_START 0xf0
#define SYSEX_STOP  0xf7

//Yamaha MIDI Manufacturer SysEx ID via https://www.midi.org/specifications/item/manufacturer-id-numbers
//Only for old THR-series
#define YAMAHA_MIDI_ID 0x43
#define YAMAHA_DEVICE_ID 0x7d

//Yamaha THR model identifiers https://www.yamaha.com/thr/
//used in "Heartbeat" SYSEX
//Only for old THR-series
#define YAMAHA_THR5  0x30
#define YAMAHA_THR10  0x31
#define YAMAHA_THR10X  0x32
#define YAMAHA_THR10C  0x33
#define YAMAHA_THR5A  0x34

//Line6 (Yamaha) MIDI Manufacturer SysEx ID via https://www.midi.org/specifications/item/manufacturer-id-numbers
//Normal Manufacturer IDs are onyl 1 Byte, but the 0x00 says: the next 2 bytes are the ID
//For new THRII series
const byte LINE6_YAMAHA_MIDI_ID[]   { 0x00, 0x01, 0x0C };
const byte LINE6_YAMAHA_DEVICE_ID[] { 0x02, 0x4D };

//Line6/Yamaha MIDI SYSEX data header (from THR30II to PC)
//For new THRII series
const byte THR30II_SYSEX_BEGIN[]  { SYSEX_START, LINE6_YAMAHA_MIDI_ID[0], LINE6_YAMAHA_MIDI_ID[1], LINE6_YAMAHA_MIDI_ID[2], 0x24,
														LINE6_YAMAHA_DEVICE_ID[0],LINE6_YAMAHA_DEVICE_ID[1]};
//Line6/Yamaha MIDI SYSEX data header (from PC to THR30II, used to be different, now the same as in the other direction)
const std::array<byte,7> PC_SYSEX_BEGIN { SYSEX_START, LINE6_YAMAHA_MIDI_ID[0], LINE6_YAMAHA_MIDI_ID[1], LINE6_YAMAHA_MIDI_ID[2], 0x24,
														LINE6_YAMAHA_DEVICE_ID[0],LINE6_YAMAHA_DEVICE_ID[1]};

const byte UNIVERSAL_SYSEX_REQUEST [] { 0xF0, 0x7E, 0x7F, 0x06, 0x01, 0xF7 };   //to ask for Model and firmware version
//1st answer to univ. inquiry
const byte THR30II_RESP_TO_UNIV_SYSEX_REQ [] { 0xF0, 0x7E, 0x7F, 0x06, 0x02, 0x00, 0x01, 0x0c, 0x24 }; //, 0x00, 0x02, 0x00, 0x63, 0x00, 0x1E, 0x01, 0xF7 };
//2nd answer to univ. inquiry
const byte THR30II_IDENTIFY [] { 0xF0, 0x00, 0x01, 0x0C, 0x24, 0x02, 0x7E, 0x7F, 0x06, 0x02 };


//Effect units (global) 
enum THR30II_UNITS { COMPRESSOR, CONTROL, EFFECT, ECHO, REVERB, GATE };

extern std::map<THR30II_UNITS, uint16_t> THR30II_UNIT_ON_OFF_COMMANDS;

//CAB-Simulation is Sub-Unit of GATE 
extern uint16_t THR30II_CAB_COMMAND;// SpkSimType
extern uint16_t THR30II_CAB_COMMAND_DUMP;// SpkSimTypeState

//Simulation collections 
enum THR30II_COL { CLASSIC = 0, BOUTIQUE, MODERN };

//AMP simulation models
enum THR30II_AMP { CLEAN = 0, CRUNCH, LEAD, HI_GAIN, SPECIAL, BASS, ACO, FLAT };

//The constant amplifier names
extern std::map<THR30II_AMP, String> THR30II_AMP_NAMES;
//The constant collection names
extern std::map<THR30II_COL, String> THR30II_COL_NAMES;

//Cabinet Simulation models
enum THR30II_CAB
{
	British_4x12 = 0, American_4x12 = 1, Brown_4x12 = 2, Vintage_4x12 = 3, Fuel_4x12 = 4, Juicy_4x12 = 5, Mods_4x12 = 6, American_2x12 = 7, British_2x12 = 8,
	British_Blues = 9, Boutique_2x12 = 0x0a, Yamaha_2x12 = 0x0b, California_1x12 = 0x0c, American_1x12 = 0x0d, American_4x10 = 0x0e, Boutique_1x12 = 0x0f,
	Bypass = 0x10
};

//Effect Unit Types, Parameters
enum THR30II_EFF_TYPES { PHASER, TREMOLO, FLANGER, CHORUS };

enum THR30II_EFF_SET_PHAS { PH_SPEED, PH_FEEDBACK, PH_MIX };
enum THR30II_EFF_SET_TREM { TR_SPEED, TR_DEPTH, TR_MIX };
enum THR30II_EFF_SET_FLAN { FL_DEPTH, FL_SPEED, FL_MIX };
enum THR30II_EFF_SET_CHOR { CH_FEEDBACK, CH_DEPTH, CH_SPEED, CH_PREDELAY, CH_MIX, CH_SYNCSELECT };

//Reverb Unit Types, Parameters
enum THR30II_REV_TYPES { SPRING, PLATE, HALL, ROOM };

enum THR30II_REV_SET_SPRI { SP_REVERB, SP_TONE, SP_MIX };
enum THR30II_REV_SET_PLAT { PL_DECAY, PL_TONE, PL_PREDELAY, PL_MIX };
enum THR30II_REV_SET_HALL { HA_DECAY, HA_TONE, HA_PREDELAY, HA_MIX };
enum THR30II_REV_SET_ROOM { RO_DECAY, RO_TONE, RO_PREDELAY, RO_MIX };

//Echo Unit Types, Parameters
enum THR30II_ECHO_TYPES { TAPE_ECHO, DIGITAL_DELAY };   //NEW for 1.40.0a

enum THR30II_ECHO_SET_TAPE { TA_BASS, TA_TREBLE, TA_FEEDBACK, TA_TIME, TA_MIX, TA_SYNCSELECT }; //NEW for 1.40.0a
enum THR30II_ECHO_SET_DIGI { DD_BASS, DD_TREBLE, DD_FEEDBACK, DD_TIME, DD_MIX, DD_SYNCSELECT }; //NEW for 1.40.0a

//Mapping the MIDI-dump keys to the corresponding single command MIDI keys
extern std::map<uint16_t, uint16_t> unitOnMap;
extern std::map<uint16_t, uint16_t> controlMap;

//THR30II manual control knobs settings
enum THR30II_CTRL_SET { CTRL_GAIN, CTRL_MASTER, CTRL_BASS, CTRL_MID, CTRL_TREBLE };

//Main Controls
extern std::map<THR30II_CTRL_SET, uint16_t> THR30II_CTRL_VALS;

//---------GATE--------------
enum THR30II_GATE { GA_THRESHOLD, GA_DECAY };
extern std::map<THR30II_GATE, uint16_t> THR30II_GATE_VALS;
extern std::map<uint16_t, uint16_t> gateMap;

//--------COMPRESSOR---------------
enum THR30II_COMP { CO_SUSTAIN, CO_LEVEL, CO_MIX };   //MIX not in THR-Remote (value always = 0x3F000000)
extern std::map<THR30II_COMP, uint16_t> THR30II_COMP_VALS;
extern std::map<uint16_t,uint16_t> compressorMap;

//structures 
struct un_cmd
{
	uint16_t unit;
 	uint16_t command;
};

// struct un_par
// {
// 	uint16_t unit;
//  	uint16_t command;
// };

template <typename T>
struct type_val
{
  byte type;  //the type marker inside THRII-MIDI patches
  T val;
};

//Structure to describe a combination of simulation collection and amp simulation (e.g. Boutique Crunch)
struct col_amp
{
  THR30II_COL c;
  THR30II_AMP a;
  inline col_amp() {};
  inline col_amp(THR30II_COL co, THR30II_AMP am):c(co),a(am) {}
  inline col_amp(col_amp & ca):c(ca.c),a(ca.a) {}
  inline col_amp(const col_amp & ca):c(ca.c),a(ca.a) {}
  
  inline bool operator==(col_amp & other){return c==other.c && a==other.a;}  
  inline bool operator < (col_amp & other){return  ((c<<8) + a )<( (other.c<<8) + other.a);}  
  inline col_amp & operator=(const col_amp & other){ c=other.c; a=other.a; return *this;} 
};

inline bool operator < (const col_amp & one, const col_amp & two) {return ((one.c<<8)+one.a) <((two.c<<8) +two.a);}

//Structure to describe a numeric key and it's friendly name
struct key_name
{
	uint16_t key;
	String name;
};

//Structure to describe a parameter setting (composed of the unit key and the subunit key, the parameter's name and the value's limits)
struct unit_info
{
 uint16_t uk;
 uint16_t sk;
 String na;
 double ll;
 double ul;
};

//Structure to describe a value as a 16-Bit type key and a 32-Bit value
struct key_longval
{
   uint16_t type;
   uint32_t val;
};

//Structure to describe a value that can be either a 32-Bit value or a String
struct string_or_ulong
{
	bool isString;
 	
	byte type;
	 
	String nam;
	uint32_t val;	  
};

//Dictionary to store unit key and unit name for each (effect-)unit
extern std::map<THR30II_UNITS, key_name> THR30II_UNITS_VALS;

//Mapping the MIDI-dump keys to the corresponding single command MIDI keys
extern std::map<uint16_t, uint16_t> effectMap;

//Dictionary to store subunit key and subunit name for each subunit of the unit "effect"
extern std::map<THR30II_EFF_TYPES, key_name> THR30II_EFF_TYPES_VALS;

//Dictionary to store info for the settings parameter of subunit "phaser" of the unit "effect"
extern std::map<THR30II_EFF_SET_PHAS, unit_info> THR30II_INFO_PHAS;
//Dictionary to store info for the settings parameter of subunit "tremolo" of the unit "effect"
extern std::map<THR30II_EFF_SET_TREM, unit_info> THR30II_INFO_TREM;
//Dictionary to store info for the settings parameter of subunit "flanger" of the unit "effect"
extern std::map<THR30II_EFF_SET_FLAN, unit_info> THR30II_INFO_FLAN;
//Dictionary to store info for the settings parameter of subunit "chorus" of the unit "effect"
extern std::map<THR30II_EFF_SET_CHOR, unit_info> THR30II_INFO_CHOR;

//Dictionary to store all the dictionaries with info for the settings parameters of the unit "effect"
extern std::map<THR30II_EFF_TYPES, std::map<String, unit_info>> THR30II_INFO_EFFECT;

//Mapping the MIDI-dump keys to the corresponding single command MIDI keys
extern std::map<uint16_t, uint16_t> reverbMap;

//Dictionary to store subunit key and subunit name for each subunit of the unit "reverb"
extern std::map<THR30II_REV_TYPES, key_name> THR30II_REV_TYPES_VALS;
//Dictionary to store info for the settings parameter of subunit "spring" of the unit "reverb"
extern std::map<THR30II_REV_SET_SPRI, unit_info> THR30II_INFO_SPRI;
//Dictionary to store info for the settings parameter of subunit "plate" of the unit "reverb"
extern std::map<THR30II_REV_SET_PLAT, unit_info> THR30II_INFO_PLAT;
//Dictionary to store info for the settings parameter of subunit "hall" of the unit "reverb"
extern std::map<THR30II_REV_SET_HALL, unit_info> THR30II_INFO_HALL;
//Dictionary to store info for the settings parameter of subunit "room" of the unit "reverb"
extern std::map<THR30II_REV_SET_ROOM, unit_info> THR30II_INFO_ROOM;

//Dictionary to store all the dictionaries with info for the settings parameters of the unit "reverb"
extern std::map<THR30II_REV_TYPES, std::map<String, unit_info>> THR30II_INFO_REVERB;

//Mapping the MIDI-dump keys to the corresponding single command MIDI keys for unit ECHO
extern std::map<uint16_t, uint16_t> echoMap;

//Dictionary to store subunit key and subunit name for each subunit of the unit "echo"
extern std::map<THR30II_ECHO_TYPES, key_name> THR30II_ECHO_TYPES_VALS;
//Dictionary to store info for the settings parameter of subunit "tape delay" of the unit "echo"
extern std::map<THR30II_ECHO_SET_TAPE, unit_info> THR30II_INFO_TAPE;
//Dictionary to store info for the settings parameter of subunit "digital delay" of the unit "echo"
extern std::map<THR30II_ECHO_SET_DIGI, unit_info> THR30II_INFO_DIGI;
//Dictionary to store all the dictionaries with info for the settings parameters of the unit "echo"
extern std::map<THR30II_ECHO_TYPES, std::map<String, unit_info>> THR30II_INFO_ECHO;

//Mapping the MIDI-dump keys to the corresponding single command MIDI keys
extern uint16_t CompressorMap(uint16_t u);
extern uint16_t UnitOnMap(uint16_t u);
extern uint16_t EchoMap(uint16_t u);
extern uint16_t EffectMap(uint16_t u);
extern uint16_t ReverbMap(uint16_t u);

//The constant cabinet names
extern std::map<THR30II_CAB, String> THR30II_CAB_NAMES;

extern std::map<col_amp, uint16_t> THR30IIAmpKeys;

extern col_amp THR30IIAmpKey_ToColAmp(uint16_t ampkey);

extern byte * dump;   //dynamic Array because of big size

extern size_t dump_len; //size of the dynamic dump array

class Outmessage;    //forward declaration
class SysExMessage;  //forward declaration

//The main class for handling all settings and transfers of THR30II
class THR30II_Settings
{
  public:
	uint32_t ConnectedModel;  //FamilyID (2 Byte) + ModelNr.(2 Byte) , 0x00240002=THR30II
	static std::map<String, std::vector<byte> > tokens;
	
	int patch_setAll(uint8_t * buf, uint16_t buf_len );
	int SetLoadedPatch(const DynamicJsonDocument &djd );
	void createPatch();
	void CreateNamePatch(); //fill send buffer with just setting for actual patchname, creating a valid SysEx for sending to THR30II
	void SetControl(uint8_t ctrl, double value);
	double GetControl(uint8_t ctrl);
	void SetGuitarVolume(double value);
	double GetGuitarVolume();
	void SetAudioVolume(double value);
	double GetAudioVolume();
	void SendTypeSetting(THR30II_UNITS unit, uint16_t val); //Send setting for unit type to THR30II	
	void SetPatchName(String nam, int nr=-1);  //for the 5 User-Settings (-1 = actual as default )
	String getPatchName();
	void updateStatusMask(uint8_t x, uint8_t y);
	void updateConnectedBanner(); //Show the connected Model 
	int8_t getActiveUserSetting(); //Getter for number of the active user setting
	bool getUserSettingsHaveChanged(); //Getter for state of user Settings
	//void SetAmp(uint8_t _amp);
	void SetColAmp(THR30II_COL _col, THR30II_AMP _amp);  //Setter for the Simulation Collection
	void setColAmp(uint16_t ca); //Setter for the Simulation Collection by key
	void SetCab(THR30II_CAB _cab);  //Setter for the Cabinet Simulation
	void ReverbSelect(THR30II_REV_TYPES type);  //Setter for selection of the reverb type
	void EffectSelect(THR30II_EFF_TYPES type);   //Setter for selection of the Effect type
	void EchoSelect(THR30II_ECHO_TYPES type);   //Setter for selection of the Echo type
	
	String ParseSysEx(const byte cur[], int cur_len);
	
    //---------FUNCTION FOR SENDING COL/AMP SETTING TO THR30II -----------------
    void SendColAmp(); //Send COLLLECTION/AMP setting to THR
  
	//---------FUNCTION FOR SENDING CAB SETTING TO THR30II -----------------
	void SendCab(); //Send cabinet setting to THR

	//---------FUNCTION FOR SENDING UNIT STATE TO THR30II -----------------

	void SendUnitState(THR30II_UNITS un); //Send unit state setting to THR30II

    //using a template function to be able to use it for different types
	template <typename T>
	void  SendParameterSetting( un_cmd command, type_val <T> valu)  //Send setting to THR
	{
		  //Special unit 0xFFFF for "system parameter"
            //THR30II remoting: change OUTPUT AUDIO volume => Message to THR30: (2 Frame)
            //PC:
            //0000   f0 00 01 0c 24 02 4d 00 16 00 00 07 00 0a 00 00   header 0A for par.change
            //0010   00 10 00 00 00 00 00 00 00 00 00 00 f7
            //PC:
            //0000   f0 00 01 0c 24 02 4d 00 17 00 00 0f 78 7f 7f 7f     FFFFFFFF = system par.
            //0010   7f 4b 01 00 03 00 04 00 00 00 4d 4c 40 4c 3e 00    01 4b = "AudioVolume"
            //0020   00 00 00 00 f7              0x3ecccccd = 0.4(float little endian)
            //THR:
            //0000   f0 00 01 0c 24 02 4d 00 49 00 00 0b 00 01 00 00   Response 4 Byte val = 0
            //0010   00 04 00 00 00 00 | 00 00 00 00 | 00 00 f7(Ack.)

		if (!MIDI_Activated)
			return;
		
		extern ArduinoQueue<Outmessage> outqueue;

		std::array<byte,16> raw_msg_body = {}; //4 Ints:  Unit + Setting + Type + Val
		std::array<byte,8>  raw_msg_head = {};  //2 Ints:  Opcode + Len(Body)

		raw_msg_head[0] = 0x0A;  //0x0A = Opcode for "parameter change"
		raw_msg_head[4] = (byte) 16; //Length of body  

		raw_msg_body[0] = (byte)(command.unit % 256);  //Normal parameters like GAIN use 2-Byte unit e.g. 0x013C
		raw_msg_body[1] = (byte)(command.unit / 256);
		
		if(command.unit==0xFFFF)   //Extended parameters like AudioVolume and GuitarVolume use 4-Byte unit "0xFFFFFFFF"
        {  //so fill next two bytes with FF instead of 00
                raw_msg_body[2] = (byte)(command.unit % 256);
                raw_msg_body[3] = (byte)(command.unit / 256);
		}
		
		raw_msg_body[4] = (byte)(command.command % 256);
		raw_msg_body[5] = (byte)(command.command / 256);
		raw_msg_body[8] = valu.type;

		uint32_t c_val = 0x00lu;
		
		if(valu.type != (byte)0x04)  //0x04 = double
		{	
		      c_val = (uint32_t) valu.val;
		}
		else
		{
			//in these cases the T-Parameter is of type double => overwrite it with 
			if (command.command == THR30II_UNITS_VALS[GATE].key && command.unit==THR30II_GATE_VALS[GA_THRESHOLD])
			{
				c_val = ValToNumber_Threshold((double) valu.val);
			}
			else if(valu.type == 0x04 ) //marker for double
			{
				c_val = ValToNumber((double) valu.val);
			}
		}
		raw_msg_body[12] = (byte)(c_val & 0xFF);
		raw_msg_body[13] = (byte)((c_val & 0xFF00) >> 8);
		raw_msg_body[14] = (byte)((c_val & 0xFF0000) >> 16);
		raw_msg_body[15] = (byte)((c_val & 0xFF000000) >> 24);

		//Prepare Message-Body
		std::array<byte,100> msg_body = { };
		byte *mblast = msg_body.begin();

		mblast=Enbucket(msg_body, raw_msg_body, raw_msg_body.end() );
		
		//PC_SYSEX_BEGIN.size() =7u
		//msg_body.size() = 100u
		std::array<byte,(size_t)(7u + 2u + 3u + 100u + 1u)>  sendbuf_body = {};  //for "00" and frame-counter; 3 for Lenght-Field, 1 for "F7"
        byte* sbblast=sendbuf_body.begin();

		sbblast=std::copy(PC_SYSEX_BEGIN.begin(), PC_SYSEX_BEGIN.end(), sbblast);
		
		sbblast++;
		*sbblast++ = 0x00; //place holder for SysExSendCounter
		*sbblast++ = 0x00;  //only one frame in this message
		*sbblast++ = (byte)((raw_msg_body.size() - 1) / 16);  //Length Field (Hi)
		*sbblast++ = (byte)((raw_msg_body.size() - 1) % 16); //Length Field (Low)

		sbblast = std::copy(msg_body.begin(),mblast, sbblast);
		*sbblast++ = SYSEX_STOP;
		
		//Prepare Message-Header
		std::array<byte,16> msg_head = {};  //The 8 Bytes of raw_msg_head result in 2 groups of  (7 bytes+ 1 bitbucket byte)
		byte * mhlast=msg_head.begin();
		mhlast = Enbucket(msg_head, raw_msg_head, raw_msg_head.end());

		//PC_SYSEX_BEGIN.size() =7u
		//msg_body.size() = 100u
		//msg_head.size() = 16u
		std::array<byte, 7 + 2 + 3 + 16 + 1> sendbuf_head = {};  //29 Bytes
		byte *sbhlast = sendbuf_head.begin();

		sbhlast=std::copy(PC_SYSEX_BEGIN.begin(), PC_SYSEX_BEGIN.end(),sbhlast);

		sbhlast++;
		*sbhlast++ = 0x00;
		*sbhlast++ = 0x00;  //only one frame in this message
		*sbhlast++ = (byte)((raw_msg_head.size() - 1) / 16);  //Length Field (Hi)
		*sbhlast++ = (byte)((raw_msg_head.size() - 1) % 16);  //Length Field (Low)
		sbhlast=std::copy(msg_head.begin(), mhlast, sbhlast );
		*sbhlast++ = SYSEX_STOP;
		
		sendbuf_head[PC_SYSEX_BEGIN.size() + 1] = UseSysExSendCounter();
		
		//hexdump(sendbuf_head,sendbuf_head.size());
		outqueue.enqueue(Outmessage(SysExMessage ( sendbuf_head.data(), sendbuf_head.size()),1000,false,false)); //no ack/answ for the header  
		
		sendbuf_body[PC_SYSEX_BEGIN.size() + 1] = UseSysExSendCounter();
		//hexdump(sendbuf_body,sbblast-sendbuf_body.begin()); 
		outqueue.enqueue(Outmessage(SysExMessage( sendbuf_body.data(), sbblast-sendbuf_body.begin()),1001,true,false)); //needs ack  

		//ToDO:  handle ACK for id=1001
		//e.g. only accept parameter as changed, if ack. has arrived - otherwise show broken connection/timeout
		//and roll back parameter change in the internal mirror settings
	}	//of SendParameterSetting

   

	std::array<double,CTRL_TREBLE-CTRL_GAIN+1> control {{50,50,50,50,50}}; //actual state of main control knobs
	std::array<double,CTRL_TREBLE-CTRL_GAIN+1> control_store {{50,50,50,50,50}}; //state of main control knobs for simple Volume-Solo
	
	static double NumberToVal(uint32_t num, bool cut = true); //convert the 32Bit parameter value to 0..100 Slider value
	static double NumberToVal_Threshold(uint32_t num, bool cut = true); //convert the 32Bit parameter value to a 0..100 Slider value
	static uint32_t ValToNumber_Threshold(double val); //convert a 0..100 Slider value to a 32Bit parameter value (for internal -96dB to 0dB range)
	static uint32_t ValToNumber(double val); //convert a 0..100 Slider value to a 32Bit parameter value

	void EchoSetting(THR30II_ECHO_TYPES type, uint16_t ctrl, double value);  //Setter for the Echo Parameters
	void EchoSetting(uint16_t ctrl, double value); //using the internally selected actual Echo type
	void EffectSetting(THR30II_EFF_TYPES type, uint16_t ctrl, double value);  //Setter for Effect parameters
	void EffectSetting(uint16_t ctrl, double value); //using the internally selected actual effect type
	void GateSetting(THR30II_GATE ctrl, double value); //Setter for gate effect parameters
	void ReverbSetting(THR30II_REV_TYPES type, uint16_t ctrl, double value); //Setter for Reverb parameters
	void ReverbSetting(uint16_t ctrl, double value);  //using internally selected actual reverb type
	void CompressorSetting(THR30II_COMP ctrl, double value);  //Setter for compressor effect parameters
	void Switch_On_Off_Gate_Unit(bool state);   //Setter for switching on / off the effect unit
	void Switch_On_Off_Echo_Unit(bool state);   //Setter for switching on / off the Echo unit
	void Switch_On_Off_Effect_Unit(bool state);   //Setter for switching on / off the effect unit
	void Switch_On_Off_Compressor_Unit(bool state);  //setter for switching on /off the compressor unit
	void Switch_On_Off_Reverb_Unit(bool state);   //Setter for switching on / off the reverb unit
    void Init_Dictionaries();

	byte UseSysExSendCounter(); //returns the actual counter value and increments it afterwards
	
	THR30II_Settings();  //Constructor

	//State Machine for Dump - Analysis
    enum States { St_none, St_idle, St_structure, St_meta, St_global, St_data, St_unit, St_subunit, St_valuesUnit, St_valuesSubunit, St_error };
    //enum Triggers { Tr_key };

    static States _state; //Initial State as pre-Set Value
	//static States _last_state;
	bool sendChangestoTHR = true;  //set to false, if changes come from THR-Knobs
	//State vars
  private:
	double guitarVolume = 50;  //Field for the actualState of the "GuitarVolume" knob
	double audioVolume = 50;   //Field for the actualState of the "AudioVolume"  knob
	bool MIDI_Activated = false;   //set true, if MIDI unlocked by magic key (success checked by receiving first regular THR-SysEx)
	
	bool dumpInProgress = false;
	bool patchdump = false;
	bool symboldump = false;
	std::array< String, 6 > patchNames { { "Actual", "UserSetting1", "UserSetting2", "UserSetting3", "UserSetting4", "UserSetting5" } }; //better than an array, because it can be deep copied

	uint16_t dumpFrameNumber = 0; 
	uint32_t dumpByteCount = 0;  //received bytes
	uint16_t dumplen = 0;  //expected length in bytes
    uint32_t Firmware = 0x00000000;
     //00 24 00 00 : THR10II
     //00 24 00 01 : THR10IIWireless
     //00 24 00 02 : THR30IIWireless
     //00 24 00 03 : THR30IIAcousticWireless

	std::array<bool,GATE-COMPRESSOR+1> unit;  //actual on/off state of the effect units
	
	THR30II_AMP amp; //Field for the simulated AMP model
    THR30II_COL col; //Field for the simulation collection (BOUTIQUE, CLASSIC, MODERN)
    THR30II_CAB cab; //Field for the simulated cabinet
    int8_t activeUserSetting; //Field for the selected user preset's index (0..4 , -1 for none / actual)
    bool userSettingsHaveChanged; //Field for state of selected user preset

    std::array<double,CO_MIX-CO_SUSTAIN +1> compressor_setting;   //Field for the Compressor settings
	//double echo_setting[EC_MIX-EC_BASS+1];    //Field for the Echo settings
    std::array<double, GA_DECAY-GA_THRESHOLD+1 > gate_setting;   //Field for the Gate settings
	
	std::map<THR30II_ECHO_TYPES, std::map<int, double>> echo_setting =     //Fields
	{
	 {
		{ TAPE_ECHO ,     {  {(int)TA_BASS ,50.0}, {(int)TA_TREBLE, 50.0}, {(int)TA_FEEDBACK, 50.0}, {(int)TA_TIME, 50.0}, {(int)TA_MIX, 50.0}, {(int)TA_SYNCSELECT, 50.0} } } ,
		{ DIGITAL_DELAY , {  {(int)TA_BASS ,50.0}, {(int)DD_TREBLE, 50.0}, {(int)DD_FEEDBACK, 50.0}, {(int)DD_TIME, 50.0}, {(int)DD_MIX, 50.0}, {(int)DD_SYNCSELECT, 50.0} } } 
	 }	
	}; 

	std::map<THR30II_REV_TYPES, std::map<int, double>> reverb_setting =     //Fields
	{
	 {
		{ SPRING , {  {(int)SP_REVERB ,10.0}, {(int)SP_TONE, 25.0}, {(int)SP_MIX, 77.0} } } ,
		{ PLATE  , {  {(int)PL_DECAY,  10.0}, {(int)PL_TONE, 25.0}, {(int)PL_PREDELAY, 77.0}, {(int)PL_MIX, 88.0} } },
		{ HALL   , {  {(int)HA_DECAY , 10.0}, {(int)HA_TONE, 25.0}, {(int)HA_PREDELAY, 77.0}, {(int)HA_MIX, 88.0} } },
		{ ROOM   , {  {(int)RO_DECAY,  10.0}, {(int)RO_TONE, 25.0}, {(int)RO_PREDELAY, 77.0}, {(int)RO_MIX, 88.0} } } 
	 }	
	}; 

	std::map<THR30II_EFF_TYPES, std::map<int, double>> effect_setting=   //Fields
	{            
		{
			{PHASER, { {PH_SPEED, 0.0},    {PH_FEEDBACK, 0.0}, {PH_MIX, 0.0}  } },
			{TREMOLO,{ {TR_SPEED, 0.0},    {TR_DEPTH, 0.0},    {TR_MIX, 0.0}  } },
			{FLANGER,{ {FL_DEPTH, 0.0},    {FL_SPEED, 0.0},    {FL_MIX, 0.0}  } },
			{CHORUS, { {CH_FEEDBACK, 0.0}, {CH_DEPTH, 0.0},    {CH_SPEED, 0.0 }, {CH_PREDELAY, 0.0}, {CH_MIX, 0.0} } }
		}
	};

	THR30II_REV_TYPES reverbtype = SPRING;

	THR30II_EFF_TYPES effecttype = PHASER;

	THR30II_ECHO_TYPES echotype = TAPE_ECHO;

    //Yamaha THRII models names
    String THR30II_MODEL_NAME();

	uint32_t Tnid;  //global from Patch dump
	uint32_t UnknownGlobal; //global from Patch dump
	uint32_t ParTempo; //global from Patch dump (minimum value of 110 is coded with 0x00000000)

	byte SysExSendCounter;


}; //of class THR30II_Settings

struct Dumpunit
{
	public:
	 	uint16_t type;  //In an FX unit the type is not a var type but an FX type
		uint16_t parCount;
		std::map<uint16_t, key_longval> values;
	    std::map<uint16_t, Dumpunit> subUnits;
};

class SysExMessage
{
	public:
	   SysExMessage();
	   SysExMessage(const byte * data ,size_t size); //Constructor
	   SysExMessage(const SysExMessage &other );  //Copy Constructor
	   SysExMessage(SysExMessage &&other ) noexcept ; //Move Constructor
	   ~SysExMessage(); //destructor
	   SysExMessage & operator=( const SysExMessage & other ); //Copy-assignment
	   SysExMessage & operator=( SysExMessage && other ) noexcept; //Move-assignment
	   const byte * getData(); //getter for the byte-Array
	   size_t getSize(); //getter for the byte-Array-Size
	private:
		byte *Data;	
		size_t Size=0;
};

class Outmessage
{
	public:
	 uint16_t _id = 0;

	 SysExMessage _msg;
	 boolean _needs_ack;
	 boolean _needs_answer;
	 boolean _sent_out = false;
	 boolean _acknowledged = false;
	 boolean _answered = false;

	 uint32_t _time_stamp;   //time as millis()-value, when message was sent out. Use for queue time-out if no ack /answ

	Outmessage(const SysExMessage &msg, uint16_t id, bool needs_ack = false, bool needs_answer = false)
	:_id(id),_msg(msg),_needs_ack(needs_ack),_needs_answer(needs_answer)
	{
	}
	Outmessage():_msg( SysExMessage() )
	{}

}; 

#endif /* THR30II_H_ */
