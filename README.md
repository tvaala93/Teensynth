# Teensynth Sourcecode
Updated 5/18/2025 - Checking off TODOs from **Make Some Noise**

## Contents  
- [TODO](#todo)
- [Usage](#usage)
- [Modules](#modules)
- [Error Codes](#error-codes)

## TODO - Cleanup and Optimize
- [ ] Cleanup / optimize io library for faster access
- [ ] Cleanup / optimize setup functions
- [ ] Make UI consistent (Menu Activation, knob control, etc.)
- [ ] Add useful comments to code

## TODO - Make Some Noise
- [X] Get Oscillator menus connected to sound objects
- [X] Get Mixer menus connected to sound object
- [ ] Get Envelopes connected to sound objects
- [ ] Get Effects menus connected to sounds objects

## TODO - Menu Layouts
- [X] Organize menu system into a tree structure
- [X] Create slot system for per-menu encoder control
- [X] Create text menu system
- [X] Create round icon menu system
- [ ] Create ADSR menu system for Envolopes
- [ ] Create grid menu system for Mod Matrix

## IO Validation -- COMPLETE  
- [X] Initial power-up from micro-USBv
- [X] Full stack sound board test; do I get startup tone on initial power?
- [X] Analog read for volume control
- [X] Analog read for filter control
- [X] OLED display test w/ Adafruit demo
- [X] I2C LED control/ blinkenlights
- [x] I2C Keyboard input: serial display
- [x] I2C Keyboard input: OLED display
- [x] I2C Encoder input: OLED display

## Usage

## Modules
### TeensynthIO  
This module contains relevant pin definitions for the Teensynth along with classes for the following IO devices:
- PCA9555
- Encoder
- TLED  
### MenusOLED  
This module contains relevant code for organizing the various menu screens that can be shown on the SSD1306 OLED display.  
### DisplayManager
This module connects relevant data stored in MenusOLED objects to the necessary commands to the Adafruit_GFX library that results in updates to the SSD1306 display
### NotesVolts
This module contains definitions for different note frequencies and other functionality helpful in setting and interpretting frequencies  

The module also contains structs for organizing Audio objects
## Error Codes
