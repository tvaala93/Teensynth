#include <Arduino.h>
//#include <NotesVolts.h>
//#include <TeensynthIO.h>
//#include <MenusOLED.h>

//#include <Audio.h>
#include <Wire.h>
//#include <SPI.h>
//#include <SD.h>
//#include <SerialFlash.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <synth_setup.h>

MenusOLED* currentMenu = &menuHome;
//MenusOLED* lastMenu = &menuHome;

unsigned char optionIndex = 0;

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


int8_t encoderDirection;
uint8_t navigationMode = MODE_DEFAULT;

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

void handleTextNavigation(){
  if(optionIndex==0 && encoderDirection==-1){return;} // Don't go below 0
  else if(optionIndex==currentMenu->getChildCount()-1 && encoderDirection==1){return;} // Don't go above NUM_OPTIONS
  optionIndex+=encoderDirection;
  currentMenu->highlight(optionIndex);  
}

// Need to tell navEnc what to do based on current navigationMode
void handleNavigation(){
  encoderDirection= blkEnc.getDir();
  if(encoderDirection!= 0){    
    Serial.print("dir: ");
    Serial.println(encoderDirection);
    Serial.print("navigationMode: ");
    Serial.println(navigationMode);
    Serial.print("currentMenu: ");
    Serial.println(currentMenu->getName());
    Serial.println();

    // If menu is not active, navigate left/right
    if(!currentMenu->getActive() || navigationMode == MODE_DEFAULT){
      // If menu is not active, navigate left/right
      if(encoderDirection==1 && currentMenu->getRight() != nullptr){
        currentMenu = currentMenu->getRight();
        //lastMenu = currentMenu;
      }
      else if(encoderDirection==-1 && currentMenu->getLeft() != nullptr){
        currentMenu = currentMenu->getLeft();
        //lastMenu = currentMenu;
      } 
      currentMenu->show(SSD1306_WHITE);
    }
    // If menu is active, navigate based on screen type
    else{
      // If menu is active, navigate based on screen type
      switch(navigationMode){      
        case MODE_TEXT:
          handleTextNavigation();
          break;
        case MODE_GRAPHIC:
          //handleGraphicNavigation();
          break;
        default:
          break;
      }
    }
  }
}

void handleForwardButton(){
  if(!currentMenu->getActive()){
    currentMenu->activate();
    navigationMode = currentMenu->getMode(); // If a text menu, navigate in text mode; same for graphic            
    optionIndex = 0;
    currentMenu->highlight(optionIndex);
  }
  // If in text mode, switch to the highlighted menu
  else if(currentMenu->getMode()==MODE_TEXT){
    if(currentMenu->getChild(optionIndex) != nullptr){
      //lastMenu = currentMenu;            
      currentMenu = currentMenu->getChild(optionIndex);            
      currentMenu->deactivate();            
      navigationMode = currentMenu->getMode();      
    }
  }
  else{
      Serial.println("UNHANDLED LOGIC: navButton press");
  }
}

void handleBackButton(){
  // If current menu is not active, 
  if(!currentMenu->getActive()){          
    //if(currentMenu->getParent() != nullptr){            
      currentMenu = currentMenu->getParent();
      if (currentMenu->getName() == "ROOT"){
        currentMenu = &menuHome;
      }
      else{
        currentMenu->show(SSD1306_WHITE);
      }
    //}
    /*
    else{
      currentMenu = lastMenu;
    }
    */
    currentMenu->activate();
    navigationMode = currentMenu->getMode(); // update navigationMode
    if(navigationMode == MODE_TEXT){
      currentMenu->highlight(optionIndex);
    }          
  }        
  else{
      currentMenu->deactivate();
      navigationMode = MODE_DEFAULT;            
  }
}

// Every 5 milliseconds, update each Encoder
void handleEncoderUpdates(){
  for (size_t i = 0; i < sizeof(encoders) / sizeof(encoders[0]); ++i) {
    if (encoderTimers[i] % ENCODER_INTERVAL == 0) {
        encoders[i]->getPosn();
        if (i == 0) { // blkEnc is the navigation encoder
            handleNavigation();
        }
        encoderTimers[i] = 0;
    }
}
}

void handleButtonPresses(elapsedMillis buttonTick){
  if(buttonTick >= ENCODER_DEBOUNCE){    
    if(blkEnc.getButton()){          
        handleForwardButton();   
        Serial.println("Forward button pressed");     
    }
    else if(ongEnc.getButton()){
        handleBackButton();
        Serial.println("Back button pressed");
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
  for (size_t i = 0; i < waveIcons.size(); ++i) {
    drawIcon(waveIcons[i], display);
  }
  delay(2000);

  display.clearDisplay();  
  currentMenu->show(SSD1306_WHITE);
  currentMenu->highlight(0);
  //menuOSC.show();
  //menuOSC.showText();
  //display.clearDisplay();
  tick = millis();
  utick = micros();
  Serial.println("Setup complete!");
}

void loop(){ 
  handlePCAReads();
  handleEncoderUpdates();
  handleButtonPresses(tick);
  
  tick = millis();  
}