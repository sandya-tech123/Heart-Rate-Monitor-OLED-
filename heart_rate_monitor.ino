/*
 * Heart Rate Monitor (OLED)
 * Board : Arduino Uno / Nano / ESP32
 * Sensor: MAX30102 / MAX30105 (I2C 0x57)
 * Screen: SSD1306 128x64 OLED (I2C 0x3C)
 *
 * Libraries (Library Manager):
 *   - SparkFun MAX3010x Pulse and Proximity Sensor Library
 *   - Adafruit SSD1306
 *   - Adafruit GFX Library
 */
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "MAX30105.h"
#include "heartRate.h"

#define SCREEN_W 128
#define SCREEN_H 64
#define OLED_ADDR 0x3C
#define FINGER_THRESHOLD 50000L   // IR value below this = no finger
#define AVG_SIZE 4

Adafruit_SSD1306 display(SCREEN_W, SCREEN_H, &Wire, -1);
MAX30105 sensor;

byte rates[AVG_SIZE];
byte rateSpot = 0;
long lastBeat = 0;
float beatsPerMinute = 0;
int beatAvg = 0;
unsigned long beatFlashUntil = 0;

uint8_t wave[SCREEN_W];          // scrolling pulse waveform
long irMin = 0, irMax = 0;

void drawHeart(int x, int y, bool big) {
  if (big) {
    display.fillCircle(x + 3, y + 3, 3, SSD1306_WHITE);
    display.fillCircle(x + 9, y + 3, 3, SSD1306_WHITE);
    display.fillTriangle(x, y + 5, x + 12, y + 5, x + 6, y + 12, SSD1306_WHITE);
  } else {
    display.fillCircle(x + 2, y + 2, 2, SSD1306_WHITE);
    display.fillCircle(x + 6, y + 2, 2, SSD1306_WHITE);
    display.fillTriangle(x, y + 3, x + 8, y + 3, x + 4, y + 8, SSD1306_WHITE);
  }
}

void render(long ir, bool finger) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print(F("HEART RATE"));
  drawHeart(114, 0, millis() < beatFlashUntil);

  if (!finger) {
    display.setCursor(22, 22);
    display.print(F("PLACE FINGER"));
  } else if (beatAvg == 0) {
    display.setTextSize(2);
    display.setCursor(40, 16);
    display.print(F("--"));
  } else {
    display.setTextSize(3);
    display.setCursor(0, 12);
    if (beatAvg < 100) display.print(' ');
    display.print(beatAvg);
    display.setTextSize(1);
    display.setCursor(80, 26);
    display.print(F("BPM"));
  }

  // waveform strip (rows 40..63)
  for (int x = 0; x < SCREEN_W - 1; x++) {
    display.drawLine(x, 63 - wave[x], x + 1, 63 - wave[x + 1], SSD1306_WHITE);
  }
  display.display();
}

void setup() {
  Serial.begin(115200);
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println(F("SSD1306 not found"));
    for (;;) ;
  }
  if (!sensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println(F("MAX30102 not found"));
    for (;;) ;
  }
  sensor.setup(0x1F, 4, 2, 400, 411, 4096); // brightness, avg, mode(red+IR), rate, pulse width, range
  display.clearDisplay();
  display.display();
}

void loop() {
  long ir = sensor.getIR();
  bool finger = ir > FINGER_THRESHOLD;

  if (finger && checkForBeat(ir)) {
    long delta = millis() - lastBeat;
    lastBeat = millis();
    beatsPerMinute = 60 / (delta / 1000.0);
    if (beatsPerMinute > 30 && beatsPerMinute < 220) {
      rates[rateSpot++ % AVG_SIZE] = (byte)beatsPerMinute;
      long sum = 0;
      for (byte i = 0; i < AVG_SIZE; i++) sum += rates[i];
      beatAvg = sum / AVG_SIZE;
      beatFlashUntil = millis() + 120;
      Serial.print(F("BPM="));
      Serial.println(beatAvg);
    }
  }
  if (!finger) {
    beatAvg = 0;
    memset(rates, 0, sizeof(rates));
  }

  // update waveform (scaled to 0..22 px)
  irMin = min(irMin == 0 ? ir : irMin, ir) + 1;
  irMax = max(irMax, ir) - 1;
  for (int i = 0; i < SCREEN_W - 1; i++) wave[i] = wave[i + 1];
  wave[SCREEN_W - 1] = finger && irMax > irMin
      ? constrain(map(ir, irMin, irMax, 0, 22), 0, 22) : 0;

  render(ir, finger);
}
