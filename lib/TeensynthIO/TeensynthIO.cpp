#include <TeensynthIO.h>

int* pollAnalog() {
    // Static array: valid only within the function scope
    // int arr[5] = {1, 2, 3, 4, 5}; 
    // return arr; // Avoid returning address of local variable

    // Dynamically allocated array: valid outside the function scope
    int* arr = new int[5]{1, 2, 3, 4, 5};
    return arr;
}


uint8_t cfgPCA(int addr, int port0, int port1){
    //Clear output
  Wire.beginTransmission(addr);
  Wire.write(OUT_PORT_0);
  Wire.write(0x00);  
  Wire.write(0x00);
  Wire.endTransmission();
  
  //Set DDRs
  Wire.beginTransmission(addr);
  Wire.write(DDR0);
  Wire.write(port0);
  Wire.write(port1);  
  return Wire.endTransmission();  
}


uint8_t cfg_ALL_PCA(){
    byte err = cfgPCA(ADDR_PCA0,PCA0_IO0,PCA0_IO1);
    if(err==0){
        err = cfgPCA(ADDR_PCA1,PCA1_IO0,PCA1_IO1);
        if(err==0){
            err = cfgPCA(ADDR_PCA2,PCA2_IO0,PCA2_IO1);
        }
    }
    if(err!=0){
        Serial.println("Error on PCA setup");
    }
}

// Get status of all 16bits for given PCA
uint16_t read_pca(int addr){
    uint16_t data = 0;
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
    return data;  
  }