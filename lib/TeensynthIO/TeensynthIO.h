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


int* pollAnalog();

uint8_t cfgPCA(int addr);
uint8_t cfg_ALL_PCA();
uint16_t read_pca(int addr);
