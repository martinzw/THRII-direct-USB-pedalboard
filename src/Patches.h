#include <string>
#include <vector>


//Patches statically compiled to PROGMEM, if no SD-Card is used

std::vector<std::string> patchesII PROGMEM = 
{ 
 //0 "Banjo2"
 R"({
   "data" : {
   "device" : 2359298,
   "device_version" : 19923043,
   "meta" : {
      "name" : "Banjo2",
      "tnid" : 0
   },
   "tone" : {
      "THRGroupAmp" : {
      "@asset" : "THR10C_Deluxe",
      "Bass" : 0.0,
      "Drive" : 0.779334,
      "Master" : 0.697666,
      "Mid" : 1.0,
      "Treble" : 0.0457521
      },
      "THRGroupCab" : {
      "@asset" : "speakerSimulator",
      "SpkSimType" : 14
      },
      "THRGroupFX1Compressor" : {
      "@asset" : "RedComp",
      "@enabled" : true,
      "Level" : 0.556085,
      "Sustain" : 0.439683
      },
      "THRGroupFX2Effect" : {
      "@asset" : "Phaser",
      "@enabled" : false,
      "@wetDry" : 0.50,
      "Feedback" : 0.948360,
      "Speed" : 0.040
      },
      "THRGroupFX3EffectEcho" : {
      "@asset" : "TapeEcho",
      "@enabled" : true,
      "@wetDry" : 0.386243,
      "Bass" : 0.0,
      "Feedback" : 0.423281,
      "Time" : 0.0,
      "Treble" : 0.0423280
      },
      "THRGroupFX4EffectReverb" : {
      "@asset" : "LargePlate1",
      "@enabled" : true,
      "@wetDry" : 0.337566,
      "Decay" : 0.0666665,
      "PreDelay" : 0.0708995,
      "Tone" : 0.570370
      },
      "THRGroupGate" : {
      "@asset" : "noiseGate",
      "@enabled" : true,
      "Decay" : 0.104762,
      "Thresh" : -34.2920
      },
      "global" : {
      "THRPresetParamTempo" : 110
      }
   }
   },
   "meta" : {
   "original" : 0,
   "pbn" : 0,
   "premium" : 0
   },
   "schema" : "L6Preset",
   "version" : 5
   })"
 ,
 //1 "Blood Money"
 R"({
   "data" : {
   "device" : 2359298,
   "device_version" : 19988587,
   "meta" : {
      "name" : "Blood Money",
      "tnid" : 0
   },
   "tone" : {
      "THRGroupAmp" : {
      "@asset" : "THR30_Blondie",
      "Bass" : 0.380392,
      "Drive" : 0.778302,
      "Master" : 0.418170,
      "Mid" : 0.549020,
      "Treble" : 0.658824
      },
      "THRGroupCab" : {
      "@asset" : "speakerSimulator",
      "SpkSimType" : 3
      },
      "THRGroupFX1Compressor" : {
      "@asset" : "RedComp",
      "@enabled" : false,
      "Level" : 0.830,
      "Sustain" : 0.30
      },
      "THRGroupFX2Effect" : {
      "@asset" : "StereoSquareChorus",
      "@enabled" : true,
      "@wetDry" : 0.557907,
      "Depth" : 0.118372,
      "Feedback" : 0.155767,
      "Freq" : 0.234744,
      "Pre" : 0.50
      },
      "THRGroupFX3EffectEcho" : {
      "@asset" : "TapeEcho",
      "@enabled" : false,
      "@wetDry" : 0.397647,
      "Bass" : 0.217209,
      "Feedback" : 0.355609,
      "Time" : 0.3910,
      "Treble" : 0.346133
      },
      "THRGroupFX4EffectReverb" : {
      "@asset" : "LargePlate1",
      "@enabled" : true,
      "@wetDry" : 0.30,
      "Decay" : 0.150,
      "PreDelay" : 0.30,
      "Tone" : 0.708472
      },
      "THRGroupGate" : {
      "@asset" : "noiseGate",
      "@enabled" : false,
      "Decay" : 0.2050,
      "Thresh" : -33.0
      },
      "global" : {
      "THRPresetParamTempo" : 110
      }
   }
   },
   "meta" : {
   "original" : 0,
   "pbn" : 0,
   "premium" : 0
   },
   "schema" : "L6Preset",
   "version" : 5
   }
   )"
 ,
 //2  "Passenger"
   R"(
   {
   "data" : {
   "device" : 2359298,
   "device_version" : 19923043,
   "meta" : {
      "name" : "Passenger",
      "tnid" : 0
   },
   "tone" : {
      "THRGroupAmp" : {
      "@asset" : "THR10C_DC30",
      "Bass" : 0.384314,
      "Drive" : 0.934858,
      "Master" : 0.874074,
      "Mid" : 0.501961,
      "Treble" : 0.454902
      },
      "THRGroupCab" : {
      "@asset" : "speakerSimulator",
      "SpkSimType" : 0
      },
      "THRGroupFX1Compressor" : {
      "@asset" : "RedComp",
      "@enabled" : true,
      "Level" : 0.515344,
      "Sustain" : 0.439683
      },
      "THRGroupFX2Effect" : {
      "@asset" : "Phaser",
      "@enabled" : false,
      "@wetDry" : 0.50,
      "Feedback" : 0.948360,
      "Speed" : 0.040
      },
      "THRGroupFX3EffectEcho" : {
      "@asset" : "TapeEcho",
      "@enabled" : false,
      "@wetDry" : 0.386243,
      "Bass" : 0.0,
      "Feedback" : 0.445503,
      "Time" : 0.0,
      "Treble" : 0.0423280
      },
      "THRGroupFX4EffectReverb" : {
      "@asset" : "SmallRoom1",
      "@enabled" : true,
      "@wetDry" : 0.337566,
      "Decay" : 0.50,
      "PreDelay" : 0.50,
      "Tone" : 0.50
      },
      "THRGroupGate" : {
      "@asset" : "noiseGate",
      "@enabled" : false,
      "Decay" : 0.60,
      "Thresh" : -38.0
      },
      "global" : {
      "THRPresetParamTempo" : 110
      }
   }
   },
   "meta" : {
   "original" : 0,
   "pbn" : 0,
   "premium" : 0
   },
   "schema" : "L6Preset",
   "version" : 5
   }

   )"
 ,
 //3 "Clean Blues"
  R"({
  "data" : {
    "device" : 2359298,
    "device_version" : 19923043,
    "meta" : {
    "name" : "001 - Clean Blues",
    "tnid" : 0
    },
    "tone" : {
    "THRGroupAmp" : {
      "@asset" : "THR10C_BJunior2",
      "Bass" : 0.354031,
      "Drive" : 0.401307,
      "Master" : 0.803704,
      "Mid" : 0.808497,
      "Treble" : 0.653159
    },
    "THRGroupCab" : {
      "@asset" : "speakerSimulator",
      "SpkSimType" : 3
    },
    "THRGroupFX1Compressor" : {
      "@asset" : "RedComp",
      "@enabled" : true,
      "Level" : 0.607778,
      "Sustain" : 0.60
    },
    "THRGroupFX2Effect" : {
      "@asset" : "StereoSquareChorus",
      "@enabled" : false,
      "@wetDry" : 0.429216,
      "Depth" : 0.249420,
      "Feedback" : 0.122343,
      "Freq" : 0.113012,
      "Pre" : 0.50
    },
    "THRGroupFX3EffectEcho" : {
      "@asset" : "TapeEcho",
      "@enabled" : true,
      "@wetDry" : 0.204662,
      "Bass" : 0.307407,
      "Feedback" : 0.303542,
      "Time" : 0.153963,
      "Treble" : 0.302986
    },
    "THRGroupFX4EffectReverb" : {
      "@asset" : "LargePlate1",
      "@enabled" : true,
      "@wetDry" : 0.153355,
      "Decay" : 0.151852,
      "PreDelay" : 0.155556,
      "Tone" : 0.50
    },
    "THRGroupGate" : {
      "@asset" : "noiseGate",
      "@enabled" : true,
      "Decay" : 0.2050,
      "Thresh" : -47.2222
    },
    "global" : {
      "THRPresetParamTempo" : 110
    }
    }
  },
  "meta" : {
    "original" : 0,
    "pbn" : 0,
    "premium" : 0
  },
  "schema" : "L6Preset",
  "version" : 5
  }
  )"
 ,
 //4 "Money for Nothing"
   R"(
   {
   "data" : {
   "device" : 2359298,
   "device_version" : 19923043,
   "meta" : {
      "name" : "Money for Nothing",
      "tnid" : 0
   },
   "tone" : {
      "THRGroupAmp" : {
      "@asset" : "THR10X_South",
      "Bass" : 0.883660,
      "Drive" : 1.0,
      "Master" : 1.0,
      "Mid" : 0.0,
      "Treble" : 0.248148
      },
      "THRGroupCab" : {
      "@asset" : "speakerSimulator",
      "SpkSimType" : 3
      },
      "THRGroupFX1Compressor" : {
      "@asset" : "RedComp",
      "@enabled" : false,
      "Level" : 0.830,
      "Sustain" : 0.30
      },
      "THRGroupFX2Effect" : {
      "@asset" : "Phaser",
      "@enabled" : false,
      "@wetDry" : 0.50,
      "Feedback" : 0.948360,
      "Speed" : 0.040
      },
      "THRGroupFX3EffectEcho" : {
      "@asset" : "TapeEcho",
      "@enabled" : true,
      "@wetDry" : 0.288888,
      "Bass" : 0.60,
      "Feedback" : 0.501059,
      "Time" : 0.265556,
      "Treble" : 0.0423280
      },
      "THRGroupFX4EffectReverb" : {
      "@asset" : "StandardSpring",
      "@enabled" : true,
      "@wetDry" : 0.411111,
      "Time" : 0.0574074,
      "Tone" : 0.474074
      },
      "THRGroupGate" : {
      "@asset" : "noiseGate",
      "@enabled" : true,
      "Decay" : 0.349445,
      "Thresh" : -23.8222
      },
      "global" : {
      "THRPresetParamTempo" : 110
      }
   }
   },
   "meta" : {
   "original" : 0,
   "pbn" : 0,
   "premium" : 0
   },
   "schema" : "L6Preset",
   "version" : 5
   }

   )"
 ,
 //5 "Crossroads"
 R"(
   {
   "data" : {
   "device" : 2359298,
   "device_version" : 19923043,
   "meta" : {
      "name" : "Crossroads",
      "tnid" : 0
   },
   "tone" : {
      "THRGroupAmp" : {
      "@asset" : "THR30_FLead",
      "Bass" : 0.713726,
      "Drive" : 0.698039,
      "Master" : 0.917647,
      "Mid" : 0.501961,
      "Treble" : 0.133333
      },
      "THRGroupCab" : {
      "@asset" : "speakerSimulator",
      "SpkSimType" : 4
      },
      "THRGroupFX1Compressor" : {
      "@asset" : "RedComp",
      "@enabled" : false,
      "Level" : 0.830,
      "Sustain" : 0.30
      },
      "THRGroupFX2Effect" : {
      "@asset" : "StereoSquareChorus",
      "@enabled" : false,
      "@wetDry" : 0.408627,
      "Depth" : 0.272520,
      "Feedback" : 0.114299,
      "Freq" : 0.108071,
      "Pre" : 0.50
      },
      "THRGroupFX3EffectEcho" : {
      "@asset" : "TapeEcho",
      "@enabled" : false,
      "@wetDry" : 0.180,
      "Bass" : 0.50,
      "Feedback" : 0.323528,
      "Time" : 0.3910,
      "Treble" : 0.251544
      },
      "THRGroupFX4EffectReverb" : {
      "@asset" : "ReallyLargeHall",
      "@enabled" : false,
      "@wetDry" : 0.785294,
      "Decay" : 0.428235,
      "PreDelay" : 0.10,
      "Tone" : 0.233812
      },
      "THRGroupGate" : {
      "@asset" : "noiseGate",
      "@enabled" : true,
      "Decay" : 0.2050,
      "Thresh" : -33.0
      },
      "global" : {
      "THRPresetParamTempo" : 110
      }
   }
   },
   "meta" : {
   "original" : 0,
   "pbn" : 0,
   "premium" : 0
   },
   "schema" : "L6Preset",
   "version" : 5
   }
   )"
 ,
 //6 "Strat Rocker"
 R"(
   {
   "data" : {
      "meta" : {
         "name" : "Strat Rocker",
         "tnid" : 0
      },
      "device" : 2359298,
      "tone" : {
         "THRGroupGate" : {
         "Decay" : 0.20499999821186066,
         "@enabled" : false,
         "@asset" : "noiseGate",
         "Thresh" : -18.094444274902344
         },
         "THRGroupFX3EffectEcho" : {
         "@asset" : "TapeEcho",
         "Time" : 0.47142362594604492,
         "Bass" : 0.20057870447635651,
         "Treble" : 0.26224517822265625,
         "@wetDry" : 0.19725489616394043,
         "@enabled" : false,
         "Feedback" : 0.3257642388343811
         },
         "global" : {
         "THRPresetParamTempo" : 110
         },
         "THRGroupFX1Compressor" : {
         "@enabled" : false,
         "Sustain" : 0.52764755487442017,
         "@asset" : "RedComp",
         "Level" : 0.62625283002853394
         },
         "THRGroupFX4EffectReverb" : {
         "@wetDry" : 0.065294116735458374,
         "Decay" : 0.15000000596046448,
         "@enabled" : false,
         "PreDelay" : 0.30000001192092896,
         "@asset" : "LargePlate1",
         "Tone" : 0.85711181163787842
         },
         "THRGroupCab" : {
         "@asset" : "speakerSimulator",
         "SpkSimType" : 3
         },
         "THRGroupFX2Effect" : {
         "@asset" : "StereoSquareChorus",
         "Depth" : 0.26811999082565308,
         "Freq" : 0.10901176929473877,
         "Pre" : 0.5,
         "@wetDry" : 0.41254901885986328,
         "@enabled" : false,
         "Feedback" : 0.1158311516046524
         },
         "THRGroupAmp" : {
         "Treble" : 0.71407866477966309,
         "Bass" : 0.45882353186607361,
         "Master" : 0.72156864404678345,
         "@asset" : "THR10X_Brown2",
         "Drive" : 0.36078432202339172,
         "Mid" : 0.67058825492858887
         }
      },
      "device_version" : 19988587
   },
   "meta" : {
      "original" : 0,
      "pbn" : 0,
      "premium" : 0
   },
   "schema" : "L6Preset",
   "version" : 5
   }
)"
,
//7 "Thrill"
R"({
 "data" : {
  "device" : 2359298,
  "device_version" : 19923043,
  "meta" : {
   "name" : "Thrill",
   "tnid" : 0
  },
  "tone" : {
   "THRGroupAmp" : {
    "@asset" : "THR10C_DC30",
    "Bass" : 0.596296,
    "Drive" : 0.892592,
    "Master" : 0.733334,
    "Mid" : 0.718518,
    "Treble" : 0.137037
   },
   "THRGroupCab" : {
    "@asset" : "speakerSimulator",
    "SpkSimType" : 10
   },
   "THRGroupFX1Compressor" : {
    "@asset" : "RedComp",
    "@enabled" : true,
    "Level" : 0.533333,
    "Sustain" : 0.30
   },
   "THRGroupFX2Effect" : {
    "@asset" : "StereoSquareChorus",
    "@enabled" : false,
    "@wetDry" : 0.50,
    "Depth" : 0.20,
    "Feedback" : 0.130,
    "Freq" : 0.040,
    "Pre" : 0.50
   },
   "THRGroupFX3EffectEcho" : {
    "@asset" : "TapeEcho",
    "@enabled" : false,
    "@wetDry" : 0.50,
    "Bass" : 0.50,
    "Feedback" : 0.630,
    "Time" : 0.3910,
    "Treble" : 0.0
   },
   "THRGroupFX4EffectReverb" : {
    "@asset" : "StandardSpring",
    "@enabled" : true,
    "@wetDry" : 0.303704,
    "Time" : 0.205556,
    "Tone" : 0.50
   },
   "THRGroupGate" : {
    "@asset" : "noiseGate",
    "@enabled" : true,
    "Decay" : 0.60,
    "Thresh" : -38.0
   },
   "global" : {
    "THRPresetParamTempo" : 110
   }
  }
 },
 "meta" : {
  "original" : 0,
  "pbn" : 0,
  "premium" : 0
 },
 "schema" : "L6Preset",
 "version" : 5
}
)"
,
//8 "Angus"
R"({
  "data" : {
    "meta" : {
      "name" : "Angus",
      "tnid" : 0
    },
    "device" : 2359298,
    "tone" : {
      "THRGroupGate" : {
        "Decay" : 0.20499999821186066,
        "@enabled" : true,
        "@asset" : "noiseGate",
        "Thresh" : -16
      },
      "THRGroupFX3EffectEcho" : {
        "@asset" : "TapeEcho",
        "Time" : 0.2531510591506958,
        "Bass" : 0.39346066117286682,
        "Treble" : 0.26224517822265625,
        "@wetDry" : 0.19725489616394043,
        "@enabled" : false,
        "Feedback" : 0.3257642388343811
      },
      "global" : {
        "THRPresetParamTempo" : 110
      },
      "THRGroupFX1Compressor" : {
        "@enabled" : false,
        "Sustain" : 0.39503762125968933,
        "@asset" : "RedComp",
        "Level" : 0.60096347332000732
      },
      "THRGroupFX4EffectReverb" : {
        "@wetDry" : 0.056743763387203217,
        "Decay" : 0.078356482088565826,
        "@enabled" : true,
        "PreDelay" : 0.29435762763023376,
        "@asset" : "LargePlate1",
        "Tone" : 0.48835358023643494
      },
      "THRGroupCab" : {
        "@asset" : "speakerSimulator",
        "SpkSimType" : 5
      },
      "THRGroupFX2Effect" : {
        "@asset" : "StereoSquareChorus",
        "Depth" : 0.27252000570297241,
        "Freq" : 0.10807058960199356,
        "Pre" : 0.38349246978759766,
        "@wetDry" : 0.40862745046615601,
        "@enabled" : false,
        "Feedback" : 0.11429891735315323
      },
      "THRGroupAmp" : {
        "Treble" : 0.45239481329917908,
        "Bass" : 0.59064793586730957,
        "Master" : 0.65610533952713013,
        "@asset" : "THR10_Brit",
        "Drive" : 0.40791633725166321,
        "Mid" : 0.61073070764541626
      }
    },
    "device_version" : 19988587
  },
  "meta" : {
    "original" : 0,
    "pbn" : 0,
    "premium" : 0
  },
  "schema" : "L6Preset",
  "version" : 5
}
)"
,
//9 "Fav Blues"
R"({
  "data" : {
    "meta" : {
      "name" : "Fav Blues",
      "tnid" : 0
    },
    "device" : 2359298,
    "tone" : {
      "THRGroupGate" : {
        "Decay" : 0.5,
        "@enabled" : false,
        "@asset" : "noiseGate",
        "Thresh" : -32
      },
      "THRGroupFX3EffectEcho" : {
        "@asset" : "TapeEcho",
        "Time" : 0.2531510591506958,
        "Bass" : 0.39346066117286682,
        "Treble" : 0.26224517822265625,
        "@wetDry" : 0.18957260251045227,
        "@enabled" : true,
        "Feedback" : 0.192242830991745
      },
      "global" : {
        "THRPresetParamTempo" : 110
      },
      "THRGroupFX1Compressor" : {
        "@enabled" : false,
        "Sustain" : 0.21383102238178253,
        "@asset" : "RedComp",
        "Level" : 0.77584773302078247
      },
      "THRGroupFX4EffectReverb" : {
        "@wetDry" : 0.084796428680419922,
        "@enabled" : true,
        "Time" : 0.22864583134651184,
        "@asset" : "StandardSpring",
        "Tone" : 0.57520252466201782
      },
      "THRGroupCab" : {
        "@asset" : "speakerSimulator",
        "SpkSimType" : 7
      },
      "THRGroupFX2Effect" : {
        "@asset" : "StereoSquareChorus",
        "Depth" : 0.51049554347991943,
        "Freq" : 0.10901176929473877,
        "Pre" : 0.38349246978759766,
        "@wetDry" : 0.40375274419784546,
        "@enabled" : false,
        "Feedback" : 0.30618128180503845
      },
      "THRGroupAmp" : {
        "Treble" : 0.45651212334632874,
        "Bass" : 0.42907816171646118,
        "Master" : 0.94877022504806519,
        "@asset" : "THR30_JKBass2",
        "Drive" : 0.93376737833023071,
        "Mid" : 0.26014262437820435
      }
    },
    "device_version" : 19988587
  },
  "meta" : {
    "original" : 0,
    "pbn" : 0,
    "premium" : 0
  },
  "schema" : "L6Preset",
  "version" : 5
}
)"
,
//10 "Tele-Arp"
R"({
  "data" : {
    "meta" : {
      "name" : "Tele-Arp",
      "tnid" : 0
    },
    "device" : 2359298,
    "tone" : {
      "THRGroupGate" : {
        "Decay" : 0.20499999821186066,
        "@enabled" : false,
        "@asset" : "noiseGate",
        "Thresh" : -33
      },
      "THRGroupFX3EffectEcho" : {
        "@asset" : "TapeEcho",
        "Time" : 0.47142362594604492,
        "Bass" : 0.20057870447635651,
        "Treble" : 0.3364604115486145,
        "@wetDry" : 0.40388515591621399,
        "@enabled" : false,
        "Feedback" : 0.47538340091705322
      },
      "global" : {
        "THRPresetParamTempo" : 110
      },
      "THRGroupFX1Compressor" : {
        "@enabled" : false,
        "Sustain" : 0.52764755487442017,
        "@asset" : "RedComp",
        "Level" : 0.62625283002853394
      },
      "THRGroupFX4EffectReverb" : {
        "@wetDry" : 0.13852721452713013,
        "Decay" : 0.73440390825271606,
        "@enabled" : true,
        "PreDelay" : 0.31626155972480774,
        "@asset" : "SmallRoom1",
        "Tone" : 0.5
      },
      "THRGroupCab" : {
        "@asset" : "speakerSimulator",
        "SpkSimType" : 5
      },
      "THRGroupFX2Effect" : {
        "@asset" : "StereoSquareChorus",
        "Depth" : 0.26811999082565308,
        "Freq" : 0.10901176929473877,
        "Pre" : 0.5,
        "@wetDry" : 0.41254901885986328,
        "@enabled" : false,
        "Feedback" : 0.1158311516046524
      },
      "THRGroupAmp" : {
        "Treble" : 0.34214666485786438,
        "Bass" : 0.65340840816497803,
        "Master" : 0.76444464921951294,
        "@asset" : "THR30_Blondie",
        "Drive" : 0.34236794710159302,
        "Mid" : 0.33501330018043518
      }
    },
    "device_version" : 19988587
  },
  "meta" : {
    "original" : 0,
    "pbn" : 0,
    "premium" : 0
  },
  "schema" : "L6Preset",
  "version" : 5
}
)"
,
//11 "Following Echo"
R"({
 "data" : {
  "device" : 2359298,
  "device_version" : 20971617,
  "meta" : {
   "name" : "Following Echo",
   "tnid" : 0
  },
  "tone" : {
   "THRGroupAmp" : {
    "@asset" : "THR30_SR101",
    "Bass" : 0.388235,
    "Drive" : 0.545098,
    "Master" : 0.752912,
    "Mid" : 0.407843,
    "Treble" : 0.509804
   },
   "THRGroupCab" : {
    "@asset" : "speakerSimulator",
    "SpkSimType" : 12
   },
   "THRGroupFX1Compressor" : {
    "@asset" : "RedComp",
    "@enabled" : false,
    "Level" : 0.830,
    "Sustain" : 0.30
   },
   "THRGroupFX2Effect" : {
    "@asset" : "StereoSquareChorus",
    "@enabled" : false,
    "@wetDry" : 0.268861,
    "Depth" : 0.414677,
    "Feedback" : 0.107841,
    "Freq" : 0.200665,
    "Pre" : 0.245240
   },
   "THRGroupFX3EffectEcho" : {
    "@asset" : "L6DigitalDelay",
    "@enabled" : true,
    "@wetDry" : 0.485185,
    "Bass" : 0.233333,
    "Feedback" : 0.217037,
    "Time" : 0.3910,
    "Treble" : 0.555556
   },
   "THRGroupFX4EffectReverb" : {
    "@asset" : "LargePlate1",
    "@enabled" : true,
    "@wetDry" : 0.0722367,
    "Decay" : 0.232567,
    "PreDelay" : 0.216999,
    "Tone" : 0.50
   },
   "THRGroupGate" : {
    "@asset" : "noiseGate",
    "@enabled" : false,
    "Decay" : 0.60,
    "Thresh" : -38.0
   },
   "global" : {
    "THRPresetParamTempo" : 110
   }
  }
 },
 "meta" : {
  "original" : 0,
  "pbn" : 0,
  "premium" : 0
 },
 "schema" : "L6Preset",
 "version" : 5
}
)"
,
//12 "Solid Chorus"
R"({
 "data" : {
  "device" : 2359298,
  "device_version" : 20971617,
  "meta" : {
   "name" : "Solid Chorus",
   "tnid" : 0
  },
  "tone" : {
   "THRGroupAmp" : {
    "@asset" : "THR10_Flat_V",
    "Bass" : 0.388235,
    "Drive" : 0.758886,
    "Master" : 0.849599,
    "Mid" : 0.407843,
    "Treble" : 0.587148
   },
   "THRGroupCab" : {
    "@asset" : "speakerSimulator",
    "SpkSimType" : 16
   },
   "THRGroupFX1Compressor" : {
    "@asset" : "RedComp",
    "@enabled" : false,
    "Level" : 0.830,
    "Sustain" : 0.30
   },
   "THRGroupFX2Effect" : {
    "@asset" : "StereoSquareChorus",
    "@enabled" : true,
    "@wetDry" : 0.50,
    "Depth" : 0.321224,
    "Feedback" : 0.185758,
    "Freq" : 0.0772974,
    "Pre" : 0.50
   },
   "THRGroupFX3EffectEcho" : {
    "@asset" : "L6DigitalDelay",
    "@enabled" : true,
    "@wetDry" : 0.366667,
    "Bass" : 0.603704,
    "Feedback" : 0.354074,
    "Time" : 0.3910,
    "Treble" : 0.411111
   },
   "THRGroupFX4EffectReverb" : {
    "@asset" : "ReallyLargeHall",
    "@enabled" : true,
    "@wetDry" : 0.595790,
    "Decay" : 0.373452,
    "PreDelay" : 0.373090,
    "Tone" : 0.351577
   },
   "THRGroupGate" : {
    "@asset" : "noiseGate",
    "@enabled" : false,
    "Decay" : 0.50,
    "Thresh" : -32.0
   },
   "global" : {
    "THRPresetParamTempo" : 780
   }
  }
 },
 "meta" : {
  "original" : 0,
  "pbn" : 0,
  "premium" : 0
 },
 "schema" : "L6Preset",
 "version" : 5
}
)"
,
//13 "American Clean Combo"
R"({
  "data" : {
    "meta" : {
      "name" : "American Clean Combo",
      "tnid" : 0
    },
    "device" : 2359298,
    "tone" : {
      "THRGroupGate" : {
        "Decay" : 0.60000002384185791,
        "@enabled" : false,
        "@asset" : "noiseGate",
        "Thresh" : -38
      },
      "THRGroupFX3EffectEcho" : {
        "@asset" : "TapeEcho",
        "Time" : 0.41547450423240662,
        "Bass" : 0.43097510933876038,
        "Treble" : 0,
        "@wetDry" : 0.18551149964332581,
        "@enabled" : false,
        "Feedback" : 0.26153844594955444
      },
      "global" : {
        "THRPresetParamTempo" : 780
      },
      "THRGroupFX1Compressor" : {
        "@enabled" : false,
        "Sustain" : 0.30000001192092896,
        "@asset" : "RedComp",
        "Level" : 0.82999998331069946
      },
      "THRGroupFX4EffectReverb" : {
        "@wetDry" : 0.28699362277984619,
        "Decay" : 0.61193573474884033,
        "@enabled" : false,
        "PreDelay" : 0.41723090410232544,
        "@asset" : "ReallyLargeHall",
        "Tone" : 0.43255206942558289
      },
      "THRGroupCab" : {
        "@asset" : "speakerSimulator",
        "SpkSimType" : 7
      },
      "THRGroupFX2Effect" : {
        "@asset" : "StereoSquareChorus",
        "Depth" : 0.41467744112014771,
        "Freq" : 0.20066490769386292,
        "Pre" : 0.24524016678333282,
        "@wetDry" : 0.26886078715324402,
        "@enabled" : false,
        "Feedback" : 0.10784143209457397
      },
      "THRGroupAmp" : {
        "Treble" : 0.53940457105636597,
        "Bass" : 0.59243088960647583,
        "Master" : 0.68022704124450684,
        "@asset" : "THR10C_Deluxe",
        "Drive" : 0.56334173679351807,
        "Mid" : 0.63690853118896484
      }
    },
    "device_version" : 18088037
  },
  "meta" : {
    "original" : 0,
    "pbn" : 0,
    "premium" : 0
  },
  "schema" : "L6Preset",
  "version" : 5
}
)"
,
//14 "SilverMountain"
R"(
   {
  "data" : {
    "meta" : {
      "name" : "Silver Mountain",
      "tnid" : 0
    },
    "device" : 2359298,
    "tone" : {
      "THRGroupGate" : {
        "Decay" : 0.20499999821186066,
        "@enabled" : false,
        "@asset" : "noiseGate",
        "Thresh" : -33
      },
      "THRGroupFX3EffectEcho" : {
        "@asset" : "TapeEcho",
        "Time" : 0.39100000262260437,
        "Bass" : 0.5,
        "Treble" : 0.26224517822265625,
        "@wetDry" : 0.19725489616394043,
        "@enabled" : false,
        "Feedback" : 0.3257642388343811
      },
      "global" : {
        "THRPresetParamTempo" : 110
      },
      "THRGroupFX1Compressor" : {
        "@enabled" : false,
        "Sustain" : 0.30000001192092896,
        "@asset" : "RedComp",
        "Level" : 0.82999998331069946
      },
      "THRGroupFX4EffectReverb" : {
        "@wetDry" : 0.076330341398715973,
        "Decay" : 0.15000000596046448,
        "@enabled" : true,
        "PreDelay" : 0.30000001192092896,
        "@asset" : "LargePlate1",
        "Tone" : 0.86664116382598877
      },
      "THRGroupCab" : {
        "@asset" : "speakerSimulator",
        "SpkSimType" : 3
      },
      "THRGroupFX2Effect" : {
        "@asset" : "StereoSquareChorus",
        "Depth" : 0.23754000663757324,
        "Freq" : 0.11555293947458267,
        "Pre" : 0.5,
        "@wetDry" : 0.43980392813682556,
        "@enabled" : false,
        "Feedback" : 0.12648019194602966
      },
      "THRGroupAmp" : {
        "Treble" : 0.51372551918029785,
        "Bass" : 0.61960786581039429,
        "Master" : 1,
        "@asset" : "THR30_Blondie",
        "Drive" : 1,
        "Mid" : 0.69411766529083252
      }
    },
    "device_version" : 18088037
  },
  "meta" : {
    "original" : 0,
    "pbn" : 0,
    "premium" : 0
  },
  "schema" : "L6Preset",
  "version" : 5
}
)",
//15 "EarlyVH"
R"(
{
 "data" : {
  "device" : 2359298,
  "device_version" : 20971617,
  "meta" : {
   "name" : "EarlyVH",
   "tnid" : 0
  },
  "tone" : {
   "THRGroupAmp" : {
    "@asset" : "THR10X_Brown1",
    "Bass" : 0.409973,
    "Drive" : 0.525651,
    "Master" : 0.689062,
    "Mid" : 0.694118,
    "Treble" : 0.745062
   },
   "THRGroupCab" : {
    "@asset" : "speakerSimulator",
    "SpkSimType" : 2
   },
   "THRGroupFX1Compressor" : {
    "@asset" : "RedComp",
    "@enabled" : false,
    "Level" : 0.830,
    "Sustain" : 0.30
   },
   "THRGroupFX2Effect" : {
    "@asset" : "Phaser",
    "@enabled" : false,
    "@wetDry" : 0.334071,
    "Feedback" : 0.434175,
    "Speed" : 0.0144242
   },
   "THRGroupFX3EffectEcho" : {
    "@asset" : "TapeEcho",
    "@enabled" : true,
    "@wetDry" : 0.212781,
    "Bass" : 0.50,
    "Feedback" : 0.344024,
    "Time" : 0.422019,
    "Treble" : 0.320351
   },
   "THRGroupFX4EffectReverb" : {
    "@asset" : "LargePlate1",
    "@enabled" : true,
    "@wetDry" : 0.195660,
    "Decay" : 0.237066,
    "PreDelay" : 0.314381,
    "Tone" : 0.536849
   },
   "THRGroupGate" : {
    "@asset" : "noiseGate",
    "@enabled" : true,
    "Decay" : 0.2050,
    "Thresh" : -33.0
   },
   "global" : {
    "THRPresetParamTempo" : 110
   }
  }
 },
 "meta" : {
  "original" : 0,
  "pbn" : 0,
  "premium" : 0
 },
 "schema" : "L6Preset",
 "version" : 5
}
)"
,
//16 "That Humbucker Dude"
R"(
 {
  "data" : {
    "meta" : {
      "name" : "That Humbucker Dude",
      "tnid" : 0
    },
    "device" : 2359298,
    "tone" : {
      "THRGroupGate" : {
        "Decay" : 0.20499999821186066,
        "@enabled" : false,
        "@asset" : "noiseGate",
        "Thresh" : -33
      },
      "THRGroupFX3EffectEcho" : {
        "@asset" : "TapeEcho",
        "Time" : 0.60699999332427979,
        "Bass" : 0.55782699584960938,
        "Treble" : 0.3028811514377594,
        "@wetDry" : 0.34394592046737671,
        "@enabled" : false,
        "Feedback" : 0.38064959645271301
      },
      "global" : {
        "THRPresetParamTempo" : 607
      },
      "THRGroupFX1Compressor" : {
        "@enabled" : false,
        "Sustain" : 0.5189814567565918,
        "@asset" : "RedComp",
        "Level" : 0.62625283002853394
      },
      "THRGroupFX4EffectReverb" : {
        "@wetDry" : 0.5,
        "Decay" : 0.5,
        "@enabled" : false,
        "PreDelay" : 0.5,
        "@asset" : "ReallyLargeHall",
        "Tone" : 0.5
      },
      "THRGroupCab" : {
        "@asset" : "speakerSimulator",
        "SpkSimType" : 3
      },
      "THRGroupFX2Effect" : {
        "@asset" : "StereoSquareChorus",
        "Depth" : 0.33108294010162354,
        "Freq" : 0.18709336221218109,
        "Pre" : 0.27825519442558289,
        "@wetDry" : 0.35650157928466797,
        "@enabled" : false,
        "Feedback" : 0.1158311516046524
      },
      "THRGroupAmp" : {
        "Treble" : 0.68922162055969238,
        "Bass" : 0.41664204001426697,
        "Master" : 0.63174188137054443,
        "@asset" : "THR10X_Brown1",
        "Drive" : 0.60865336656570435,
        "Mid" : 0.50002044439315796
      }
    },
    "device_version" : 18088037
  },
  "meta" : {
    "original" : 0,
    "pbn" : 0,
    "premium" : 0
  },
  "schema" : "L6Preset",
  "version" : 5
}  
)"

};

