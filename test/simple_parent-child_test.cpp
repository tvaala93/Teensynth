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

#include <setup.h>

MenusOLED grandchild1("GRANDCHILD1",display,NULL);
MenusOLED grandchild2("GRANDCHILD2",display,NULL);
MenusOLED grandchildArr[] = {grandchild1, grandchild2};
String grandchildOptions[] = {"GRANDCHILD1","GRANDCHILD2","","","","","","","","","","","","","",""};
MenusOLED child1(MODE_TEXT,"CHILD1",display,NULL,grandchildOptions,grandchildArr);
MenusOLED child2(MODE_TEXT,"CHILD2",display,NULL,grandchildOptions,grandchildArr);
MenusOLED childArr[] = {child1, child2};
String childOptions[] = {"CHILD1","CHILD2","","","","","","","","","","","","","",""};
MenusOLED parent1(MODE_TEXT,"PARENT1",display,NULL,childOptions,childArr);
MenusOLED parent2(MODE_TEXT,"PARENT2",display,NULL,childOptions,childArr);

MenusOLED* currentMenu = &parent1;

unsigned char optionIndex = 0;

elapsedMillis tick;
elapsedMicros utick;
uint32_t start;
uint32_t finish;

int8_t dir;
uint8_t navMode = NAV_DEFAULT;

// Every 1500 microseconds, read each PCA
void pcaHandler(elapsedMicros uptick){
    switch(utick % 1500){
        case 0:
            pca0.read();
            break;
        case 500:
            pca1.read();
            break;
        case 1000:
            pca2.read();
            break;
        default:
            break;
    }
  }

// Need to tell navEnc what to do based on current navMode
void navHandler(){
    dir = navEnc.getDir();
    if(dir != 0){    
      switch(navMode){
        case NAV_DEFAULT:
          if(dir==1 && currentMenu->getRight() != NULL) currentMenu = currentMenu->getRight();          
          if(dir==-1 && currentMenu->getLeft() != NULL) currentMenu = currentMenu->getLeft();        
          currentMenu->deactivate();
          Serial.println(currentMenu->getName());
          break;
        case NAV_TEXT:
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
  
// Every 5 milliseconds, update each Encoder
void encoderHandler(elapsedMillis eTick){
    switch (eTick % 5){
        case 0:                  
            navEnc.getPosn();
            navHandler();
            break;      
        case 1:            
            bluEnc.getPosn();
            break;
        case 2:            
            grnEnc.getPosn();
            
            break;
        case 3:            
            ylwEnc.getPosn();
            break;
        case 4:            
            ongEnc.getPosn();            
            break;    
        default:
            break;
    }
  }

void keyButtonHandler(elapsedMillis kTick){
    if(kTick % 50 == 0){    
        if(navEnc.getButton()){
            if(!currentMenu->getActive()){                
                currentMenu->activate();
                Serial.println("keyButton nav activate");
            }
        }
        if(ongEnc.getButton()){
            if(currentMenu->getActive()){
                currentMenu->deactivate();
            }
            else{
                MenusOLED* lastMenu = currentMenu->getParent();
                Serial.println(lastMenu->getName());
                lastMenu->activate();
                currentMenu = lastMenu;
                Serial.println("keyButton check");
            }
        }
    }
}

void setup(){    

    Serial.begin(9600);
    while(!Serial);
    Wire.begin();

    //setup PCAs    
    pca0.config(PCA0_IO0,PCA0_IO1);
    pca1.config(PCA1_IO0,PCA1_IO1);
    pca2.config(PCA2_IO0,PCA2_IO1);

    //setup Encoders
    navEnc.config(6,5,4,1);
    bluEnc.config(1,2,3,1);
    grnEnc.config(6,5,4,1);
    ylwEnc.config(1,2,3,1);
    ongEnc.config(7,6,5,1);

    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;); // Don't proceed, loop forever
    }

    parent1.setSiblings(NULL,&parent2);
    parent2.setSiblings(&parent1,NULL);
    grandchild1.setSiblings(NULL,&grandchild2);
    grandchild2.setSiblings(&grandchild1,NULL);

    //display.clearDisplay();
    display.cp437(true);         // Use full 256 char 'Code Page 437' font
    display.setTextSize(1);  
    display.setTextColor(SSD1306_WHITE,SSD1306_BLACK);
    currentMenu = &parent1;
    Serial.println("CHECK1");

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
  
  pcaHandler(utick);
  encoderHandler(tick);
  keyButtonHandler(tick);
  
  tick = millis();
  utick = micros();
}
