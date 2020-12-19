#include <Adafruit_NeoPixel.h>
#include <math.h>

#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define PIN        6 // On Trinket or Gemma, suggest changing this to 1

#define NUMPIXELS 4

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

led h_red = {0,0,80,0, 300};

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

void loop() {
  if (h_red.startTime + h_red.duration + 5000 < millis()) {
    if (h_red.target == 80) {
      h_red = animate(h_red, 0, 500, 5000);
    } else {
      h_red = animate(h_red, 80, 300, 5000);
    }
  }

  unsigned char newBrightness = animateStep(h_red);
  if (newBrightness != h_red.current) {
    h_red.current = newBrightness;
    pixels.setPixelColor(0, pixels.Color(newBrightness, 0, 0));

    pixels.show();   // Send the updated pixel colors to the hardware.
  }
}
