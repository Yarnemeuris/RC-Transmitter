#include <SPI.h>
#include "RF24.h"
#include <Wire.h>

#define CE_PIN 15
#define CSN_PIN 17

#define updateFreq 50
#define displayFreq 15

#define millisPerUpdate floor(1000 / updateFreq)
#define millisPerDisplay floor(1000 / displayFreq)

uint64_t nextDraw = millis();

void setup() {
  setupBattery(); //start MAX ic and let it get a clear first reading without too much going on

  rp2040.fifo.push(0); //signal CORE1 to start up

  setupDisplay();
}

void loop() {
  if (millis() > nextDraw) {
    nextDraw = millis() + millisPerDisplay;
    updateDisplay();
  }
}