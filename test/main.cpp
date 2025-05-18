#include <Arduino.h>
//#include <NotesVolts.h>
//#include <TeensynthIO.h>
//#include <MenusOLED.h>

//#include <Audio.h>
//#include <Wire.h>
//#include <SPI.h>
//#include <SD.h>
//#include <SerialFlash.h>
//#include <Adafruit_GFX.h>
//#include <Adafruit_SSD1306.h>
#include <synth_setup_mini.h>

String revision_string = "teensynth v0.1\nrev: 2.24.25";

/*
// Standard hardware IO. Do not change without good reason
PCA9555 pca0(ADDR_PCA0);
PCA9555 pca1(ADDR_PCA1);
PCA9555 pca2(ADDR_PCA2);

Encoder navEnc(pca0);
Encoder bluEnc(pca0);
Encoder grnEnc(pca1);
Encoder ylwEnc(pca1);
Encoder ongEnc(pca2);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
// End standard hardware IO
*/

elapsedMillis tick;
elapsedMicros utick;

/*
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
*/


// put function declarations here:
int myFunction(int, int);

void setup() {
  
  Serial.begin(9600);
  Wire.begin();

  ioSetup();
  startupScreen();
  
  /*
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  display.setTextSize(1);
  //display.setTextColor(SSD1306_WHITE); // Draw white text  
  display.setTextColor(SSD1306_BLACK,SSD1306_WHITE);
  display.setCursor(28, 28);
  display.println("teensynth v0.1");
  display.setCursor(28,36);
  display.println("2.24.25");
  display.display();

  pca0.config(PCA0_IO0,PCA0_IO1);
  pca1.config(PCA1_IO0,PCA1_IO1);
  pca2.config(PCA2_IO0,PCA2_IO1);
  */

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
