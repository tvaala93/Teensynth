# Teensynth Sourcecode
Updated 2/28/2025

## Contents  
- [TODO](#todo)
- [Usage](#usage)
- [Modules](#modules)
- [Error Codes](#error-codes)

## IO Validation TODOs
- [X] Initial power-up from micro-USB
- [X] Full stack sound board test; do I get startup tone on initial power?
- [X] Analog read for volume control
- [X] Analog read for filter control
- [X] OLED display test w/ Adafruit demo
- [X] I2C LED control/ blinkenlights
- [ ] I2C Keyboard input: serial display
- [ ] I2C Keyboard input: OLED display
- [ ] I2C Encoder input: OLED display

## Usage

## Modules
### TeensynthIO  
This module contains relevant pin definitions for the Teensynth along with classes for the following IO devices:
- PCA9555
- Encoder
- TLED  
### MenusOLED  
This module contains relevant code for organizing the various menu screens that can be shown on the SSD1306 OLED display.  

The current ideology is to have a core menu class with subclasses for text-only menu screens and graphic menu screens
### NotesVolts
This module contains definitions for different note frequencies and other functionality helpful in setting and interpretting frequencies
## Error Codes
