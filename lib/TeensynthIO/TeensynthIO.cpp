#include <TeensynthIO.h>

/**
 * @brief Class constructor for PCA9555 object
 * 
 * @param address the i2c address of the specific physical PCA9555
 */
PCA9555::PCA9555(int address) {
  addr = address;
}

  /**
   * @brief Nulls all outputs on the PCA chip and sets the DDRs
   * 
   * This function configures the data direction of all IO pins on the PCA chip.
   * Prior to any configuration, all outputs on the chip are nulled/cleared.
   * Each pin correlates to a bit where 0=output and 1=input. 
   * Two bytes are fed in; 1 for each IO port on the PCA chip.
   * 
   * @param port0 the data direction config for port 0, msb-first
   * @param port1 the data direction config for port 1, msb-first
   * @return whether the PCA chip accepted the new configuration
   * 
   */
  bool PCA9555::config(uint8_t port0, uint8_t port1){
    //Clear output
    Wire.beginTransmission(addr);
    Wire.write(OUT_PORT_0);
    Wire.write(0x00);  
    Wire.write(0x00);
    Wire.endTransmission();

    port0_cfg = port0;
    port1_cfg = port1;
    
    //Set DDRs
    Wire.beginTransmission(addr);
    Wire.write(DDR0);
    Wire.write(port0);
    Wire.write(port1);  
    if(Wire.endTransmission() == 0){
      return true;
    }
    return false;
  }

  /**
   * @brief Take a reading of the current IO status of a PCA chip
   * 
   * This function makes the required i2c calls to get the current state
   * of both input ports on the PCA9555 chip. 
   * The status parameter of the class is then updated.
   */
  void PCA9555::read(){
    uint16_t data = 0xDEAD;
    Wire.beginTransmission(addr);
    Wire.write(IN_PORT_1);
    Wire.endTransmission();
    Wire.beginTransmission(addr);
    Wire.requestFrom(addr,2);
    if(Wire.available()){
      data = Wire.read();
    }if(Wire.available()){
      data = data << 8;
      data |= Wire.read();
    }
    status = data;    
  }

  /**
   * @brief Checks to see if the PCA chip resonds to i2c messages
   * 
   * @return Whether the PCA chip fully acknowledged the message
   */
  bool PCA9555::check_aliveness(){
    uint8_t err = 4;
    Wire.beginTransmission(addr);
    err = Wire.endTransmission();
    if(err == 0){
      return true;
    }
    return false;
  }

  /**
   * @brief Provides read-only access to the PCA's input registers
   * 
   * @return The status of all 16 IO bits, msb-first. Bits 0-7 are Port 0; 8-15 are Port 1. 
   */
  uint16_t PCA9555::get_status(){
    return status;
  }

/**
 * @brief Class constructor for an Encoder object
 * 
 * @param ioExp the reference to an existing PCA9555 object the encoder is connected to
 */
Encoder::Encoder(PCA9555& ioExp): ioExpander(ioExp){
  knobPosn = 0;
  knobPosnExt = 0;
  ioExpander = ioExp;
  lastState = 0; // just as a default
}

  /**
   * @brief configure the input pins and port on the PCA955 this encoder is connected to
   * 
   * Configures the IO pins to check on the PCA9555 and initializes the knob position and last state
   * 
   * @param pinAA pin number (0-7) of the encoder pin A (as noted in KiCad)
   * @param pinBB pin number (0-7) of the encoder pin B (as noted in KiCad)
   * @param portt the port number (0 or 1) the encoder pins connect to on the PCA9555
   */
  void Encoder::config(uint8_t pinAA, uint8_t pinBB, uint8_t portt){
    knobPosn = 0; // Always set knob posn to 0 on init
    pinA = pinAA;
    pinB = pinBB;
    port = portt;

    uint16_t data = ioExpander.get_status();
    // data: msb-first output of all pins read off ioExpander
    // port: 0 or 1 of spec'd PCA9555
    // pinA/B: pin 0-7 of the spec'd port
    // port << 3 accomodates for which port is being read/requested
    sigA = (data >> (pinA + (port << 3))) & 0x01;
    sigB = (data >> (pinB + (port << 3))) & 0x01;    
    lastState = sigA | (sigB << 1); // initialize knob's last state  
  }

  /**
   * @brief Samples the encoder position as a count from 0 (+CW/-CCW)
   * 
   * @return The number of steps the encoder has moved from 0 (+CW/-CCW)
   */
  int16_t Encoder::getPosn(){
    //TODO get sigA and sigB
    uint16_t data =  ioExpander.get_status();
    //Serial.print("getPosn data: ");
    //Serial.println(data,BIN);
    sigA = (data >> (pinA + (port << 3))) & 1;
    sigB = (data >> (pinB + (port << 3))) & 1;    
    currState = sigA | (sigB << 1);

    if(currState != lastState){
      knobPosn += KNOBDIR[currState | (lastState<<2)];
      lastState = currState;      
      if(currState == 3){
        knobPosnExt = knobPosn >> 2;
      }
    }
    
    return knobPosnExt;
  }


int* pollAnalog() {
  // Static array: valid only within the function scope
  // int arr[5] = {1, 2, 3, 4, 5}; 
  // return arr; // Avoid returning address of local variable

  // Dynamically allocated array: valid outside the function scope
  int* arr = new int[5]{1, 2, 3, 4, 5};
  return arr;
}
