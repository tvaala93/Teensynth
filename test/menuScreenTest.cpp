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

String revision_string = "v0.1\nrev: 3.02.25";


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

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// End standard hardware IO -----------------------------------------------------------------------


// ================================================================================================
// Setup Menu screens
// ================================================================================================

String oscOptions[] = {"CONFIG","ADD","DELETE","ROUTE","ANOTHER","MORE","",""};
String envOptions[] = {"CONFIG","ADD","DELETE","ROUTE","ANOTHER","","",""};

MenusOLED menuHome("HOME", display,homeBMP);
MenusOLED menuKeys("KEYS", display,keysBMP);
MenusOLED menuOSC(MODE_TEXT,"OSCILLATORS", display,oscBMP, oscOptions);
MenusOLED menuNSH("NOISE, S&H", display,noiseSHBMP);
MenusOLED menuMIX("MIXER", display,mixBMP);
MenusOLED menuFLT("FILTER", display,fltBMP);
MenusOLED menuENV(MODE_TEXT,"ENVELOPE", display,envBMP, envOptions);
MenusOLED menuEFF("EFFECTS", display,keysBMP);
MenusOLED menuArr[] = {
  menuHome,
  menuKeys,
  menuOSC,
  menuNSH,
  menuMIX,
  menuFLT,
  menuENV,
  menuEFF
};
//MenusOLED* currMenu;
unsigned char menuIndex = 0;
unsigned char menuLength = 8;

uint8_t navMode = NAV_DEFAULT;

// End setup menu screens -------------------------------------------------------------------------

elapsedMillis tick;
elapsedMicros utick;
uint32_t start;
uint32_t finish;

int8_t dir;

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

void encoderHandler(elapsedMillis eTick){
    // Encoder read hanling
    
    switch (eTick % 5){
        case 0:
            //Serial.println(navEnc.getPosn());
            
            navEnc.getPosn();            
            dir = navEnc.getDir();
            // Serial.println(dir);
            
            if(dir!=0){            
                if(menuIndex==0 && dir==-1){menuIndex = menuLength -1;}
                else if(menuIndex==menuLength-1 && dir==1){menuIndex = 0;}
                else{menuIndex+=dir;}
                menuArr[menuIndex].show(SSD1306_WHITE);                
            }
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
            Serial.println("black button");
            menuArr[menuIndex].activate();
        }
        else if(ongEnc.getButton()){
            menuArr[menuIndex].deactivate();
            Serial.println("orange button");
        }
    }
}


void setup() {
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
    display.clearDisplay();
    display.cp437(true);         // Use full 256 char 'Code Page 437' font
    display.setTextSize(1);
    //display.setTextColor(SSD1306_WHITE); // Draw white text  
    display.setTextColor(SSD1306_WHITE,SSD1306_BLACK);
    display.setCursor(40, 28);
    display.println("fw v0.1");
    display.setCursor(40,36);
    display.println("03.01.25");
    display.drawBitmap(0,18,qrCode,35,35,SSD1306_WHITE);
    display.drawBitmap(0,0,teensynthlogo,128,16,SSD1306_WHITE);
    display.display();
    delay(3000);
    
    display.clearDisplay();
    menuHome.show(SSD1306_WHITE);
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
    //finish = micros();

    //if(finish - start > UPDATE_MICROS){
    //    Serial.print("This is taking too long... ");
    //    Serial.println(finish-start);
        //exit(0);
    //}

  
}