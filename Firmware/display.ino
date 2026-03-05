#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 32  // OLED display height, in pixels

const uint8_t buttonDisplayLocX[] = { 18, 36, 54, 73, 91, 109 };

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, -1);

void setupDisplay() {
  Wire1.setSDA(2);
  Wire1.setSCL(3);
  Wire1.setClock(400000);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
}

void updateDisplay() {
  GFXcanvas1 canvas(SCREEN_WIDTH, SCREEN_HEIGHT);

  drawBattery(&canvas);
  drawInputs(&canvas);

  display.clearDisplay();
  display.drawBitmap(0, 0, canvas.getBuffer(), SCREEN_WIDTH, SCREEN_HEIGHT, 1, 0);
  display.display();
}

void drawBattery(struct GFXcanvas1* canvas) {
  canvas->drawBitmap(119, 1, BATTERY_IOCN, 8, 5, 1);

  int8_t batteryPercentageDigits[3] = { 1, 0, 0 };
  uint8_t batteryPercentage = getBattPercentage();
  if (batteryPercentage < 100) {
    batteryPercentageDigits[0] = -1;
    batteryPercentageDigits[1] = batteryPercentage / 10;
    batteryPercentageDigits[2] = batteryPercentage % 10;
  }

  uint8_t x = 106 + 4 * (batteryPercentage != 100);
  canvas->drawBitmap(x - 4, 1, PERCENT, 3, 5, 1);
  for (int8_t digit : batteryPercentageDigits) {
    if (digit == -1) continue;
    canvas->drawBitmap(x, 1, DIGITS[digit], 3, 5, 1);
    x += 4;
  }

  canvas->fillRect(126 - batteryPercentage / 20, 2, batteryPercentage / 20, 3, 1);
}

void drawInputs(struct GFXcanvas1* canvas) {
  for (uint8_t i = 0; i < sizeof(buttonDisplayLocX); i++) {
    if (bitRead(sharedState->buttons, i)) {
      canvas->fillCircle(buttonDisplayLocX[i], 28, 2, 1);
    } else {
      canvas->drawCircle(buttonDisplayLocX[i], 28, 2, 1);
    }
  }

  for (uint8_t i = 0; i < 4; i++) {
    uint8_t x = 1 + 65 * ((i & 2) == 2);
    uint8_t y = 7 + 10 * (i & 1);
    canvas->drawRect(x, y, 30, 7, 1);
    canvas->drawRect(30 + x, y, 30, 7, 1);

    int16_t axis = sharedState->axes[i];
    if (axis < 0) {
      canvas->fillRect(x + 30 - axis / 70, y + 1, axis / 70, 5, 1);
    } else {
      canvas->fillRect(x + 30, y + 1, axis / 70, 5, 1);
    }
  }
}