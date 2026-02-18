#include <SPI.h>
#include "RF24.h"
#include <Wire.h>

#define CE_PIN 15
#define CSN_PIN 17

#define updateFreq 50
#define millisPerUpdate floor(1000 / updateFreq)

uint8_t buttons[8] = { 12, 13, 20, 21, 22, 23, 24, 25 };
uint8_t axes[4] = { 26, 27, 28, 29 };

uint8_t RXAddress[] = "RCT";
uint8_t TXAdress[] = "1RX";

uint64_t nextUpdate = millis();

struct stateStruct {
  int16_t axes[4];
  uint8_t buttons;
};

struct payloadStruct {
  stateStruct state;
};

RF24 radio(CE_PIN, CSN_PIN);

void setup() {
  setupBattery(); //start MAX ic and let it get a clear first reading without too much going on

  rp2040.fifo.push(0); //signal CORE1 to start up

  setupDisplay();
}

void loop(){
  delay(10);
}