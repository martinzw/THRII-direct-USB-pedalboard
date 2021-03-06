**Target device**

The provided source code is intended for a Teensy 3.6 controller. It should work for the more powerful (but a bit cheaper) Teensy 4.1 controller as well, but i have not tested it.
The Teensy 4.0 controller does not have a SD-card reader on board but could work as well, if you do not need SD card.
The Teensy 3.5 can not run the powerful Teensy3.6 USB-Host library and will not work for this project.

**Programming language**

-The software is written in C++ (-std=c++17)

**Development Environment**

-I use "PlatformIO" IDE as a plugin of "VisualStudio Code". You will have to install/use the "Teensy" Platform inside PlatformIO as well as the board "Teensy3.6" and the "arduino" framework.


**Libraries**

-*"ArduinoQueue"* Copyright (c) 2018 Einar Arnason (MIT License)

-*"SdFat"* Copyright (c) 2011..2020 Bill Greiman   (MIT License)

-*"USBHost_t36"* Copyright (c)t 2017 Paul Stoffregen

-*"2_8_Friendly_t3"* based on "ST77889 TFT Display Driver" by Brian L. Clevenger 2019-02-22  Requires Adafruit GFX Driver (MIT license)

- *"Bounce2"*  Copyright (c) 2013 thomasfredericks (MIT License) Web: https://github.com/thomasfredericks/Bounce2

- *"map", "array", "string", "vector", "algorithm>"*  of GNU C++  

- *"Teensy 3.x RAM Monitor"* Copyright (c) by Adrian Hunt 2015 - 2016  (not needed, but can be helpful to find memory leaks during development)

- *"ArduinoJson"* Copyright (c) Benoit Blanchon 2014-2021 https://arduinojson.org 

- *"SPI"* version=1.0  Web: http://www.arduino.cc/en/Reference/SPI 

**Software history and structure**

A lot of knowledge about the SysEx protocol came up while programming and was not available before i started.
I would have designed the program quite different otherwise.
Do not wonder about weiry design for this reason...

Because testing is easier on a PC I started with a desktop application in C#.
Some techniques i used in C# were not possible this way in C++ on a microcontroller.
I had to avoid heap storage where ever possible und could not make extensive use of STL containers.

Some complexity results from the program first being designed for the THR10, where protocol is very different.

Another reason for complexity is the symbol table.
It is received from the THR30II in startup.
So the symbols must be hold in dynamic data structures.




