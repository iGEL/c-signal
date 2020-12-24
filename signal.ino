#include <Adafruit_NeoPixel.h>
#include <math.h>

#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define PIN 7
#define NUMPIXELS 3

#define NUMLEDS 3
#define HYELLOW 0
#define HGREEN 1
#define HRED 2

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

typedef struct {
  unsigned char current;
  unsigned char start;
  unsigned char target;
  unsigned long startTime;
  unsigned int duration;
} led;

led leds[NUMLEDS] = {
 {0, 0, 0, 0, 0},
 {0, 0, 0, 0, 0},
 {0, 0, 255, 0, 300},
 {0, 0, 0, 0, 0},
 {0, 0, 0, 0, 0},
 {0, 0, 0, 0, 0},
 {0, 0, 0, 0, 0}
};
unsigned char hauptsignal = 0;
unsigned long lastEvent = 0;

led animate(led prev, unsigned char target, unsigned int duration, unsigned long startDelay) {
  return {prev.current, prev.current, target, millis() + startDelay, duration};
}

float easeOutQuart(float num) {
  return 1 - pow(1 - num, 4);
}

unsigned char animateStep(led px) {
  unsigned long now = millis();

  if (now < px.startTime) {
    return px.start;
  } else if (now > px.startTime + px.duration) {
    return px.target;
  } else {
    return px.start + ((px.target - px.start) * easeOutQuart((now - px.startTime) / ((float) px.duration)));
  }
}

void setup() {
  Serial.begin(9600);

  pixels.begin();
}

void hp0() {
  leds[HYELLOW] = animate(leds[HYELLOW], 0, 300, 0);
  leds[HGREEN] = animate(leds[HGREEN], 0, 300, 0);
  leds[HRED] = animate(leds[HRED], 255, 300, 250);
  hauptsignal = 0;
}

void hp1() {
  int offset;

  if (hauptsignal == 0) {
    leds[HRED] = animate(leds[HRED], 0, 300, 0);
    offset = 250;
  } else {
    leds[HYELLOW] = animate(leds[HYELLOW], 0, 300, 0);
    offset = 0;
  }
  leds[HGREEN] = animate(leds[HGREEN], 255, 300, offset);
  hauptsignal = 1;
}

void hp2() {
  int offset;

  if (hauptsignal == 0) {
    leds[HRED] = animate(leds[HRED], 0, 300, 0);
    offset = 250;
  } else {
    offset = 0;
  }
  leds[HGREEN] = animate(leds[HGREEN], 255, 300, offset);
  leds[HYELLOW] = animate(leds[HYELLOW], 255, 300, offset + 50);
  hauptsignal = 2;
}

void loop() {
  if (lastEvent + 5000 < millis()) {
    lastEvent = millis();
    int next = random(3);
    if (next == 0) {
      hp0();
    } else if (next == 1) {
      hp1();
    } else {
      hp2();
    }
  }

  for(int i = 0; i < NUMLEDS; i++) {
    leds[i].current = animateStep(leds[i]);
  }

  for(int i = 0; i < ceil(NUMLEDS / 3.0); i++) {
    unsigned char r, g, b;
    r = leds[i * 3].current;
    if (i * 3 + 1 < NUMLEDS) {
      g = leds[i * 3 + 1].current;
    } else {
      g = 0;
    }
    if (i * 3 + 2 < NUMLEDS) {
      b = leds[i * 3 + 2].current;
    } else {
      b = 0;
    }
    pixels.setPixelColor(i, pixels.Color(r, b, g));
  }

  pixels.show();
}
