#include <Arduino.h>
#include <NotesVolts.h>
#include <TeensynthIO.h>

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthWaveform       waveform2;      //xy=443,424
AudioSynthWaveform       waveform3;      //xy=444,463
AudioSynthWaveform       waveform1;      //xy=445,385
AudioMixer4              mixer1;         //xy=620,452
AudioFilterLadder        ladder1;        //xy=800,464
AudioOutputI2S           i2s1;           //xy=961,464
AudioConnection          patchCord1(waveform2, 0, mixer1, 1);
AudioConnection          patchCord2(waveform3, 0, mixer1, 2);
AudioConnection          patchCord3(waveform1, 0, mixer1, 0);
AudioConnection          patchCord4(mixer1, 0, ladder1, 0);
AudioConnection          patchCord5(ladder1, 0, i2s1, 0);
AudioConnection          patchCord6(ladder1, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=959,505
// GUItool: end automatically generated code


// put function declarations here:
int myFunction(int, int);

void setup() {
  Serial.begin(9600);
  AudioMemory(20);
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.32);
  waveform1.begin(WAVEFORM_SQUARE);
  waveform2.begin(WAVEFORM_SAWTOOTH);
  waveform3.begin(WAVEFORM_TRIANGLE);
  waveform1.amplitude(0.3);
  waveform2.amplitude(0.3);
  waveform3.amplitude(0.3);
  waveform1.frequency(110);
  waveform2.frequency(110*1.5);
  waveform3.frequency(220);

  mixer1.gain(0,1);
  mixer1.gain(1,1);
  mixer1.gain(2,1);  
}

void loop() {
  // put your main code here, to run repeatedly:

  // read the vol knob position
  int knob = analogRead(VOL);
  float vol = (float)knob / 1280.0;
  sgtl5000_1.volume(vol);

  // adjust the filter
  float freq_cut = (float)analogRead(FREQ);
  freq_cut = map(freq_cut,0,1023,0,10); // Converting to a "voltage" 10v = C10 note
  freq_cut = volt_to_freq(freq_cut);
  ladder1.frequency(freq_cut);

  int resonance = analogRead(RES);  
  ladder1.resonance(1.5 * (float)resonance/1023.0);

  // set osc frequency???
  float new_freq = (float)analogRead(CON);
  new_freq = map(new_freq,0,1023,1,8); // Converting to a "voltage" 10v = C10 note
  new_freq = volt_to_freq(new_freq);
  waveform1.frequency(new_freq);
  waveform2.frequency(new_freq*1.5);
  waveform3.frequency(new_freq*2.0);
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}