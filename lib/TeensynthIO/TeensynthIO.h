//#include <Arduino.h>
#include <Wire.h>

// Analog Inputs
#define FREQ 22 // Frequency pin for filter
#define RES 17 // Resonance pin for filter
#define CON 16 // Amount of contour pin for filter
#define VOL 15 // Volume knob pin
#define RATE 14 // Rate control knob pin 

// I2C Addresses
#define ADDR_OLED 0x3c // SSD1306 display; 128x64
#define ADDR_PCA0 0x20 // PCA9555 io expander
#define ADDR_PCA1 0x21
#define ADDR_PCA2 0x22

//PCA9555 Ctrl Regs
#define IN_PORT_0   0x00
#define IN_PORT_1   0x01
#define OUT_PORT_0  0x02
#define OUT_PORT_1  0x03
#define POL_INV_0   0x04
#define POL_INV_1   0x05
#define DDR0        0x06
#define DDR1        0x07

//PCA9555 IO
// 0 = output, 1 = input
#define PCA0_IO0 0b11111111
#define PCA0_IO1 0b11111111
#define PCA1_IO0 0b00111111
#define PCA1_IO1 0b11111111
#define PCA2_IO0 0b11111111
#define PCA2_IO1 0b11100111

#define UPDATE_MICROS 400

/*  --  LEDs  --
 *   PCA1:
 *    - LED3 @IO07
 *    - LED4 @IO06
 *   PCA2:
 *    - LED1 @IO14
 *    - LED2 @IO13
 */


// Error codes
#define IO_ERR 1000
#define PCA_ERR 100
#define OLED_ERR 200

//Key mapping
const int8_t pca0Map[] = {
    4, 2, 1, 3, 5, 7, 8,10,
    9,-1,-1,-1,-1,-1,-1, 6
};
const int8_t pca1Map[] = {
    12,13,28,27,26,25,-1,-1,
    14,-1,-1,-1,-1,-1,-1,11
};
const int8_t pca2Map[] = {
    18,16,15,17,19,20,22,24,
    21,23,-1,-1,-1,-1,-1,-1
};


class PCA9555{
    private:        
        //uint8_t port0_cfg;
        //uint8_t port1_cfg;
        //uint16_t status;
        //uint16_t lastStatus;
        //elapsedMicros lastRead;
        //const int8_t* keyMap;
        int addr;
        uint16_t status;
    public:
        PCA9555(int address);        
        //uint16_t lastStatus;        
        bool checkAliveness();
        bool config(uint8_t port0, uint8_t port1);
        uint16_t getStatus();
        uint8_t getAddr();
        uint16_t read();        
};


// The array holds the value -1 for the entries where a position was decremented,
// a 1 for the entries where the position was incremented
// and 0 in all the other (no change or not valid) cases.
const int8_t KNOBDIR[] = {
    0, -1,  1,  0,
    1,  0,  0, -1,
   -1,  0,  0,  1,
    0,  1, -1,  0  
};

class Encoder{
    private:        
        int8_t currState;
        int8_t lastState;        
        uint8_t port;        
        uint8_t pinA;
        uint8_t pinB;
        uint8_t pinPush;
        PCA9555& ioExpander;
        int16_t knobPosn;
        int8_t knobDir;
        int knobPosnExt;
        int prevPosnExt;
        int8_t knobDirExt;
    public:
        Encoder(PCA9555& ioExp);
        void config(uint8_t pinAA, uint8_t pinBB, uint8_t pinCC, uint8_t portt);
        int16_t getPosn();
        bool getButton();
        int8_t getDir();        
};


class TLED{
    private:
        PCA9555& ioExpander;
        uint8_t pinLED;
        uint8_t port;
    public:
        TLED(PCA9555& ioExp,uint8_t portt,uint8_t pin);
        void write(bool isOn);    
};

/*
int* pollAnalog();

const float keyLookup[] = {

};
*/

class Keyboard{
    private:
        uint16_t last0 ; // Value of last tracked read from PCA0
        uint16_t press0; // Tracks buttons that were just pressed
        uint16_t rel0; // Tracks buttons that were just released
        uint16_t last1; // Value of last tracked read from PCA1
        uint16_t press1; // Tracks buttons that were just pressed
        uint16_t rel1; // Tracks buttons that were just released
        uint16_t last2; // Value of last tracked read from PCA2
        uint16_t press2; // Tracks buttons that were just pressed
        uint16_t rel2; // Tracks buttons that were just released
        int8_t press_buf[256];
        int8_t rel_buf[256];
        uint8_t pb_rptr; // read pointer
        uint8_t pb_wptr; // write pointer        
        uint8_t rb_rptr; // read pointer
        uint8_t rb_wptr; // write pointer
    public:
        Keyboard();
        void update();
        void key_handler(uint16_t stat0, uint16_t stat1, uint16_t stat2);        
};
