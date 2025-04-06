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

#include <synth_setup.h>

//MenusOLED* currentMenu = &menuHome;
//MenusOLED* lastMenu = &menuHome;

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


int8_t direction = 0;
//uint8_t navigationMode = MODE_DEFAULT;

// Every 1500 microseconds, read each PCA
void handlePCAReads(){
  if (pca0Timer >= PCA_INTERVAL) {
    pca0.read();
    pca0Timer = 0;
  }
  if (pca1Timer >= PCA_INTERVAL) {
    pca1.read();
    pca1Timer = 0;
  }
  if (pca2Timer >= PCA_INTERVAL) {
    pca2.read();
    pca2Timer = 0;
  }
}

void handleForwardButton(){
  dispMgr.navigateForward();
}

void handleBackButton(){
  dispMgr.navigateBackward();
}

void handleBlu(int encDir){
  // Handle blue encoder  
  if(encDir != 0 ){
    // Handle blue encoder in graphic mode{
    Serial.print("Blue Encoder direction: ");
    Serial.println(encDir);

    //dispMgr.drawIcon(currentMenu->getIcon(encDir));    
  }
}
void handleGrn(int encDir){
  // Handle green encoder  
  if(encDir != 0) {
    Serial.print("Green Encoder direction: ");
    Serial.println(encDir);
  }
}
void handleYlw(int encDir){
  // Handle yellow encoder  
  if(encDir != 0) {
    Serial.print("Yellow Encoder direction: ");
    Serial.println(encDir);
  }
}
void handleOng(int encDir){
  // Handle orange encoder  
  if(encDir != 0) {
    Serial.print("Orange Encoder direction: ");
    Serial.println(encDir);
  }
}

// Every 5 milliseconds, update each Encoder
void handleEncoderUpdates(){
  for (size_t i = 0; i < 5; ++i) {
    if (encoderTimers[i] % ENCODER_INTERVAL == 0) {
      encoders[i]->getPosn();
      direction = encoders[i]->getDir();
      if(direction != 0) {
        switch (i) {
            case 0: dispMgr.handleNavigation(direction); break;
            case 1: handleBlu(direction);break;
            case 2: handleGrn(direction);break;
            case 3: handleYlw(direction);break;
            case 4: handleOng(direction);break;
            default: break;
        }
      // Reset the timer for this encoder
      encoderTimers[i] = 0;
      }
    }
  }
}

void handleButtonPresses(elapsedMillis buttonTick){
  if(buttonTick >= ENCODER_DEBOUNCE){    
    if(blkEnc.getButton()){          
        handleForwardButton();   
        //Serial.println("Forward button pressed");     
    }
    else if(ongEnc.getButton()){
        handleBackButton();
        //Serial.println("Back button pressed");
    }
  }
}

void setup() {
  Serial.begin(9600);
  //while(!Serial);
  Wire.begin();
  
  ioSetup();
  screenSetup();
  startupScreen();
  
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
  handleButtonPresses(tick);
  
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