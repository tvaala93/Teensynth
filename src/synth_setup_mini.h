//#include <Arduino.h>
#include <NotesVolts.h>
#include <TeensynthIO.h>
//#include <MenusOLED.h>
#include <DisplayManager.h>

#include <Audio.h>
#include <Wire.h>
//#include <SPI.h>
//#include <SD.h>
#include <SerialFlash.h>
//#include <Adafruit_GFX.h>
//#include <Adafruit_SSD1306.h>

#define APP_NAME "Teensynth - Mini"
#define APP_VERSION "0.0.1"
#define APP_DATE "2025-04-12"
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

extern DisplayManager dispMgr;

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
extern MenusOLED menuRoot;

// End setup menu screens -------------------------------------------------------------------------


// ================================================================================================
// Setup Audio
// ================================================================================================

extern AudioOutputI2S           i2s1;
extern AudioSynthWaveform       waveform0;
extern AudioSynthWaveform       waveform1;
extern AudioSynthWaveform       waveform2;
extern AudioSynthWaveform       waveform3;
extern AudioSynthWaveform       lowFreqOsc0;
extern AudioSynthWaveform       lowFreqOsc1;
extern AudioEffectEnvelope      envelopeAmp;
extern AudioEffectEnvelope      envelopeFilter;
extern AudioMixer4              mixer1;
extern AudioFilterLadder        ladder1;
extern AudioConnection          patchCord1;
extern AudioConnection          patchCord2;
extern AudioConnection          patchCord3;
extern AudioConnection          patchCord4;
extern AudioConnection          patchCord5;
extern AudioConnection          patchCord6;
extern AudioControlSGTL5000     sgtl5000_1;     //xy=959,505

/*
int asdf = WAVEFORM_SINE;
asdf = WAVEFORM_SAWTOOTH;
asdf = WAVEFORM_SQUARE;
asdf = WAVEFORM_TRIANGLE;
asdf = WAVEFORM_ARBITRARY;
asdf = WAVEFORM_PULSE;
asdf = WAVEFORM_SAWTOOTH_REVERSE;
asdf = WAVEFORM_SAMPLE_HOLD;
asdf = WAVEFORM_TRIANGLE_VARIABLE;
*/

extern Oscillator osc0Params;
extern Oscillator osc1Params;
extern Oscillator osc2Params;
extern Oscillator osc3Params;
extern LFO lfo0Params;
extern LFO lfo1Params;
extern Filter filterParams;
extern Envelope env0Params;
extern Envelope env1Params;

// End setup audio --------------------------------------------------------------------------------


// ================================================================================================
// Icons
// ================================================================================================

extern std::vector<Icon> waveIcons;

// End icons -----------------------------------------------------------------------------------

// ================================================================================================
// Helper Functions
// ================================================================================================
void ioSetup();
void screenSetup();
void startupScreen();