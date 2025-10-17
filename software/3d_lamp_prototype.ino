#include <Adafruit_NeoPixel.h>

#define TOUCH_PIN 15     // TTP223 OUT
#define LED_PIN   13     // WS2812 data input
#define NUM_LEDS  12

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

bool stripOn = false;
int lastTouchState = HIGH;  // depends on TTP223 board
int rainbowOffset = 0;      // for animation

// helper: input 0–255 → rainbow color
uint32_t Wheel(byte pos) {
  if (pos < 85) {
    return strip.Color(pos * 3, 255 - pos * 3, 0);         // Red → Green
  } else if (pos < 170) {
    pos -= 85;
    return strip.Color(255 - pos * 3, 0, pos * 3);         // Green → Blue
  } else {
    pos -= 170;
    return strip.Color(0, pos * 3, 255 - pos * 3);         // Blue → Red
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(TOUCH_PIN, INPUT);

  strip.begin();
  strip.setBrightness(204); // 80% of 255 ≈ 204
  strip.show();

  Serial.println("Touch toggle WS2812 rainbow demo started.");
}

void loop() {
  int touchState = digitalRead(TOUCH_PIN);

  // edge detection: trigger on rising edge
  if (touchState != lastTouchState && touchState == HIGH) {
    stripOn = !stripOn;

    if (!stripOn) {
      strip.clear();
      strip.show();
      Serial.println("Strip OFF");
    } else {
      Serial.println("Strip ON (rainbow flow)");
    }
  }
  lastTouchState = touchState;

  // rainbow animation runs only when stripOn
  if (stripOn) {
    for (int i = 0; i < NUM_LEDS; i++) {
      int colorIndex = (i * 256 / NUM_LEDS + rainbowOffset) & 255;
      strip.setPixelColor(i, Wheel(colorIndex));
    }
    strip.show();
    rainbowOffset = (rainbowOffset + 2) & 255; // adjust speed here
    delay(30); // ~33 fps
  } else {
    delay(20); // debounce delay when OFF
  }
}
