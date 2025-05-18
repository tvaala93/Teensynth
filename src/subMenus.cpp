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
elapsedMillis potTimer = 0;


int8_t direction = 0;
//uint8_t navigationMode = MODE_DEFAULT;

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
          if(dispMgr.getCurrentMenu()->getActive()){
            dispMgr.getCurrentMenu()->writeSlot(i-1, direction);
            dispMgr.drawIcon(i-1, direction);
            display.display();
            /*
            switch (effector)
            {
            case TYP_OSC:
              updateOSC();
              break;
            case TYP_LFO:
              updateLFO();
              break;
            case TYP_ENV:
              updateENV();
              break;
            case TYP_FLT:
              updateVCF();
              break;
            case TYP_MIX:
              updateMIX();
              break;
            default:
              break;
            }
            */
          }
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
    freq_cut = volt_to_freq(freq_cut);
    ladder1.frequency(freq_cut);

    int resonance = analogRead(RES);  
    ladder1.resonance(1.5 * (float)resonance/1023.0);
    potTimer = 0; // reset the timer
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