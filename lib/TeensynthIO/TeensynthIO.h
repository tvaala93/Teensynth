#include <Arduino.h>
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

/*  --  LEDs  --
 *   PCA1:
 *    - LED3 @IO07
 *    - LED4 @IO06
 *   PCA2:
 *    - LED1 @IO14
 *    - LED2 @IO13
 */

//TODO - Add functions for PCA9555 reads and writes

// Error codes
#define IO_ERR 1000
#define PCA_ERR 100
#define OLED_ERR 200


class PCA9555{
    private:        
        uint8_t port0_cfg;
        uint8_t port1_cfg;
        uint16_t status;
    public:
        PCA9555(int address);
        int addr;
        bool check_aliveness();
        bool config(uint8_t port0, uint8_t port1);
        uint16_t get_status();
        void read();
};


// The array holds the value -1 for the entries where a position was decremented,
// a 1 for the entries where the position was incremented
// and 0 in all the other (no change or not valid) cases.
const int8_t KNOBDIR[] = {
    0, -1,  1,  0,
    1,  0,  0, -1,
   -1,  0,  0,  1,
    0,  1, -1,  0  };

class Encoder{
    private:        
        int8_t currState;
        int8_t lastState;        
        uint8_t port;
        int8_t sigA;
        int8_t sigB;
        uint8_t pinA;
        uint8_t pinB;
        PCA9555& ioExpander;
        int16_t knobPosn;
    public:
        Encoder(PCA9555& ioExp);
        void config(uint8_t pinAA, uint8_t pinBB, uint8_t portt);
        int16_t getPosn();
        int16_t knobPosnExt;
};

int* pollAnalog();
