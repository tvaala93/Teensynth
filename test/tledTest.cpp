#include <TeensynthIO.h>
#include <Arduino.h>

PCA9555 pca0(ADDR_PCA0);
PCA9555 pca1(ADDR_PCA1);
PCA9555 pca2(ADDR_PCA2);

Encoder navEnc(pca0);
Encoder bluEnc(pca0);
Encoder grnEnc(pca1);
Encoder ylwEnc(pca1);
Encoder ongEnc(pca2);

TLED bluLED(pca2,1,4);
TLED grnLED(pca2,1,3);
TLED ylwLED(pca1,0,7);
TLED redLED(pca1,0,6);

void setup(){
    Serial.begin(9600);    
    Wire.begin();    
    Wire.setClock(400000);
    
    //setup PCAs    
    if(pca0.config(PCA0_IO0,PCA0_IO1)){
        Serial.println("PCA0 Setup Success!");        
    }
    if(pca1.config(PCA1_IO0,PCA1_IO1)){
        Serial.println("PCA1 Setup Success!");        
    }
    if(pca2.config(PCA2_IO0,PCA2_IO1)){
        Serial.println("PCA2 Setup Success!");        
    }
    
    //setup Encoders
    navEnc.config(6,5,4,1);
    bluEnc.config(1,2,3,1);
    grnEnc.config(6,5,4,1);
    ylwEnc.config(1,2,3,1);
    ongEnc.config(7,6,5,1);
}

void loop(){
    bluLED.write(navEnc.getButton());
    grnLED.write(bluEnc.getButton());
    ylwLED.write(ylwEnc.getButton());
    redLED.write(ongEnc.getButton());
    delay(200);
}