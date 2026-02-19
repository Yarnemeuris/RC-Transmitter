const uint8_t RXAddress[] = "RCT";
const uint8_t TXAdress[] = "1RX";

#define millisPerUpdate floor(1000 / updateFreq)

const uint8_t buttons[6] = { 20, 21, 22, 23, 24, 25 };
const uint8_t axes[4] = { 26, 27, 28, 29 };

uint64_t nextUpdate = millis();

struct stateStruct {
  int16_t axes[4];
  uint8_t buttons;
};

struct payloadStruct {
  struct stateStruct state;
};

stateStruct* sharedState = new stateStruct;  //create a state object on the heap so it's shared between the cores.

RF24 radio(CE_PIN, CSN_PIN);

void setup1() {
  while (rp2040.fifo.available() == 0) delay(10);  //wait until core0 wants to start the second core
  rp2040.fifo.pop();

  setupInputs();
  setupRF24();

  *sharedState = getState();  //set initial values for the shared state
}

void loop1() {
  if (millis() > nextUpdate) {
    nextUpdate = millis() + millisPerUpdate;
    update();
  }
}

void update() {
  struct stateStruct currentState = getState();
  struct payloadStruct payload = { currentState };

  sendPayload(payload);

  *sharedState = currentState;
}

void setupInputs() {
  for (uint8_t button : buttons) {
    pinMode(button, INPUT_PULLUP);
  }

  for (uint8_t axis : axes) {
    pinMode(axis, INPUT);
  }

  analogReadResolution(12);
}

void setupRF24() {
  SPI.setMISO(16);
  SPI.setCS(17);
  SPI.setSCK(18);
  SPI.setMOSI(19);
  SPI.begin();

  if (!radio.begin(&SPI)) {
    Serial.println(F("radio hardware not responding!!"));
    while (1) {}
  }

  radio.setChannel(100);                        //sets to 2.41 GHZ which is outside of bluethoot and wifi range
  radio.setRetries(5, 2);                       //limits the amount of resends so it doesn't take too long
  radio.setAddressWidth(3);                     //use 3 byte addresses which is the smallest
  radio.setDataRate(RF24_250KBPS);              //slowest data rate for longest range
  radio.setPayloadSize(sizeof(payloadStruct));  //set the payload size to make transmission faster

  radio.stopListening(TXAdress);        //set the adress to transmit to
  radio.openReadingPipe(1, RXAddress);  //set the adress to read ACK from or that's how I think it works
}

bool sendPayload(struct payloadStruct payload) {
  bool report = radio.write(&payload, sizeof(payload));
  return report;
}

struct stateStruct getState() {
  struct stateStruct currentState;

  for (uint8_t i = 0; i < sizeof(axes) / sizeof(axes[0]); i++) {
    currentState.axes[i] = map(analogRead(axes[i]), 0, 4096, -2048, 2048);
  }

  for (uint8_t i = 0; i < sizeof(buttons) / sizeof(buttons[0]); i++) {
    bitWrite(currentState.buttons, i, digitalRead(buttons[i]));
  }

  return currentState;
}