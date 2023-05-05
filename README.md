# THRII-direct-USB-pedalboard
A pedalboard that can send complete settings patches to a THRII guitar amplifier with direct USB-connection.


<p align="left">
  <img src="https://github.com/martinzw/THRII-direct-USB-pedalboard/blob/main/Pedalboard_top1.jpg" width="350" title="THRII-direct-USB-pedalboard" alt="The pedalboard (almost ready)">
</p>

*Note the disclaimer down this page!*

**Demonstration video of the prototype:**

[![IMAGE Demonstration video](https://img.youtube.com/vi/Kstgtiw6ibM/0.jpg)](https://www.youtube.com/watch?v=Kstgtiw6ibM)

The Yamaha THRII-series are small guitar practice amplifiers.

There is a smartphone app and a PC and Mac application to control all the sound parameters.
There even is a bluetooth connection for using foot switches with this amp. But you always need the smartphone app running to use a bluetooth foot switch.
Furthermore you can only switch some dedicated parameters with a bluetooth footswitch.

My project's goal is to avoid the sometimes not very stable bluetooth connection and connect directly to the amp via USB-cable.
It is possible to send whole "patch" settings including the complete set of parameters by pressing a button on the board with your foot.
(see demonstration video)

To achieve this, a Teensy 3.6 or 4.1 microcontroller board, that is capable of USB-host mode, connects to the THRII via the wirde USB interface.
It then activates the USB-MIDI-interface and communicates with the THRII by exchangig MIDI SystemExclusive messages.

On a TFT display the actual settings are shown and with foot switches a "patch" can be pre-selected from a library stored on the controller or a SD-card.

Patches can be pre-selected by choosing a group of 5 patches (e.g. containing 5 sounds for a song) and separately choosing a patch from that group. 
A "Patch-Send" button activates that special patch on the THRII.

A "SOLO" button either activates an altered sound patch fitting to the actual one or (if no dedicated solo patch exists) toggles a volume increasement.

<p align="left">
  <img src="https://github.com/martinzw/THRII-direct-USB-pedalboard/blob/main/Pedalboard_back2.jpg" width="350" title="THRII-direct-USB-pedalboard" alt="backside">
</p>

More information is available in the Wiki and in the pdf-document about the SysEx protocol.
<p align="left">
**Note for 07/27/2022**
A new firmware for the THR30II is online (1.43.0.b)
For this reason, the provided code was updated in some positions
</p>
<p align="left">
**Note for 02/16/2023**
A new firmware for the THR30II is online (1.44.0.a)
Code patched
</p>
<p align="left">
**Note for 03/29/2023**
Added a timeout mechanism for the outgoing message queue. This could be neccessary if a message, that expects an acknowledge and/or an answer is not released from the queue (e.g. if the acknowledging frame was not received for whatever reason that might have).
This could happen, if you use a volume-pedal at an anlog input of the Teensy, resulting in sending out a lot of parameter change messages in a short time period.
</p>
<p align="left">
**Note for 05/02/2023**
Added support for an analog GuitarVolume pedal at analog input A0.
The analog value at A0 should vary from 0V to 1.4V if the pedal (being simply an adjustable resistor of 47KOhms in my case) is plugged in and going higher to 1.7V if plugged out. I used an JVC foot pedal from an old broken electronik piano.
</p>
<p align="left">
**Note for 05/04/2023
The extended parameters AudioVolume and GuitarVolume are now shown as bars on the tft screen.
</p>
<p align="left">
**DISCLAIMER:**
THE HARDWARE SUGGESTIONS THE LISTED SOFTWARE AND ALL INFORMATION HERE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHOR OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE AND HARDWARE SUGGESTIONS OR THE USE OR OTHER DEALINGS IN THE SOFTWARE AND HARDWARE.

I have no connection to Yamaha / Line6. Every knowledge about the data communications protocol was gained by experimenting, try and error and by looking at the data stream. I especially have not decompiled any part of the Yamaha software application nor of the THR's firmware. Described protocol behaviour can be totally wrong. I only describe results from (good) guesses and succeded experiments - not facts!

I can not ensure, that sending messages in the suggested way will not damage the THRII. As well I can not guarantee, that the hardware, i describe will not damage your THRII device or it's power supply or your computer.
</p>
