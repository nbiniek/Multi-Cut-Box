# Multi-Cut-Box
The ability to cut strings (AKA lines) during stratospheric ballooning missions has a wide variety of uses, including, but not limited to: (a) flight termination (i.e. cutting payloads away from the main balloon), (b) cutting away excess lift balloon(s) to slow ascent rate (and possibly achieve float), (c) cutting away ballast weights to slow descent rate or increase ascent rate, (d) cutting away burst balloon(s) on descent to avoid parachute entanglement, and (e) cutting away payloads which are intended to return to the ground independently for experimental purposes. We report on the development of a “multi-cut” payload box that uses an Arduino microcontroller that can control the cutting of multiple strings in arbitrary order at arbitrary points during a mission, expanding our options for stratospheric ballooning operations. For example, this device may be used during the solar eclipse of August 2017 to drop a timed-series of independently-recovered Geiger counter payloads from a stratospheric balloon stack to characterize changes to the Pfotzer maximum as the Moon’s shadow passes.

The “multi-cut” payload consists of (a) independent nichrome burner modules – currently 4, but this can be changed based on mission requirements, (b) a set of four 1-farad capacitors shared by the burner modules and recharged by a single 9- volt battery, (c) an Arduino Mega microprocessor with SD card logging, (d) sensors (gps and pressure) to allow the microprocessor to execute a mission profile autonomously, and (e) a short-range XBEE radio module through which commands can be sent to the Arduino from the ground via a communications relay payload. Each burner is attached to two pins on the Arduino, one to fire it and one to monitor the status of a pull-pin to give the Arduino feedback about whether or not a burn command has been successfully executed. The lines that run through the payload are 50-lb Dacron fishing line which can be easily cut by red-hot nichrome wire. Items to be released, either balloons to float away upward or weights to fall away downward, are attached to pull-pins so as to pull them out after a successful burn. Burners can be set to fire only if pull-pins are not pulled or forced to fire regardless of pull-pin status. Status updates, logged to an SD card, shown on a bank of LEDs, and sent out via XBEE transmission, include pull-pin status and “attempted to burn” (or not) status.


##Telemetry Operation
**ID Code: MC**
### Command List:
* GO: Initial flight start command/Reset flight clock
* BA: Burn A  
* BB: Burn B
* BC: Burn C
* BD: Burn D
* TG: Transmit GPS
* TP: Transmit pull status
* TT: Transmit flight time
* A1: Turn autonomous controls on
* A0: Turn autonomous controls off
* TEST: Test XBee
* XX: Burn all modules