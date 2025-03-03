#include <Arduino.h>
#include <TeensynthIO.h>

PCA9555 pca0(ADDR_PCA0,pca0Map);
PCA9555 pca1(ADDR_PCA1,pca1Map);
PCA9555 pca2(ADDR_PCA2,pca2Map);

elapsedMillis tick;
elapsedMicros utick;

uint32_t start;
uint32_t finish;

float keyArr[10];

void setup(){
    Serial.begin(9600);
    while(!Serial);    
    Wire.begin();    
    Wire.setClock(400000);

    //setup PCAs    
    pca0.config(PCA0_IO0,PCA0_IO1);
    pca1.config(PCA1_IO0,PCA1_IO1);
    pca2.config(PCA2_IO0,PCA2_IO1);

    tick = millis();
    utick = micros();
}

void loop(){
    start = micros();

    if(tick%1==0){
        pca0.read();
        pca1.read();
        pca2.read();
    }

    uint8_t count = 0;
    uint16_t vals = pca0.getStatus();    
    for(int i=0;i<16;i++){
        if(~(vals >> i) & 1){
            keyArr[i] = 5.40;
            count++;
        }
        if(count >= 9){
            break;
        }
    }

    tick = millis();
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
            Serial.println("Over 800uS");
    }
}