#include <BQ25887.h>

#define designCap 8000   //value for 1250mAh
#define ichgTerm 0x0640  //default value needs to change
#define VEmpty 0xA561    //VE = 3.3V, VR = 3.88V

BQ25887 charger = BQ25887();

void setupBattery() {
  Wire.setSDA(4);
  Wire.setSCL(5);
  Wire.begin();

  uint16_t statusPOR = readRegister(0x00) & 0x0002;
  if (statusPOR == 1) loadBattConfig();

  charger.setInputCurrentLimit(0.5);
}

uint8_t getBattPercentage() {
  uint16_t data = readRegister(0x06);  //read the data from the MAX ic

  return highByte(data);
}

void loadBattConfig() {
  while (readRegister(0x3D) & 1) delay(10);  //wait for the ic to startup

  uint16_t HibCFG = readRegister(0xBA);  //Store original HibCFG value

  writeRegister(0x60, 0x90);  // Exit Hibernate Mode
  writeRegister(0xBA, 0x0);
  writeRegister(0x60, 0x0);

  writeRegister(0x18, designCap);  // Write DesignCap
  writeRegister(0x1E, ichgTerm);   // Write IchgTerm
  writeRegister(0x3A, VEmpty);     // Write VEmpty
  writeRegister(0xDB, 0x8000);     // Write ModelCFG

  while (readRegister(0xDB) & 0x8000) delay(10);  // wait until ModelCFG.Refresh is 0

  writeRegister(0xBA, HibCFG);  // Restore Original HibCFG value

  uint16_t status = readRegister(0x00);
  writeAndVerify(0x00, status & 0xFFFD);  //clear POR bit
}

void writeAndVerify(uint8_t reg, uint16_t value) {
  int attempt = 0;
  uint16_t dataRead = 0;

  //write and read data according to the datasheet
  while (dataRead != value && attempt != 0 && attempt < 3) {
    writeRegister(reg, value);
    delay(1);
    dataRead = readRegister(reg);
    attempt++;
  }
}

uint16_t readRegister(uint8_t reg) {
  Wire.beginTransmission(0x36);  //write register address to read from
  Wire.write((byte)reg);
  Wire.endTransmission();

  Wire.requestFrom(0x36, 2);    //request data
  while (!Wire.available()) {}  //wait for data to be available
  uint16_t data = (Wire.read() << 8) + Wire.read();
  return data;
}

void writeRegister(uint8_t reg, uint16_t value) {
  byte data[2] = { highByte(value), lowByte(value) };
  Wire.beginTransmission(0x36);
  Wire.write((byte)reg);  //write register address to read from
  Wire.write(data, 2);    //write data
  Wire.endTransmission();
}