#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>
#include <Preferences.h>  // For saving presets

#define TOUCH_PIN 15
#define LED_PIN   13
#define NUM_LEDS  12

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
Preferences prefs;

bool stripOn = true;
int lastTouchState = HIGH;
int brightness = 200;
float speedMult = 1.0;
String currentPreset = "Rainbow";
uint8_t r = 255, g = 255, b = 255;
int rainbowOffset = 0;

// For smooth random shifting
uint8_t targetR = 0, targetG = 0, targetB = 0;

uint32_t Wheel(byte pos) {
  if (pos < 85) return strip.Color(pos * 3, 255 - pos * 3, 0);
  else if (pos < 170) { pos -= 85; return strip.Color(255 - pos * 3, 0, pos * 3); }
  else { pos -= 170; return strip.Color(0, pos * 3, 255 - pos * 3); }
}

void setup() {
  Serial.begin(115200);
  pinMode(TOUCH_PIN, INPUT);
  strip.begin();

  // Load saved settings
  prefs.begin("ledprefs", true);
  brightness = prefs.getInt("brightness", 200);
  speedMult = prefs.getFloat("speedMult", 1.0);
  currentPreset = prefs.getString("preset", "Rainbow");
  r = prefs.getUInt("r", 255);
  g = prefs.getUInt("g", 255);
  b = prefs.getUInt("b", 255);
  prefs.end();

  strip.setBrightness(brightness);
  strip.show();
  Serial.println("ESP32 LED controller ready");
}

void loop() {
  handleTouch();
  handleSerialCommands();

  if (!stripOn) {
    strip.clear();
    strip.show();
    return;
  }

  if (currentPreset == "Rainbow") {
    for (int i = 0; i < NUM_LEDS; i++) {
      int colorIndex = (i * 256 / NUM_LEDS + rainbowOffset) & 255;
      strip.setPixelColor(i, Wheel(colorIndex));
    }
    strip.show();
    rainbowOffset = (rainbowOffset + 2 * speedMult);
    if (rainbowOffset > 255) rainbowOffset -= 255;
    delay(30);
  } else if (currentPreset == "Solid") {
    for (int i = 0; i < NUM_LEDS; i++) strip.setPixelColor(i, r, g, b);
    strip.show();
    delay(50);
  } else if (currentPreset == "Random Shift") {
    smoothRandomShift();
  }
}

void smoothRandomShift() {
  static unsigned long lastChange = 0;
  static unsigned long lastUpdate = 0;
  static uint8_t currentR = random(255), currentG = random(255), currentB = random(255);

  if (millis() - lastChange > (3000 / speedMult)) {
    targetR = random(255);
    targetG = random(255);
    targetB = random(255);
    lastChange = millis();
  }

  if (millis() - lastUpdate > 30) {
    currentR += (targetR - currentR) * 0.1;
    currentG += (targetG - currentG) * 0.1;
    currentB += (targetB - currentB) * 0.1;

    for (int i = 0; i < NUM_LEDS; i++) {
      strip.setPixelColor(i, currentR, currentG, currentB);
    }
    strip.show();
    lastUpdate = millis();
  }
}

void handleTouch() {
  int touchState = digitalRead(TOUCH_PIN);
  if (touchState != lastTouchState && touchState == HIGH) {
    stripOn = !stripOn;
    Serial.println(stripOn ? "Touch ON" : "Touch OFF");
  }
  lastTouchState = touchState;
}

void handleSerialCommands() {
  static String input = "";
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      parseJSON(input);
      input = "";
    } else {
      input += c;
    }
  }
}

void parseJSON(String jsonStr) {
  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, jsonStr);
  if (error) return;

  String type = doc["type"];

  if (type == "brightness") {
    brightness = doc["value"];
    strip.setBrightness(brightness);
    strip.show();
  } else if (type == "speed") {
    speedMult = doc["value"];
  } else if (type == "color") {
    r = doc["value"]["r"];
    g = doc["value"]["g"];
    b = doc["value"]["b"];
  } else if (type == "preset") {
    currentPreset = String((const char*)doc["value"]);
  } else if (type == "save") {
    saveSettings();
  }
}

void saveSettings() {
  prefs.begin("ledprefs", false);
  prefs.putInt("brightness", brightness);
  prefs.putFloat("speedMult", speedMult);
  prefs.putString("preset", currentPreset);
  prefs.putUInt("r", r);
  prefs.putUInt("g", g);
  prefs.putUInt("b", b);
  prefs.end();
  Serial.println("Settings saved!");
}
