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

  } else if (currentPreset == "Smooth Shift") {
    smoothShift();

  } else if (currentPreset == "Fire Flicker") {
    fireFlicker();

  } else if (currentPreset == "Waves") {
    waves();

  } else if (currentPreset == "Pulse Sync") {
    pulseSync();

  } else if (currentPreset == "Ocean Flow") {
    oceanFlow();
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

// ===== PRESETS =====
void smoothShift() {
  static float hue = 0;
  hue += 0.5 * speedMult;  // increase for faster color change
  if (hue >= 360) hue -= 360;

  // Convert HSV to RGB (simple function below)
  uint8_t rr, gg, bb;
  HSVtoRGB(hue, 1.0, 1.0, rr, gg, bb);

  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, rr, gg, bb);
  }
  strip.show();
  delay(30);
}

void fireFlicker() {
  for (int i = 0; i < NUM_LEDS; i++) {
    int flicker = random(180, 255);
    int rVal = flicker;
    int gVal = random(flicker * 0.3, flicker * 0.6);
    int bVal = random(0, flicker * 0.1);
    strip.setPixelColor(i, rVal, gVal, bVal);
  }
  strip.show();
  delay(random(40, 80) / speedMult);
}

void waves() {
    static float pos = 0;          // current position in "wave"
    static float dir = 1;          // 1 = forward, -1 = backward
    float step = 0.2 * speedMult;  // controls speed

    // move position
    pos += dir * step;
    if (pos >= NUM_LEDS - 1) { dir = -1; pos = NUM_LEDS - 1; } // reflect
    if (pos <= 0) { dir = 1; pos = 0; }                          // reflect

    // draw sine brightness
    for (int i = 0; i < NUM_LEDS; i++) {
        float distance = abs(i - pos);
        float intensity = max(0.0f, cos(distance * 3.14159 / 2)); // peak at pos, drops off
        strip.setPixelColor(i, r * intensity, g * intensity, b * intensity);
    }

    strip.show();
    delay(30);
}

void pulseSync() {
  static float angle = 0;
  angle += 0.05 * speedMult;
  float intensity = (sin(angle) + 1.0) / 2.0 * 0.6 + 0.4;  // 0.4 → 1.0
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, r * intensity, g * intensity, b * intensity);
  }
  strip.show();
  delay(30);
}

void oceanFlow() {
  static float hue = 180; // ocean blue
    hue += 0.05 * speedMult;
    if (hue > 200) hue = 180;

    for (int i = 0; i < NUM_LEDS; i++) {
        float intensity = 0.5 + 0.5 * ((float)random(50, 101) / 100.0); // mostly bright, some dimmer
        uint8_t rr, gg, bb;
        HSVtoRGB(hue, 0.8, 0.6 * intensity, rr, gg, bb); // scale brightness by intensity
        strip.setPixelColor(i, rr, gg, bb);
    }
    strip.show();
    delay(50 / speedMult); 
}

void HSVtoRGB(float h, float s, float v, uint8_t &r, uint8_t &g, uint8_t &b) {
  float c = v * s;
  float x = c * (1 - abs(fmod(h / 60.0, 2) - 1));
  float m = v - c;
  float r1, g1, b1;
  if (h < 60) { r1 = c; g1 = x; b1 = 0; }
  else if (h < 120) { r1 = x; g1 = c; b1 = 0; }
  else if (h < 180) { r1 = 0; g1 = c; b1 = x; }
  else if (h < 240) { r1 = 0; g1 = x; b1 = c; }
  else if (h < 300) { r1 = x; g1 = 0; b1 = c; }
  else { r1 = c; g1 = 0; b1 = x; }
  r = (r1 + m) * 255;
  g = (g1 + m) * 255;
  b = (b1 + m) * 255;
}
