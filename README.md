# Teensynth Sourcecode
Updated 4/10/2025

## Contents  
- [TODO](#todo)
- [Usage](#usage)
- [Modules](#modules)
- [Error Codes](#error-codes)

## TODO
- [x] Cleanup / optimize menu system into more of a tree system
- [ ] Cleanup / optimize io library for faster access
- [ ] Cleanup / optimize setup functions
- [ ] Refactor DisplayManager to be more templated(?)
- [ ] Integrate Audio components

## IO Validation -- COMPLETE
- [X] Initial power-up from micro-USB
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
This module contains relevant code for organizing the various menu screens that can be shown on the SSD1306 OLED display. The MenusOLED objects are intended to store the specific data and hierarchy of the various menu screens, but these objects do not directly interact with the Adafruit_GFX library or other display code.  
### DisplayManager
This module contains code that directly interacts with the Adafruit_GFX library. It contains functions for drawing the menu bar along the top of the display, showing text lists, showing graphics, and other related functionality.  

### NotesVolts
This module contains definitions for different note frequencies and other functionality helpful in setting and interpretting frequencies.  
## Error Codes
