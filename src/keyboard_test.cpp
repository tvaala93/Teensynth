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
MenusOLED menuHome("HOME", display,homeBMP);
MenusOLED menuKeys("KEYS", display,keysBMP);
MenusOLED menuOSC("OSCILLATORS", display,oscBMP);
MenusOLED menuNSH("NOISE, S&H", display,noiseSHBMP);
MenusOLED menuMIX("MIXER", display,mixBMP);
MenusOLED menuFLT("FILTER", display,fltBMP);
MenusOLED menuENV("ENVELOPE", display,envBMP);
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


void press_handler(){
    switch(KB.press_buf[KB.pb_rptr]){
        case -1:
            break;
        case 25: case 26: case 27: case 28:
            String dispval = String(KB.press_buf[KB.pb_rptr]);
            display.setCursor(56,28);
            display.print(dispval);
            display.display();
    }
    KB.pb_rptr++;
}


void release_handler(){
    switch(KB.rel_buf[KB.rb_rptr]){
        case -1:
            break;
        case 25: case 26: case 27: case 28:
            break;
        default:
            //Serial.println("Unhandled");
            break;
    }
    KB.rb_rptr++;
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
    
    display.clearDisplay();
    menuHome.show();
    //display.clearDisplay();
    tick = millis();
    utick = micros();
}

void loop(){
    //start = micros();
    
    pca_handler(utick);
    encoder_handler(tick);
    if(tick % 50 == 0){
        KB.key_handler(pca0.status,pca1.status,pca2.status);
    }

    if(KB.pb_rptr != KB.pb_wptr){press_handler();}
    if(KB.rb_rptr != KB.rb_wptr){release_handler();}

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
