//#include <Arduino.h>
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

//#include <string>
//#include <iostream>

String revision_string = "v0.1\nrev: 3.14.25";

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
MenusOLED menuRoot(MODE_DEFAULT,"ROOT",NULL);
MenusOLED menuHome(MODE_TEXT,"Home", homeBMP);
MenusOLED menuKeys(MODE_DEFAULT,"Keys", keysBMP);
MenusOLED menuOSC(MODE_TEXT,"Oscillators",oscBMP);
MenusOLED menuNSH(MODE_DEFAULT,"Noise, S&H", noiseSHBMP);
MenusOLED menuMIX(MODE_ICON,"Mixer", mixBMP);
MenusOLED menuFLT(MODE_DEFAULT,"Filter", fltBMP);
MenusOLED menuENV(MODE_TEXT,"Envelopes", envBMP);
MenusOLED menuEFF(MODE_TEXT,"Effects", keysBMP);
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
unsigned char menuIndex = 0;
unsigned char menuLength = 8;

// End setup menu screens -------------------------------------------------------------------------

// ================================================================================================
// Setup Keyboard
// ================================================================================================
Keyboard KB;
// End keyboard setup -----------------------------------------------------------------------------

elapsedMillis tick;
elapsedMicros utick;

uint32_t start;
uint32_t finish;

//float keyArr[10];

void pca_handler(elapsedMicros uptick){
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

void encoder_handler(elapsedMillis eTick){
    // Encoder read hanling
    switch (eTick % 5){
        case 0:
            {
            //Serial.println(navEnc.getPosn());
            navEnc.getPosn();
            int8_t dir = navEnc.getDir();
            // Serial.println(dir);
            if(dir!=0){            
                if(menuIndex==0 && dir==-1){menuIndex = menuLength -1;}
                else if(menuIndex==menuLength-1 && dir==1){menuIndex = 0;}
                else{menuIndex+=dir;}            
                menuArr[menuIndex].show();          
            }
            break;
        }
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

void key_handler(elapsedMillis kTick){
    if(kTick % 50 == 0){
        KB.update(pca0.getStatus(),pca1.getStatus(),pca2.getStatus());
    }
}

void setup(){
    Serial.begin(9600);
    //while(!Serial);    
    Wire.begin();    
    Wire.setClock(400000);

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
    display.println("03.14.25");
    display.drawBitmap(0,18,qrCode,35,35,SSD1306_WHITE);
    display.drawBitmap(0,0,teensynthlogo,128,16,SSD1306_WHITE);
    display.display();
    delay(3000);
    
    //display.clearDisplay();
    //menuHome.show();
    //display.clearDisplay();
    tick = millis();
    utick = micros();
}

void loop(){
    //start = micros();
    
    pca_handler(utick);
    encoder_handler(tick);
    key_handler(tick);
    
    KB.sync();

    // Timing -------------------------------------------------------------------------------------
    
    /*
    finish = micros();

    switch(((finish-start)% 5000)/20){
        case 0:
            Serial.println("S-tier");
            break;            
        case 1:
            Serial.println("A-tier");
            break;
        case 2:
            Serial.println("B-tier");
            break;
        case 3:
            Serial.println("C-tier");
            break;
        default:
            //Serial.println("Over 800uS");
            Serial.println(finish-start);
            //exit(0);
            break;
    }
    */

    tick = millis();
    utick = micros();
}
