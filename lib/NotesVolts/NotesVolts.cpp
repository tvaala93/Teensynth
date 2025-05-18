#include <NotesVolts.h>
using namespace std;


/**
 * @brief Function to convert a voltage to a frequency based on 1V/Oct scaling
 * based off the set ROOT_FREQ value
 * 
 * @param volt the value representing the effective voltage of a signal
 * @return the corresponding frequency
 */
float volt_to_freq(float volt){       
    return ROOT_FREQ * pow(2,volt);
}

/*
int updateOSC(Oscillator *oscRef, AudioSynthWaveform *waveObj){
    float tune = float(oscRef->tune / 100.0);
    float gain = float(oscRef->gain / 100.0);
    float pwm = float(oscRef->pulseWidth / 100.0);
    float newFreq = volt_to_freq(oscRef->tune + tune);
    
    if(oscRef->wave != oscRef->lastWave){
        if(oscRef->wave == WAVEFORM_SQUARE) oscRef->wave = WAVEFORM_PULSE;
        if(oscRef->wave == WAVEFORM_TRIANGLE) oscRef->wave = WAVEFORM_TRIANGLE_VARIABLE;
        oscRef->lastWave = oscRef->wave;
        waveObj->begin(oscRef->wave);
    }    
    
    waveObj->amplitude(gain);
    waveObj->frequency(newFreq);
    waveObj->pulseWidth(pwm);

    return 0;
}

int updateLFO(LFO *lfoRef, AudioSynthWaveform *waveObj){
    return 0;
}

int updateVCF(Filter *vcfRef, AudioFilterLadder *filtObj){
    return 0;
}

int updateENV(Envelope *envRef, AudioEffectEnvelope *envObj){
    return 0;
}

int updateMIX(Mixer *mixRef, AudioMixer4 *mixObj){
    float gain0 = float(mixRef->chan0 / 100.0);
    float gain1 = float(mixRef->chan1 / 100.0);
    float gain2 = float(mixRef->chan2 / 100.0);
    float gain3 = float(mixRef->chan3 / 100.0);
    mixObj->gain(0,gain0);
    mixObj->gain(1,gain1);
    mixObj->gain(2,gain2);
    mixObj->gain(3,gain3);    
    return 0;
}
*/

float noScale(float val){
    return val;
}