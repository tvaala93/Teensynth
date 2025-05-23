#include <Arduino.h>
//#include <NotesVolts.h>
#include <TeensynthIO.h>
#include <MenusOLED.h>

//#include <Audio.h>
#include <Wire.h>
//#include <SPI.h>
//#include <SD.h>
//#include <SerialFlash.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


// ================================================================================================
// Standard hardware IO. Do not change without good reason
// ================================================================================================
PCA9555 pca0(ADDR_PCA0);
PCA9555 pca1(ADDR_PCA1);
PCA9555 pca2(ADDR_PCA2);

Encoder navEnc(pca0);
Encoder bluEnc(pca0);
Encoder grnEnc(pca1);
Encoder ylwEnc(pca1);
Encoder ongEnc(pca2);

int8_t navDir = 0;
int8_t bluDir = 0;
int8_t grnDir = 0;
int8_t ylwDir = 0;
int8_t ongDir = 0;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// End standard hardware IO -----------------------------------------------------------------------

// ================================================================================================
// Setup Menu screens
// ================================================================================================

String oscOptions[] = {"ZERO","ONE","TWO","THREE","FOUR","FIVE","SIX","SEVEN","","","","","","","",""};
String envOptions[] = {"CONFIG","ADD","DELETE","ROUTE","ANOTHER","","","","","","","","","","",""};

MenusOLED menuHome("HOME", display,homeBMP);
MenusOLED menuKeys("KEYS", display,keysBMP);
MenusOLED menuOSC(MODE_TEXT,"OSCILLATORS", display,oscBMP, oscOptions);
//MenusOLED menuNSH("NOISE, S&H", display,noiseSHBMP);
//MenusOLED menuMIX("MIXER", display,mixBMP);
//MenusOLED menuFLT("FILTER", display,fltBMP);
MenusOLED menuENV(MODE_TEXT,"ENVELOPE", display,envBMP, envOptions);
//MenusOLED menuEFF("EFFECTS", display,keysBMP);
MenusOLED menuArr[] = {
  menuHome,
  menuKeys,
  menuOSC,
  //menuNSH,
  //menuMIX,
  //menuFLT,
  menuENV,
  //menuEFF
};

MenusOLED oscZero("ZERO",display,NULL);
MenusOLED oscOne("ONE",display,NULL);
MenusOLED oscTwo("TWO",display,NULL);
MenusOLED oscThree("THREE",display,NULL);
MenusOLED oscFour("FOUR",display,NULL);
MenusOLED oscArr[] = {
  oscZero,
  oscOne,
  oscTwo,
  oscThree,
  oscFour
};

//MenusOLED* activeMenu = &menuHome;

unsigned char menuIndex = 0;
unsigned char optionIndex = 0;
unsigned char menuLength = 4;

// End setup menu screens -------------------------------------------------------------------------

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
    // Serial.println(dir);
    switch(navMode){
      case NAV_DEFAULT:
        if(menuIndex==0 && dir==-1){menuIndex = menuLength -1;}
        else if(menuIndex==menuLength-1 && dir==1){menuIndex = 0;}
        else{menuIndex+=dir;}
        menuArr[menuIndex].show(SSD1306_WHITE);
        break;
      case NAV_TEXT:
        if(optionIndex==0 && dir==-1){optionIndex = 0;} // Don't go below 0
        else if(optionIndex==NUM_OPTIONS-1 && dir==1){optionIndex = NUM_OPTIONS-1;} // Don't go above NUM_OPTIONS
        else{
          if(menuArr[menuIndex].testText(optionIndex+dir)){
            optionIndex+=dir;
          }          
        }

        menuArr[menuIndex].highlight(optionIndex);
        //Serial.println(optionIndex);
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
          //Serial.println(navEnc.getPosn());          
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
    bool newPress = false;  
    if(navEnc.getButton()){          
      newPress = true;      
      optionIndex = 0;
      navMode++;          
      if(navMode > NAV_GRAPH){navMode = NAV_GRAPH;}      
    }
    else if(ongEnc.getButton()){          
      newPress = true;
      if(navMode <= NAV_DEFAULT){navMode = NAV_DEFAULT;}
      else{navMode--;}                
    }
    if(newPress){
      switch (navMode)
        {
        case NAV_DEFAULT:
          menuArr[menuIndex].deactivate();
          break;
        case NAV_TEXT:
          menuArr[menuIndex].activate();
          menuArr[menuIndex].highlight(optionIndex);
          break;
        case NAV_GRAPH:
          oscArr[optionIndex].deactivate();
        default:
          break;
        }
    }
  }
}

void setup() {
  Serial.begin(9600);
  //while(!Serial);
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
  display.clearDisplay();
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  display.setTextSize(1);  
  display.setTextColor(SSD1306_WHITE,SSD1306_BLACK);
  display.setCursor(40, 20);
  display.println("OSC Menu Test");
  display.setCursor(40, 28);
  display.println("fw v0.0.1");
  display.setCursor(40,36);
  display.println("03.23.25");
  display.drawBitmap(0,18,qrCode,35,35,SSD1306_WHITE);
  display.drawBitmap(0,0,teensynthlogo,128,16,SSD1306_WHITE);
  display.display();
  delay(3000);
  
  display.clearDisplay();
  menuIndex = 0;
  //activeMenu->show(SSD1306_WHITE);
  //activeMenu->highlight(0);
  menuArr[menuIndex].show(SSD1306_WHITE);
  menuArr[menuIndex].highlight(0);
  //menuOSC.show();
  //menuOSC.showText();
  //display.clearDisplay();
  tick = millis();
  utick = micros();
  Serial.println("Setup complete!");
}

void loop(){
    
  //start = micros();
      
  pcaHandler(utick);
  encoderHandler(tick);
  keyButtonHandler(tick);
  
  tick = millis();
  utick = micros();

}