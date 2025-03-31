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
MenusOLED* lastMenu = &menuHome;

unsigned char optionIndex = 0;

elapsedMillis tick;
elapsedMicros utick;
uint32_t start;
uint32_t finish;

elapsedMicros pca0Timer = 250;
elapsedMicros pca1Timer = 750;
elapsedMicros pca2Timer = 1250;

elapsedMillis blkEncTimer = 0;
elapsedMillis bluEncTimer = 1;
elapsedMillis grnEncTimer = 2;
elapsedMillis ylwEncTimer = 3;
elapsedMillis ongEncTimer = 4;

int8_t dir;
uint8_t navMode = MODE_DEFAULT;

// Every 1500 microseconds, read each PCA
void pcaHandler(){
  if (pca0Timer >= 1500) {
    pca0.read();
    pca0Timer = 0;
  }
  if (pca1Timer >= 1500) {
    pca1.read();
    pca1Timer = 0;
  }
  if (pca2Timer >= 1500) {
    pca2.read();
    pca2Timer = 0;
  }
}

// Need to tell navEnc what to do based on current navMode
void navHandler(){
  dir = blkEnc.getDir();
  if(dir != 0){    
    Serial.print("dir: ");
    Serial.println(dir);
    Serial.print("navMode: ");
    Serial.println(navMode);
    Serial.print("currentMenu: ");
    Serial.println(currentMenu->getName());
    Serial.println();
    switch(navMode){
      case MODE_DEFAULT:        
        if(dir==1 && currentMenu->getRight() != NULL){
          currentMenu = currentMenu->getRight();
          lastMenu = currentMenu;
        }
        if(dir==-1 && currentMenu->getLeft() != NULL){
          currentMenu = currentMenu->getLeft();
          lastMenu = currentMenu;
        } 
        currentMenu->deactivate();        
        break;
      case MODE_TEXT:
        if(optionIndex==0 && dir==-1){optionIndex = 0;} // Don't go below 0
        else if(optionIndex==NUM_OPTIONS-1 && dir==1){optionIndex = NUM_OPTIONS-1;} // Don't go above NUM_OPTIONS
        else{
          if(currentMenu->testText(optionIndex+dir)){
            optionIndex+=dir;
          }          
        }
        currentMenu->highlight(optionIndex);        
        break;
      default:
        break;
    }
  }
}

void forwardButton(){
  if(!currentMenu->getActive()){
    currentMenu->activate();
    navMode = currentMenu->getMode(); // If a text menu, navigate in text mode; same for graphic            
    optionIndex = 0;
    currentMenu->highlight(optionIndex);
  }
  // If in text mode, switch to the highlighted menu
  else if(currentMenu->getMode()==MODE_TEXT){
    if(currentMenu->getChild(optionIndex) != NULL){
      lastMenu = currentMenu;            
      currentMenu = currentMenu->getChild(optionIndex);            
      currentMenu->deactivate();            
      navMode = currentMenu->getMode();
      //optionIndex = 0;
      //currentMenu->highlight(optionIndex);
    }
  }
  else{
      Serial.println("UNHANDLED LOGIC: navButton press");
  }
}

void backButton(){
  // If current menu is not active, 
  if(!currentMenu->getActive()){          
    if(currentMenu->getParent() != NULL){            
      currentMenu = currentMenu->getParent();
    }
    else{
      currentMenu = lastMenu;
    }
    currentMenu->activate();
    navMode = currentMenu->getMode(); // update navMode
    if(navMode == MODE_TEXT){
      currentMenu->highlight(optionIndex);
    }          
  }        
  else{
      currentMenu->deactivate();
      navMode = MODE_DEFAULT;            
  }
}

// Every 5 milliseconds, update each Encoder
void encoderHandler(){
  if(blkEncTimer % 5 == 0){
    blkEnc.getPosn();
    navHandler();
    blkEncTimer = 0;
  }
  if(bluEncTimer % 5 == 0){
    bluEnc.getPosn();
    bluEncTimer = 0;
  }
  if(grnEncTimer % 5 == 0){
    grnEnc.getPosn();
    grnEncTimer = 0;
  }
  if(ylwEncTimer % 5 == 0){
    ylwEnc.getPosn();
    ylwEncTimer = 0;
  }
  if(ongEncTimer % 5 == 0){
    ongEnc.getPosn();
    ongEncTimer = 0;
  }
}

void keyButtonHandler(elapsedMillis kTick){
  if(kTick % 50 == 0){    
    if(blkEnc.getButton()){          
        forwardButton();   
        Serial.println("blkEnc button pressed");     
    }
    else if(ongEnc.getButton()){
        backButton();
        Serial.println("ongEnc button pressed");
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
  int16_t x1, y1;
  uint16_t w, h;
  display.clearDisplay();
  display.drawBitmap(4,32,sawtoothBMP,24,16,SSD1306_WHITE);
  display.drawCircle(16, 40, 12, SSD1306_WHITE);  
  display.getTextBounds("WAVE", 128, 0, &x1, &y1, &w, &h); //calc width of new string    
  display.setCursor((16 - w/2), 56);  
  display.print("WAVE");
  display.drawBitmap(36,32,squareBMP,24,16,SSD1306_WHITE);
  display.drawCircle(48, 40, 12, SSD1306_WHITE);
  display.getTextBounds("OCTV", 128, 0, &x1, &y1, &w, &h); //calc width of new string    
  display.setCursor((48 - w/2), 56);  
  display.print("OCTV");
  display.drawBitmap(68,32,triangleBMP,24,16,SSD1306_WHITE);
  display.drawCircle(80, 40, 12, SSD1306_WHITE);
  display.getTextBounds("TUNE", 128, 0, &x1, &y1, &w, &h); //calc width of new string    
  display.setCursor((80 - w/2), 56);  
  display.print("TUNE");
  display.drawBitmap(100,32,sineBMP,24,16,SSD1306_WHITE);
  display.drawCircle(112, 40, 12, SSD1306_WHITE);
  display.getTextBounds("GAIN", 128, 0, &x1, &y1, &w, &h); //calc width of new string    
  display.setCursor((112 - w/2), 56);  
  display.print("GAIN");
  display.display();
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
  pcaHandler();
  encoderHandler();
  keyButtonHandler(tick);
  
  tick = millis();  
}