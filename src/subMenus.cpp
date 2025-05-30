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

elapsedMillis tick;
elapsedMicros utick;
uint32_t start;
uint32_t finish;

elapsedMicros pca0Timer = 250;
elapsedMicros pca1Timer = 750;
elapsedMicros pca2Timer = 1250;

// Define encoders and their timers
Encoder* encoders[] = {&blkEnc, &bluEnc, &grnEnc, &ylwEnc, &ongEnc};
elapsedMillis encoderTimers[] = {0, 1, 2, 3, 4};
elapsedMillis buttonTimer = 0;
elapsedMillis potTimer = 1;
elapsedMillis keyTimer = 2;
elapsedMillis kbUpdateTimer = 0;

int8_t direction = 0;
//uint8_t navigationMode = MODE_DEFAULT;

// Track keyboard press/release
std::array<int8_t,2> keyVals;

// Every 1500 microseconds, read each PCA
void handlePCAReads(){
  if (pca0Timer >= PCA_INTERVAL_US) {
    pca0.read();
    pca0Timer = 0;     
  }
  if (pca1Timer >= PCA_INTERVAL_US) {
    pca1.read();
    pca1Timer = 0;    
  }
  if (pca2Timer >= PCA_INTERVAL_US) {
    pca2.read();
    pca2Timer = 0;    
  }
  if (kbUpdateTimer >= KB_UPDATE_INTERVAL){
    KB.update(pca0.getStatus(),pca1.getStatus(),pca2.getStatus());
    kbUpdateTimer = 0;
  }
}

// Every 5 milliseconds, update each Encoder
void handleEncoderUpdates(){
  for (size_t i = 0; i < 5; ++i) {
    if (encoderTimers[i] % ENCODER_INTERVAL_MS == 0) {
      encoders[i]->getPosn();
      direction = encoders[i]->getDir();
      if(direction != 0) {
        if(i==0) dispMgr.handleNavigation(direction);
        else{   
          //if(dispMgr.getCurrentMenu()->getActive()){
            dispMgr.getCurrentMenu()->writeSlot(i-1, direction);
            switch (dispMgr.getCurrentMenu()->getMode())
            {
            case MODE_DEFAULT:
              /* code */
              break;
            case MODE_TEXT:
              break;
            case MODE_ICON:
              dispMgr.drawIcon(i-1, direction);
              display.display();
              break;
            case MODE_LINE:
              dispMgr.show(SSD1306_WHITE);
              break;
            case MODE_GRID:
              break;
            default:
              break;
            }                       
          //}
          //else Serial.println("Not active");
        }

      }
      // Reset the timer for this encoder
      encoderTimers[i] = 0;      
    }
  }
}

void handleButtonPresses(){
  if(buttonTimer % BUTTON_INTERVAL_MS == 0){    
    if(blkEnc.getButton()){        
        dispMgr.navigateForward();
        //Serial.println("Forward button pressed");     
    }
    else if(ongEnc.getButton()){
        dispMgr.navigateBackward();
        //Serial.println("Back button pressed");
    }
    buttonTimer = 0; // reset the timer
  }
}

void handleAnalogKnobs(){
  if(potTimer % POTENTIOMETER_INTERVAL_MS == 0){
    // read the vol knob position
    int knob = analogRead(VOL);
    float vol = (float)knob / 1280.0;
    sgtl5000_1.volume(vol);

    // adjust the filter
    float freq_cut = (float)analogRead(FREQ);
    freq_cut = map(freq_cut,0,1023,0,10); // Converting to a "voltage" 10v = C10 note
    freq_cut = voltToFreq(freq_cut);
    ladder1.frequency(freq_cut);

    int resonance = analogRead(RES);  
    ladder1.resonance(1.5 * (float)resonance/1023.0);
    potTimer = 0; // reset the timer
  }
}

void handleKeyboard(){
  if(keyTimer % KEYBOARD_INTERVAL_MS == 0){
    keyVals = KB.sync();
    switch(keyVals[0]){
      case -1:
        break;
      case 25: case 26: case 27: case 28:
        envelopeAmp.noteOn();
        break;
      default:
        waveform0.frequency(voltToFreq(vco0.tune/120.0 + vco0.octv + keyVals[0]/12.0));
        waveform1.frequency(voltToFreq(vco1.tune/120.0 + vco1.octv + keyVals[0]/12.0));
        waveform2.frequency(voltToFreq(vco2.tune/120.0 + vco2.octv + keyVals[0]/12.0));
        waveform3.frequency(voltToFreq(vco3.tune/120.0 + vco3.octv + keyVals[0]/12.0));
        envelopeAmp.noteOn();
        break;
    }
    //if(keyVals[1] >= 0){ // Key 28 was released
    if(KB.lastRelease == KB.lastPress ){
    //if(!KB.anyKeyPressed ){ 
      envelopeAmp.noteOff();
    }
    keyTimer = 0;
  }
}

void setup() {
  Serial.begin(9600);
  //while(!Serial);
  Wire.begin();
  
  ioSetup();
  Serial.println("ioSetup complete!");
  screenSetup();
  Serial.println("screenSetup complete!");
  startupScreen();
  Serial.println("startupScreen complete!");
  setupAudio();
  Serial.println("setupAudio complete!");

  display.clearDisplay();  
  dispMgr.show(SSD1306_WHITE);

  tick = millis();
  utick = micros();
  Serial.println("Setup complete!");
}

void loop(){
  //start = micros(); 
  handlePCAReads();
  handleEncoderUpdates();
  handleButtonPresses();
  handleAnalogKnobs();
  handleKeyboard();
  tick = millis();
  
  /*
  finish = micros();
  if (finish - start > 5000) {
    Serial.print("Loop time: ");
    Serial.print(finish - start);
    Serial.println(" micros");
  }
  */

}