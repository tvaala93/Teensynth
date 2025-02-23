#include <Arduino.h>
#include <cmath>
using namespace std;

#define C2 65.41
#define D2 73.42
#define E2 82.41
#define F2 87.31
#define G2 98.00
#define AA2 110.0
#define B2 123.5
#define C3 130.8
#define D3 146.8
#define E3 164.8
#define F3 174.6
#define G3 196.0
#define AA3 220.0
#define B3 246.9
#define C4 261.6

// Working with an input of 0 tunes to a C0 (16.35Hz) 
#define ROOT_FREQ 16.352

float volt_to_freq(float volt);
