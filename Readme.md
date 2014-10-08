# KSP DSKY
This project aims to implement an Apollo DSKY inspired interface to display 
orbital and vessel information in the computer game Kerbal Space Program

For information about the DSKY please check these websites:
- http://en.wikipedia.org/wiki/Apollo_Guidance_Computer
- http://www.ibiblio.org/apollo/yaDSKY.html

For information about Kerbal Space Program go to:
- https://www.kerbalspaceprogram.com/

## What this project is not
I'm not aiming for a faithful implementation of the Apollo DSKY. 

This project is solely used to display data from Kerbal Space Program onto
several 7-segment LED displays and bar graph displays. It provides a DSKY-like
keyboard which is used to manipulate what is shown on said displays.

This DSKY is not used to program a guidance computer (yet) so it is a 100%
passive device. You will not be able to control your craft inside the game
with this. If you want to create your own custom game controller you might
find inspiration in my other project: 

https://github.com/MrOnak/avr_usbhid_gamepanel

This implements a USB human-interface-device based on an Atmel ATMega328p
which identifies itself as joystick/keyboard combination offering 32 keyboard
keys and a joystick with 6 analogue axis and 4 buttons.

## Status Quo
- I have a browser-based implementation in JavaScript working.
- The C code in this repository is a port of that JavaScript code.
- The compiled C code is too large to flash onto an ATMega328p.
- Current work in progress is to compress the C code until it fits inside 32kb

# LICENSES
The KSP DSKY project is licenced under GNU General Public License, version 2.

The full text of the license can be found in the License.txt file and online:
    http://opensource.org/licenses/GPL-2.0

# Acknowledgements
./uart

The UART library in use is based on a library written by Peter Fleury 
    <pfleury@gmx.ch>   http://jump.to/fleury
    his code was published under GNU General Public License, version 2.

The library was further extended by Tim Sharpe to include additional functions 
    such as uart_available() and uart_flush(). (See uart.h for details).
    His work is released under the GNU General Public License.

Mr Sharpe's additions in turn are based on the Arduino HardwareSerial.h,
    released under LGPL, version 2.1.
	
The full text of the license can be found in the ./uart folder and online:
    http://opensource.org/licenses/GPL-2.0
	
./kspio

The code in the kspio folder is taken from my KSPIO port to AVR C:
https://github.com/MrOnak/avr_KSPIO

It is licensed under Creative Commons Attribution (BY).

The full text of the license can be found in the ./kspio folder and online:
    http://creativecommons.org/licenses/by/4.0/legalcode

The original KSPIO code was developed as Arduino .ino code by 'Zitronen'
under Creative Commons Attribution (BY). The development thread is here:
http://forum.kerbalspaceprogram.com/threads/66393-Hardware-Plugin-Arduino-based-physical-display-serial-port-io-tutorial-%2808-Sep%29

