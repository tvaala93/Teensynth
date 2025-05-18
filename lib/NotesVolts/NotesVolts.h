#include <Arduino.h>
#include <cmath>
#include <Audio.h>

using namespace std;

// Frequencies
#define C2 65.41
#define D2 73.42
#define E2 82.41
#define F2 87.31
#define G2 98.00
#define AA2 110.0
#define B2 123.5
#define C3 130.8
#define D3 146.8
#define E3 164.8
#define F3 174.6
#define G3 196.0
#define AA3 220.0
#define B3 246.9
#define C4 261.6

// Working with an input of 0 tunes to a C0 (16.35Hz) 
#define ROOT_FREQ 16.352

float volt_to_freq(float volt);

struct Oscillator {
    //float freq;
    AudioSynthWaveform *audioObj = nullptr;
    int8_t wave =0;
    int8_t octv = 3;
    int8_t tune = 0; // V/octv, NOT frequency
    int8_t pulseWidth = 50;
    int8_t gain = 100;
    int8_t lastWave = 0;    
};
struct LFO {
    //float freq;
    AudioSynthWaveform *audioObj = nullptr;
    int8_t wave =0;
    int8_t tune; // V/octv, NOT frequency
    int8_t pulseWidth;
    int8_t gain;
    int8_t lastWave = 0;    
};
struct Filter {
    AudioFilterLadder *audioObj = nullptr;
    float cutoffFreq;
    float res;    
};
struct Envelope {
    AudioEffectEnvelope *audioObj = nullptr;
    int8_t attack;
    int8_t decay;
    int8_t sustain;
    int8_t release;    
};
struct Mixer {
    AudioMixer4 *audioObj = nullptr;
    int8_t chan0 = 50;
    int8_t chan1 = 50;
    int8_t chan2 = 50;
    int8_t chan3 = 50;
};

/*
int updateOSC(Oscillator *oscRef, AudioSynthWaveform *waveObj);
int updateLFO(LFO *lfoRef, AudioSynthWaveform *waveObj);
int updateVCF(Filter *vcfRef, AudioFilterLadder *filtObj);
int updateENV(Envelope *envRef, AudioEffectEnvelope *envObj);
int updateMIX(Mixer *mixRef, AudioMixer4 *mixObj);
*/
