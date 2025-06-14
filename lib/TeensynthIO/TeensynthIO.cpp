#include <TeensynthIO.h>

/**
 * @brief Class constructor for PCA9555 object
 * 
 * @param address the i2c address of the specific physical PCA9555
 */
PCA9555::PCA9555(int address) {
  addr = address;
  //lastRead = 0;  
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

    //port0_cfg = port0;
    //port1_cfg = port1;
    
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
   * 
   * @return the 16 bits of the PCA state
   */
  uint16_t PCA9555::read(){
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
    //lastStatus = status; // store values from last read
    status = data;
    //pressed = lastStatus & ~status;
    //released = ~lastStatus & status;
    //lastRead = micros();
    return data;
  }

  /**
   * @brief Checks to see if the PCA chip resonds to i2c messages
   * 
   * @return Whether the PCA chip fully acknowledged the message
   */
  bool PCA9555::checkAliveness(){
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
   uint16_t PCA9555::getStatus(){    
    /*
    if(micros()-lastRead > UPDATE_MICROS){
      return read();
    }
    */   
      return status;
    }

    /**
     * @brief Provides read-only access to the PCA's i2c address
     * 
     * @return the PCA's i2c address
     */
    uint8_t PCA9555::getAddr(){
      return addr;
    }
  
  

/**
 * @brief Class constructor for an Encoder object
 * 
 * @param ioExp the reference to an existing PCA9555 object the encoder is connected to
 */
Encoder::Encoder(PCA9555& ioExp): ioExpander(ioExp){
  knobPosn = 0;
  knobPosnExt = 0;
  knobDirExt = 0;
  prevPosnExt = 0;
  ioExpander = ioExp;
  lastState = 0; // just as a default
  lastButton = LOW;  
}

  /**
   * @brief configure the input pins and port on the PCA955 this encoder is connected to
   * 
   * Configures the IO pins to check on the PCA9555 and initializes the knob position and last state
   * 
   * @param pinAA pin number (0-7) of the encoder pin A (as noted in KiCad)
   * @param pinBB pin number (0-7) of the encoder pin B (as noted in KiCad)
   * @param pinCC pin number (0-7) of the encoder pushbutton pin (as noted in KiCad)
   * @param portt the port number (0 or 1) the encoder pins connect to on the PCA9555
   */
  void Encoder::config(uint8_t pinAA, uint8_t pinBB, uint8_t button, uint8_t portt){
    knobPosn = 0; // Always set knob posn to 0 on init
    lastState = 0;
    pinA = pinAA;
    pinB = pinBB;
    pinPush = button;
    port = portt;
  }

  /**
   * @brief Samples the encoder position as a count from 0 (+CW/-CCW)
   * 
   * @return The number of steps the encoder has moved from 0 (+CW/-CCW)
   */
  int16_t Encoder::getPosn(){
    //TODO get sigA and sigB
    uint16_t data =  ioExpander.getStatus();
    //Serial.print("getPosn data: ");
    //Serial.println(data,BIN);
    uint8_t sigA = (data >> (pinA + (port << 3))) & 1;
    uint8_t sigB = (data >> (pinB + (port << 3))) & 1;    
    currState = sigA | (sigB << 1);

    if(currState != lastState){
      knobDir = KNOBDIR[currState | (lastState<<2)];
      knobPosn += knobDir;
      lastState = currState;
      if(currState == 3){
        knobPosnExt = knobPosn >> 2;        
      }
    }
    

    return knobPosnExt;
  }

  /**
   * @brief Function to provide the status of the encoder's button
   * 
   * @return bool indicating if a rising edge has occurred
   */
  bool Encoder::getButton(){
        
    // Read state of button
    uint16_t data =  ~(ioExpander.getStatus());
    bool reading = (data >> (pinPush + (port <<3))) & 1;
    
    bool pressed = lastButton & !reading;    
    lastButton = reading;
    return pressed;
  }

  /**
   * @brief Function to provide direction encode is turning
   * 
   * @return int denoting direction. -1=CCW, 0=Still, 1=CW
   */
  int8_t Encoder::getDir(){
    int8_t ret = 0;    
    if (prevPosnExt > knobPosnExt) {
      ret = -1;      
    } else if (prevPosnExt < knobPosnExt) {
      ret = 1;
    }
    prevPosnExt = knobPosnExt;
    return ret;    
  }


/**
 * @brief Class constructor for a TLED (Teensynth LED) object
 * 
 * @param ioExp a reference to the PCA9555 that controls this LED
 * @param port the PCA port the LED interfaces with
 * @param pin the pin (0-7) on the PCA9555 the LED is wired to
 */
  TLED::TLED(PCA9555& ioExp, uint8_t portt, uint8_t pin): ioExpander(ioExp){
  ioExpander = ioExp;
  pinLED = pin;
  port = portt;
}

  /**
   * @brief writes the LED object on (true) or off (false)
   * 
   * @param isOn a bool specifying if the LED should be on
   */
  void TLED::write(bool isOn){
    uint16_t state = ioExpander.getStatus();
    Wire.beginTransmission(ioExpander.getAddr());
    if(port==0){
      state = state & 0xFF;
      Wire.write(OUT_PORT_0);
    }
    else{
      state = state >> 8;
      Wire.write(OUT_PORT_1);
    }
    state = (state & ~(1 << pinLED)|(isOn << pinLED));
    Wire.write(state);
    Wire.endTransmission();
  }

/**
 * @brief constructor for a keyboard container object
 */
Keyboard::Keyboard(){  
  pb_rptr = 0; // read pointer
  pb_wptr = 0; // write pointer
  
  rb_rptr = 0; // read pointer
  rb_wptr = 0; // write pointer

  last0 = 0; // Value of last tracked read from PCA0
  press0 = 0; // Tracks buttons that were just pressed
  rel0 = 0; // Tracks buttons that were just released

  last1 = 0; // Value of last tracked read from PCA1
  press1 = 0; // Tracks buttons that were just pressed
  rel1 = 0; // Tracks buttons that were just released

  last2 = 0; // Value of last tracked read from PCA2
  press2 = 0; // Tracks buttons that were just pressed
  rel2 = 0; // Tracks buttons that were just released

    lastPress = 0;
    lastRelease = 0;

}

  /**
   * @brief This function calls a pressHanlder or releaseHandler 
   * if there is a new event in either buffer
   */
  std::array<int8_t, 2> Keyboard::sync(){
    std::array<int8_t,2> retVals = {-1,-1};
    if(pb_rptr != pb_wptr){retVals[0] = pressHandler();}
    if(rb_rptr != rb_wptr){retVals[1] = releaseHandler();}    
    return retVals;
  }

  /**
   * @brief Funtion to check which buttons have been pressed/released in the last scan
   * 
   * Release and Press Buffers are updated based on diff b/w current and last value
   * Read pointers are updated each time a new event is added to a buffer
   * Buttons are debounced based on how frequently this function is called
   * 
   * @param stat0 Status/last read of PCA0
   * @param stat1 Status/last read of PCA1
   * @param stat2 Status/last read of PCA2
   */
  void Keyboard::update(uint16_t stat0, uint16_t stat1, uint16_t stat2){
      press0 = last0 & ~stat0;
      rel0 = ~last0 & stat0;
      last0 = stat0;

      press1 = last1 & ~stat1;
      rel1 = ~last1 & stat1;
      last1 = stat1;

      press2 = last2 & ~stat2;
      rel2 = ~last2 & stat2;
      last2 = stat2;

      for(int i=0;i<16;i++){
          if((press0 >> i) & 1){press_buf[pb_wptr] = pca0Map[i]; pb_wptr++;}
          if((press1 >> i) & 1){press_buf[pb_wptr] = pca1Map[i]; pb_wptr++;}
          if((press2 >> i) & 1){press_buf[pb_wptr] = pca2Map[i]; pb_wptr++;}

          if((rel0 >> i) & 1){rel_buf[rb_wptr] = pca0Map[i]; rb_wptr++;}
          if((rel1 >> i) & 1){rel_buf[rb_wptr] = pca1Map[i]; rb_wptr++;}
          if((rel2 >> i) & 1){rel_buf[rb_wptr] = pca2Map[i]; rb_wptr++;}
      }

      // Set boolean: true if any key is pressed, false otherwise
      anyKeyPressed = (press0 != 0) || (press1 != 0) || (press2 != 0);
  }
  
  int Keyboard::pressHandler(){
    lastPress = press_buf[pb_rptr];
    pb_rptr++;

    return lastPress;
}
  
  int Keyboard::releaseHandler(){
    /*
    switch(rel_buf[rb_rptr]){
        case -1:
            break;
        case 25: case 26: case 27: case 28:
            break;
        default:
            //Serial.println("Unhandled");
            break;
    }
    */
    lastRelease = rel_buf[rb_rptr];
    rb_rptr++;
    return lastRelease;
}