/*THR30II Pedal using Teensy 3.6
* Martin Zwerschke 04/2021
*/

/* Init_Dictionaries.cpp
*  last modified 09/2021
*  Author: Martin Zwerschke 
*/

#include <Arduino.h>
#undef max
#undef min
#include <map>
#include "THR30II.h"
#include "Globals.h"

std::map<String, uint16_t> Constants::glo ;

std::map<THR30II_CAB, String> THR30II_CAB_NAMES
{
	{THR30II_CAB::British_4x12 , "British 4x12"},
	{THR30II_CAB::American_4x12 , "American 4x12"},
	{THR30II_CAB::Brown_4x12 , "Brown 4x12"},
	{THR30II_CAB::Vintage_4x12 , "Vintage 4x12"},
	{THR30II_CAB::Fuel_4x12 , "Fuel 4x12"},
	{THR30II_CAB::Juicy_4x12 , "Juicy 4x12"},
	{THR30II_CAB::Mods_4x12 , "Mods 4x12"},
	{THR30II_CAB::American_2x12 , "American 2x12"},
	{THR30II_CAB::British_2x12 , "British 2x12"},
	{THR30II_CAB::British_Blues , "British Blues"},
	{THR30II_CAB::Boutique_2x12 , "Boutique 2x12"},
	{THR30II_CAB::Yamaha_2x12 , "Yamaha 2x12"},
	{THR30II_CAB::California_1x12 , "California 1x12"},
	{THR30II_CAB::American_1x12 , "American 1x12"},
	{THR30II_CAB::American_4x10 , "American 4x10"},
	{THR30II_CAB::Boutique_1x12 , "Boutique 1x12"},
	{THR30II_CAB::Bypass , "Bypass"}
};

std::map<col_amp, uint16_t> THR30IIAmpKeys;

std::map<THR30II_AMP, String> THR30II_AMP_NAMES
{
	{THR30II_AMP::CLEAN , "Clean"},
    {THR30II_AMP::CRUNCH , "Crunch"},
    {THR30II_AMP::LEAD , "Lead"},
    {THR30II_AMP::HI_GAIN , "Hi Gain"},
    {THR30II_AMP::SPECIAL , "Special"},
    {THR30II_AMP::BASS , "Bass"},
    {THR30II_AMP::ACO , "Aco"},
    {THR30II_AMP::FLAT , "Flat"}
};

std::map<THR30II_COL, String> THR30II_COL_NAMES
{
	{THR30II_COL::CLASSIC , "Classic"},
    {THR30II_COL::BOUTIQUE , "Boutique"},
    {THR30II_COL::MODERN , "Modern"},
};

std::map<THR30II_UNITS, uint16_t> THR30II_UNIT_ON_OFF_COMMANDS;

//CAB-Simulation is Sub-Unit of GATE 
uint16_t THR30II_CAB_COMMAND;// = 0x0105;       //SpkSimType
uint16_t THR30II_CAB_COMMAND_DUMP;// = 0x0124;  //SpkSimTypeState

//Mapping the MIDI-dump keys to the corresponding single command MIDI keys
std::map<THR30II_CTRL_SET, uint16_t> THR30II_CTRL_VALS;
std::map<uint16_t, uint16_t> controlMap;
std::map<THR30II_GATE, uint16_t> THR30II_GATE_VALS;
std::map<uint16_t, uint16_t> gateMap;

std::map<THR30II_UNITS, key_name> THR30II_UNITS_VALS;
std::map<uint16_t, uint16_t> effectMap;
std::map<THR30II_EFF_TYPES, key_name> THR30II_EFF_TYPES_VALS;
std::map<THR30II_EFF_SET_PHAS, unit_info> THR30II_INFO_PHAS;
std::map<THR30II_EFF_SET_TREM, unit_info> THR30II_INFO_TREM;
std::map<THR30II_EFF_SET_FLAN, unit_info> THR30II_INFO_FLAN;
std::map<THR30II_EFF_SET_CHOR, unit_info> THR30II_INFO_CHOR;
std::map<THR30II_EFF_TYPES, std::map<String, unit_info>> THR30II_INFO_EFFECT;
std::map<uint16_t, uint16_t> reverbMap;
std::map<THR30II_REV_TYPES, key_name> THR30II_REV_TYPES_VALS;
std::map<THR30II_REV_SET_SPRI, unit_info> THR30II_INFO_SPRI;
std::map<THR30II_REV_SET_PLAT, unit_info> THR30II_INFO_PLAT;
std::map<THR30II_REV_SET_HALL, unit_info> THR30II_INFO_HALL;
std::map<THR30II_REV_SET_ROOM, unit_info> THR30II_INFO_ROOM;
std::map<THR30II_REV_TYPES, std::map<String, unit_info>> THR30II_INFO_REVERB;

std::map<uint16_t, uint16_t> echoMap;
std::map<THR30II_ECHO_TYPES, key_name> THR30II_ECHO_TYPES_VALS;
std::map<THR30II_ECHO_SET_TAPE, unit_info> THR30II_INFO_TAPE;
std::map<THR30II_ECHO_SET_DIGI, unit_info> THR30II_INFO_DIGI;
std::map<THR30II_ECHO_TYPES, std::map<String, unit_info>> THR30II_INFO_ECHO;

std::map<uint16_t, uint16_t> compressorMap;
std::map<uint16_t, uint16_t> unitOnMap;
std::map<THR30II_COMP, uint16_t> THR30II_COMP_VALS;

byte * dump=nullptr;   //dynamic Array because of big size
size_t dump_len=0;  //length of the dynamic array for the dump

void THR30II_Settings::Init_Dictionaries()
{
    std::map<String,uint16_t> & glob = Constants::glo ;

    THR30IIAmpKeys =
    {
        /* Old Firmware 1.30.0c
        AMP-Sim Modern  Boutique  Classic
        CLEAN:	  32(B2)  36(B6)  4a(4A)
        CRUNCH:   23(A3)  71(71)  08(88)
        LEAD:     3b(BB)  28(A8)  19(99)
        HIGAIN:   0b(8B)  2f(AF)  7f(7F)
        SPECIAL:  1c(9C)  04(84)  78(78)
        BASS      38(B8)  2b(AB)  37(B7)
        ACOUSTIC: 17(97)  18(98)  13(93)
        FLAT:	  35(B5)  34(B4)  0f(8F)*/

        {{CLASSIC, CLEAN}, glob["THR10C_Deluxe"]}, // = 0x4A,
        {{CLASSIC, CRUNCH}, glob["THR10C_DC30"]}, // = 0x88,
        {{CLASSIC, LEAD}, glob["THR10_Lead"]}, // = 0x99,
        {{CLASSIC, HI_GAIN}, glob["THR10_Modern"]}, // = 0x7F,
        {{CLASSIC, SPECIAL}, glob["THR10X_Brown1"]}, // = 0x78,
        {{CLASSIC, BASS}, glob["THR10_Bass_Eden_Marcus"]}, // = 0xB7,
        {{CLASSIC, ACO}, glob["THR10_Aco_Condenser1"]}, // = 0x93,
        {{CLASSIC, FLAT}, glob["THR10_Flat"]}, // = 0x8F,

        {{BOUTIQUE, CLEAN}, glob["THR10C_BJunior2"]}, //= 0xB6,
        {{BOUTIQUE, CRUNCH}, glob["THR10C_Mini"]}, // = 0x71,
        {{BOUTIQUE, LEAD}, glob["THR30_Blondie"]}, // = 0xA8,
        {{BOUTIQUE, HI_GAIN}, glob["THR30_FLead"]}, // = 0xAF,
        {{BOUTIQUE, SPECIAL}, glob["THR10X_South"]}, // = 0x84,
        {{BOUTIQUE, BASS}, glob["THR10_Bass_Mesa"]}, // = 0xAB,
        {{BOUTIQUE, ACO}, glob["THR10_Aco_Tube1"]}, // = 0x98,
        {{BOUTIQUE, FLAT}, glob["THR10_Flat_B"]}, // = 0xB4,

        {{MODERN, CLEAN}, glob["THR30_Carmen"]},// = 0xB2,
        {{MODERN, CRUNCH}, glob["THR30_SR101"]}, //= 0xA3,
        {{MODERN, LEAD}, glob["THR10_Brit"]}, //= 0xBB,
        {{MODERN, HI_GAIN}, glob["THR10X_Brown2"]}, //= 0x8B,
        {{MODERN, SPECIAL}, glob["THR30_Stealth"]}, // = 0x9C,
        {{MODERN, BASS}, glob["THR30_JKBass2"]}, //= 0xB2,
        {{MODERN, ACO}, glob["THR10_Aco_Dynamic1"]}, // = 0x97,
        {{MODERN, FLAT}, glob["THR10_Flat_V"]}, // = 0xB5,
    };

    THR30II_UNITS_VALS=
    {
        {COMPRESSOR, {glob["FX1"] , "Compressor"}},
        {CONTROL, {glob["Amp"], "Control"}}, 
        {EFFECT, {glob["FX2"], "Effect"}},
        {ECHO, {glob["FX3"], "Echo"}},  
        {REVERB, {glob["FX4"], "Reverb"}},
        {GATE, {glob["GuitarProc"], "Gate"}}
    };

    THR30II_UNIT_ON_OFF_COMMANDS=
    {
        {COMPRESSOR,glob["FX1Enable"]}, 
        {EFFECT, glob["FX2Enable"]},
        {ECHO, glob["FX3Enable"]},
        {REVERB, glob["FX4Enable"]},
        {GATE, glob["GateEnable"]} 
    };
   
    unitOnMap=
    {
        {glob["FX1EnableState"], glob["FX1Enable"]},  //FX1=Compressor
        {glob["FX2EnableState"], glob["FX2Enable"]},  //FX2=Effect
        {glob["FX3EnableState"], glob["FX3Enable"]},  //FX3=Echo
        {glob["FX4EnableState"], glob["FX4Enable"]},  //FX4=Reverb 
        {glob["GateEnableState"], glob["GateEnable"]}  //GateEnableState=>GateEnable
    };

    //CAB-Simulation is Sub-Unit of GATE 
    THR30II_CAB_COMMAND = glob["SpkSimType"]; //SpkSimType
    THR30II_CAB_COMMAND_DUMP = glob["SpkSimTypeState"]; //SpkSimTypeState

    THR30II_EFF_TYPES_VALS=
    {
        {PHASER, {glob["Phaser"], "Phaser"}},  
        {TREMOLO, {glob["BiasTremolo"], "Tremolo"}},
        {FLANGER, {glob["L6Flanger"], "Flanger"}},
        {CHORUS, {glob["StereoSquareChorus"], "Chorus"}} 
    };

    effectMap=
    {
        {glob["FeedbackState"],glob["Feedback"]},//Effect.FeedBackState=>Effect.FeedBack
        {glob["SpeedState"],glob["Speed"]},//Effect.Speed=>Effect.Speed
        {glob["DepthState"],glob["Depth"]},//Effect.DepthState =>Effect.Depth
        {glob["SyncSelectState"],glob["SyncSelect"]}, //SyncSelectState=>SyncSelect  (Not in THR_Remote)
        {glob["FreqState"],glob["Freq"]},//Effect.FreqState => Effect.Freq 
        {glob["PreState"], glob["Pre"]},//Effect.PreDelayState=>Effect.Pre
        {glob["FX2MixState"],glob["FX2Mix"]}   //FX2.MixState=> FX2.Mix
    };

    THR30II_INFO_PHAS=
    {
        {PH_SPEED , {glob["FX2"], glob["Speed"], "Speed", 0, 100}},
        {PH_FEEDBACK, {glob["FX2"], glob["Feedback"], "Feedback", 0, 100}},
        {PH_MIX, {glob["GuitarProc"], glob["FX2Mix"], "Mix", 0, 100}}
    };

    THR30II_INFO_TREM=
    {
        {TR_SPEED ,{glob["FX2"], glob["Speed"], "Speed", 0, 100}}, 
        {TR_DEPTH ,{glob["FX2"], glob["Depth"], "Depth", 0, 100}}, 
        {TR_MIX   ,{glob["GuitarProc"], glob["FX2Mix"], "Mix", 0, 100}}
    };

    THR30II_INFO_FLAN=
    {
        {FL_DEPTH ,{glob["FX2"], 0x00E0, "Depth", 0, 100}},
        {FL_SPEED ,{glob["FX2"], 0x00E4, "Speed", 0, 100}},
        {FL_MIX   ,{glob["GuitarProc"], glob["FX2Mix"], "Mix", 0, 100}}
    };

    THR30II_INFO_CHOR=
    {
        {CH_SYNCSELECT,{glob["FX2"], glob["SyncSelect"], "SyncSelect", 0, 100}},
        {CH_FEEDBACK,{glob["FX2"], glob["Feedback"], "Feedback", 0, 100}}, 
        {CH_DEPTH,{glob["FX2"], glob["Depth"], "Depth", 0, 100}}, 
        {CH_SPEED,{glob["FX2"], glob["Freq"], "Speed", 0, 100}}, 
        {CH_PREDELAY,{glob["FX2"], glob["Pre"], "PreDelay", 0, 100}},
        {CH_MIX,{glob["GuitarProc"], glob["FX2Mix"], "Mix", 0, 100}}
    };

    THR30II_INFO_EFFECT=
    {
        { PHASER ,{
            {"SPEED", THR30II_INFO_PHAS[PH_SPEED]},
            {"FEEDBACK" , THR30II_INFO_PHAS[PH_FEEDBACK]},
            {"MIX", THR30II_INFO_PHAS[PH_MIX]} }
        },
        { TREMOLO ,{
            {"SPEED", THR30II_INFO_TREM[TR_SPEED]},
            {"DEPTH", THR30II_INFO_TREM[TR_DEPTH]},
            {"MIX", THR30II_INFO_TREM[TR_MIX]} }
        },
        { FLANGER ,{
            {"DEPTH", THR30II_INFO_FLAN[FL_DEPTH]},
            {"SPEED", THR30II_INFO_FLAN[FL_SPEED]},
            {"MIX",   THR30II_INFO_FLAN[FL_MIX]} }
        },
        { CHORUS ,{
            {"FEEDBACK" , THR30II_INFO_CHOR[CH_FEEDBACK]},
            {"DEPTH" , THR30II_INFO_CHOR[CH_DEPTH]},
            {"SPEED" , THR30II_INFO_CHOR[CH_SPEED]},
            {"PREDELAY" , THR30II_INFO_CHOR[CH_PREDELAY]},
            {"MIX" , THR30II_INFO_CHOR[CH_MIX]} }
        }
    };

    THR30II_REV_TYPES_VALS=
    {
        {SPRING, {glob["StandardSpring"], "Spring"}},
        {PLATE , {glob["LargePlate1"], "Plate"}},
        {HALL  , {glob["ReallyLargeHall"],"Hall"}},
        {ROOM  , {glob["SmallRoom1"],"Room"}} 
    };

    reverbMap=
    {
        {glob["TimeState"], glob["Time"]},  //Reverb.TimeState=>Reverb.Reverb (Time),
        {glob["ToneState"], glob["Tone"]},  //Reverb.ToneState =>Reverb.Tone,
        {glob["DecayState"], glob["Decay"]},  //Reverb.DecayState => Reverb.Decay,
        {glob["PreDelayState"], glob["PreDelay"]},  //Reverb.PreDelayState=>Reverb.PreDelay,
        {glob["FX4WetSendState"], glob["FX4WetSend"]}  //Reverb.MixState=>Reverb.Mix 
    };

    THR30II_INFO_SPRI=
    {
        {SP_REVERB , {glob["FX4"], glob["Time"], "Reverb", 0, 100}},
        {SP_TONE   , {glob["FX4"], glob["Tone"], "Tone", 0, 100}},  
        {SP_MIX    , {glob["GuitarProc"], glob["FX4WetSend"], "Mix", 0, 100}}
    };

    THR30II_INFO_PLAT=
    {
        {PL_DECAY, {glob["FX4"], glob["Decay"], "Decay", 0, 100}},       
        {PL_TONE, {glob["FX4"], glob["Tone"], "Tone", 0, 100}},     
        {PL_PREDELAY, {glob["FX4"], glob["PreDelay"], "Pre-Delay", 0, 100}},
        {PL_MIX, {glob["GuitarProc"], glob["FX4WetSend"], "Mix", 0, 100}}
    };

    THR30II_INFO_HALL=
    {
        {HA_DECAY     , {glob["FX4"], glob["Decay"], "Decay", 0, 100}},  
        {HA_TONE      , {glob["FX4"], glob["Tone"], "Tone", 0, 100}},   
        {HA_PREDELAY  , {glob["FX4"], glob["PreDelay"], "Pre-Delay", 0, 100}},
        {HA_MIX       , {glob["GuitarProc"], glob["FX4WetSend"], "Mix", 0, 100}}
    };

    THR30II_INFO_ROOM =
    {
        {RO_DECAY    , {glob["FX4"], glob["Decay"], "Decay", 0, 100}},  
        {RO_TONE     , {glob["FX4"], glob["Tone"], "Tone", 0, 100}},  
        {RO_PREDELAY , {glob["FX4"], glob["PreDelay"], "Pre-Delay", 0, 100}}, 
        {RO_MIX      , {glob["GuitarProc"], glob["FX4WetSend"], "Mix", 0, 100}}
    };

    THR30II_INFO_REVERB =
    {
        { SPRING , { 
        
            {"REVERB", THR30II_INFO_SPRI[SP_REVERB]},
            {"TONE", THR30II_INFO_SPRI[SP_TONE]},
            {"MIX", THR30II_INFO_SPRI[SP_MIX]}  }
        }
        ,
        { PLATE , { 
        
            {"DECAY", THR30II_INFO_PLAT[PL_DECAY]},
            {"TONE", THR30II_INFO_PLAT[PL_TONE]},
            {"PREDELAY", THR30II_INFO_PLAT[PL_PREDELAY]},
            {"MIX", THR30II_INFO_PLAT[PL_MIX]}   }
        },
        { HALL , {
        
            {"DECAY", THR30II_INFO_HALL[HA_DECAY]},
            {"TONE", THR30II_INFO_HALL[HA_TONE]},
            {"PREDELAY", THR30II_INFO_HALL[HA_PREDELAY]},
            {"MIX", THR30II_INFO_HALL[HA_MIX]}  }
        },
        { ROOM  , {
        
            {"DECAY", THR30II_INFO_ROOM[RO_DECAY]},
            {"TONE", THR30II_INFO_ROOM[RO_TONE]},
            {"PREDELAY", THR30II_INFO_ROOM[RO_PREDELAY]},
            {"MIX", THR30II_INFO_ROOM[RO_MIX]}  }
        }
    };

    THR30II_GATE_VALS=
    {
        {GA_THRESHOLD, glob["Thresh"]}, 
        {GA_DECAY, glob["Decay"]} 
    };

    gateMap=
    {
        {glob["DecayState"], glob["Decay"]}, 
        {glob["ThreshState"], glob["Thresh"]} 
    };

    THR30II_CTRL_VALS=
    {
        {CTRL_GAIN   , glob["Drive"]},
        {CTRL_MASTER , glob["Master"]},
        {CTRL_BASS   , glob["Bass"]},
        {CTRL_MID    , glob["Mid"]},
        {CTRL_TREBLE , glob["Treble"]}
    };

    controlMap = 
    {
        {glob["MasterState"],  glob["Master"]},
        {glob["BassState"],  glob["Bass"]},
        {glob["MidState"],  glob["Mid"]},
        {glob["TrebleState"],  glob["Treble"]},
        {glob["DriveState"],  glob["Drive"]}    //Drive=Gain
    };
    
    THR30II_ECHO_TYPES_VALS =
    {
        {TAPE_ECHO, {glob["TapeEcho"], "Tape Echo"}},
        {DIGITAL_DELAY, {glob["L6DigitalDelay"],"Digital Delay"}}
    };

    echoMap=   //Map patch keys to MIDI keys
    {
        {glob["BassState"], glob["Bass"]}, 
        {glob["TrebleState"], glob["Treble"]},  
        {glob["FeedbackState"], glob["Feedback"]}, 
        {glob["SyncSelectState"], glob["SyncSelect"]}, //SyncSelectState=>SyncSelect  (Not in THR_Remote) 
        {glob["TimeState"], glob["Time"]}, 
        {glob["FX3MixState"], glob["FX3Mix"]} 
    };
    
    THR30II_INFO_TAPE= //Information about settings for echo type "Tape Echo":  (unit key,  settings key, name, lower limit, upper limit)
    {
        {TA_BASS, {glob["FX3"], glob["Bass"], "Bass", 0, 100}},
        {TA_TREBLE, {glob["FX3"], glob["Treble"], "Treble", 0, 100}},
        {TA_FEEDBACK, {glob["FX3"], glob["Feedback"], "Feedback", 0, 100}},
        {TA_MIX, {glob["GuitarProc"], glob["FX3Mix"], "Mix", 0, 100}}, 
        {TA_TIME, {glob["FX3"], glob["Time"], "Time", 0, 100}},
        {TA_SYNCSELECT, {glob["FX3"], glob["SyncSelect"], "SyncSelect", 0, 100}}
    };

    THR30II_INFO_DIGI= //Information about settings for echo type "Digital Delay":  (unit key,  settings key, name, lower limit, upper limit)
    {
        {DD_BASS, {glob["FX3"], glob["Bass"], "Bass", 0, 100}},
        {DD_TREBLE, {glob["FX3"], glob["Treble"], "Treble", 0, 100}},
        {DD_FEEDBACK, {glob["FX3"], glob["Feedback"], "Feedback", 0, 100}},
        {DD_MIX, {glob["GuitarProc"], glob["FX3Mix"], "Mix", 0, 100}},
        {DD_TIME, {glob["FX3"], glob["Time"], "Time", 0, 100}},
        {DD_SYNCSELECT, {glob["FX3"], glob["SyncSelect"], "SyncSelect", 0, 100}}
    };

    THR30II_INFO_ECHO=  //Information about settings depending on echo type:  (unit key,  settings key, name, lower limit, upper limit)
    {
        {TAPE_ECHO, {
            {
                {"BASS", THR30II_INFO_TAPE[TA_BASS]},
                {"TREBLE", THR30II_INFO_TAPE[TA_TREBLE]},
                {"FEEDBACK", THR30II_INFO_TAPE[TA_FEEDBACK]},
                {"TIME", THR30II_INFO_TAPE[TA_TIME]},
                {"MIX", THR30II_INFO_TAPE[TA_MIX]} }
            }
        }
        ,
        {DIGITAL_DELAY, {
            {
                {"BASS", THR30II_INFO_DIGI[DD_BASS]},
                {"TREBLE", THR30II_INFO_DIGI[DD_TREBLE]},
                {"FEEDBACK", THR30II_INFO_DIGI[DD_FEEDBACK]},
                {"TIME", THR30II_INFO_DIGI[DD_TIME]},
                {"MIX", THR30II_INFO_DIGI[DD_MIX]} }
            }
        }
    };

    THR30II_COMP_VALS=
    {
        {CO_SUSTAIN, glob["Sustain"]}, 
        {CO_LEVEL , glob["Level"]},
        {CO_MIX   , glob["FX1Mix"]}
    };

    compressorMap=
    {
        {glob["SustainState"],glob["Sustain"]}, //SustainState=> Sustain,
        {glob["LevelState"]  ,glob["Level"]  },  //LEVELSTATE=> Level,
        {glob["FX1MixState"] ,glob["FX1Mix"] }  //FX1MixState=> FX1Mix Compressor-Mix
    };
    
    effect_setting=
    {
        {
            {PHASER ,{ {PH_SPEED,    0.0}, {PH_FEEDBACK , 0.0}, {PH_MIX  , 0.0} }},
            {TREMOLO,{ {TR_SPEED,    0.0}, {TR_DEPTH    , 0.0}, {TR_MIX  , 0.0} }},
            {FLANGER,{ {FL_DEPTH,    0.0}, {FL_SPEED    , 0.0}, {FL_MIX  , 0.0} }},
            {CHORUS ,{ {CH_FEEDBACK, 0.0}, {CH_DEPTH    , 0.0}, {CH_SPEED, 0.0}, {CH_PREDELAY , 0.0}, {CH_MIX, 0.0} } }
        }
    };
            
}

