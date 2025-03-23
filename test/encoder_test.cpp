#include <Arduino.h>
#include <TeensynthIO.h>

PCA9555 pca0(ADDR_PCA0);
PCA9555 pca1(ADDR_PCA1);
PCA9555 pca2(ADDR_PCA2);

Encoder navEnc(pca0);
Encoder bluEnc(pca0);
Encoder grnEnc(pca1);
Encoder ylwEnc(pca1);
Encoder ongEnc(pca2);

elapsedMillis tick;
elapsedMicros utick;

uint32_t start;
uint32_t finish;

void setup(){
    Serial.begin(9600);
    while(!Serial);    
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


    tick = millis();
    utick = micros();
}

void loop(){

    start = micros();    
    
    if(tick%1 == 0){
        pca0.read();
        pca1.read();
        pca2.read();
    }    
    

    /*
    if(tick % 5 == 0){
        //Serial.print("Knob Posn:");
        Serial.print(navEnc.getPosn());
        Serial.print(", ");
        Serial.print(bluEnc.getPosn());
        Serial.print(", ");
        Serial.print(grnEnc.getPosn());
        Serial.print(", ");
        Serial.print(ylwEnc.getPosn());
        Serial.print(", ");
        Serial.println(ongEnc.getPosn());
        
    }
    */

    switch (tick % 5)
    {
    case 0:
        Serial.println(navEnc.getPosn());
        break;
    case 1:
        Serial.println(bluEnc.getPosn());
        break;
    case 2:
        Serial.println(grnEnc.getPosn());
        break;
    case 3:
        Serial.println(ylwEnc.getPosn());
        break;
    case 4:
        Serial.println(ongEnc.getPosn());
        break;    
    default:
        break;
    }
    
    tick = millis();
    finish = micros();

    if(finish - start > UPDATE_MICROS){
        Serial.print("This is taking too long... ");
        Serial.println(finish-start);
        //exit(0);
    }

}