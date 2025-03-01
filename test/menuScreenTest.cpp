#include <Arduino.h>
#include <NotesVolts.h>
#include <TeensynthIO.h>
#include <MenusOLED.h>

//#include <Audio.h>
#include <Wire.h>
//#include <SPI.h>
//#include <SD.h>
//#include <SerialFlash.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

String revision_string = "teensynth v0.1\nrev: 2.24.25";

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
MenusOLED menuMain("MAIN MENU",display);

elapsedMillis tick;
elapsedMicros utick;

void setup() {
    Serial.begin(9600);
    while(!Serial);
    Wire.begin();
  
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
      Serial.println(F("SSD1306 allocation failed"));
      for(;;); // Don't proceed, loop forever
    }
    display.clearDisplay();
    display.cp437(true);         // Use full 256 char 'Code Page 437' font
    display.setTextSize(1);
    //display.setTextColor(SSD1306_WHITE); // Draw white text  
    display.setTextColor(SSD1306_BLACK,SSD1306_WHITE);
    display.setCursor(28, 28);
    display.println("teensynth v0.1");
    display.setCursor(28,36);
    display.println("2.24.25");
    display.display();
    delay(1000);

    menuMain.show();
}

void loop(){}