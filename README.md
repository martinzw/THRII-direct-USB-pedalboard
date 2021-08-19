# THRII-direct-USB-pedalboard
A pedalboard that can send complete settings patches to a THRII guitar amplifier with direct USB-connection.

*Note the disclaimer down this page!*

**Demonstration video:**


[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/Kstgtiw6ibM/0.jpg)](https://www.youtube.com/watch?v=Kstgtiw6ibM)

The Yamaha THRII-series are small guitar practice amplifiers.

There is a smartphone app and a PC and Mac application to control all the sound parameters.
There even is a bluetooth connection for using foot switches with this amp. But you always need the smartphone app running to use a bluetooth foot switch.
Furthermore you can only switch some dedicated parameters with a bluetooth footswitch.

My project's goal is to avoid the sometimes not very stable bluetooth connection and connect directly to the amp via USB-cable.
It is possible to send whole "patch" settings including a complete set of parameters by pressing a button on the board with your foot.
(see demonstration video)

To achieve this, a Teensy 3.6 or 4.1 microcontroller board, that is capable of USB-host mode, connects to the THRII via the wirde USB interface.
It then activates the USB-MIDI-interface and communicates with the THRII by exchangig MIDI SystemExclusive messages.

On a TFT display the actual settings are shown and with foot switches a "patch" can be pre-selected from a library stored on the controller or a SD-card.

Patches can be pre-selected by choosing a group of 5 patches (e.g. containing 5 sounds for a song) and separately choosing a patch from that group. 
A "Patch-Send" button activates that special patch on the THRII.

A "SOLO" button either activates an altered sound patch fitting to the actual one or (if no dedicated solo patch exists) toggles a volume increasement.

**DISCLAIMER:**
THE HARDWARE SUGGESTIONS THE LISTED SOFTWARE AND ALL INFORMATION HERE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHOR OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE AND HARDWARE SUGGESTIONS OR THE USE OR OTHER DEALINGS IN THE SOFTWARE AND HARDWARE.

I have no connection to Yamaha / Line6. Every knowledge about the data communications protocol was gained by experimenting, try and error and by looking at the data stream. I especially have not decompiled any part of the Yamaha software application nor of the THR's firmware. Described protocol behaviour can be totally wrong. I only describe results from (good) guesses and succeded experiments - not facts!

I can not ensure, that sending messages in the suggested way will not damage the THRII. As well I can not guarantee, that the hardware, i describe will not damage your THRII device or it's power supply or your computer.

