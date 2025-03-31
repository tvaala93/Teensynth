//#include <Arduino.h>
#include <NotesVolts.h>
#include <TeensynthIO.h>
#include <MenusOLED.h>

//#include <Audio.h>
//#include <Wire.h>
//#include <SPI.h>
//#include <SD.h>
//#include <SerialFlash.h>
//#include <Adafruit_GFX.h>
//#include <Adafruit_SSD1306.h>

#define APP_NAME "Teensynth"
#define APP_VERSION "0.0.2"
#define APP_DATE "2025-03-30"
#define APP_AUTHOR "Ted Vaala"

// ================================================================================================
// Standard hardware IO. Do not change without good reason
// ================================================================================================
extern PCA9555 pca0;
extern PCA9555 pca1;
extern PCA9555 pca2;

extern Encoder blkEnc;
extern Encoder bluEnc;
extern Encoder grnEnc;
extern Encoder ylwEnc;
extern Encoder ongEnc;

extern int8_t blkDir;
extern int8_t bluDir;
extern int8_t grnDir;
extern int8_t ylwDir;
extern int8_t ongDir;

extern Adafruit_SSD1306 display;

// End standard hardware IO -----------------------------------------------------------------------

// ================================================================================================
// Setup Menu screens
// ================================================================================================

extern String oscOptions[];
extern String envOptions[];

extern MenusOLED oscConfig;
extern MenusOLED oscAdd;
extern MenusOLED oscDelete;
extern MenusOLED oscRoute;
//extern MenusOLED oscFour;
//extern MenusOLED oscFive;
//extern MenusOLED oscSix;
//extern MenusOLED oscSeven;
extern MenusOLED oscArr[];

extern MenusOLED envConfig;
extern MenusOLED envAdd;
extern MenusOLED envDelete;
extern MenusOLED envRoute;
//extern MenusOLED envFour;
extern MenusOLED envArr[];

extern MenusOLED menuHome;
extern MenusOLED menuKeys;
extern MenusOLED menuOSC;
extern MenusOLED menuNSH;
extern MenusOLED menuMIX;
extern MenusOLED menuFLT;
extern MenusOLED menuENV;
extern MenusOLED menuEFF;
//MenusOLED modMatrix("MOD MATRIX",display,gridBMP);

// End setup menu screens -------------------------------------------------------------------------

void ioSetup();
void screenSetup();
void startupScreen();