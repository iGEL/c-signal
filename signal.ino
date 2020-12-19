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

unsigned long aniStartTime = 0;
float aniDuration = 1000;
int aniStartColor;
int aniTargetColor;
int aniCurrentColor = 0;

void setup() {
  Serial.begin(9600);

  pixels.begin();
}

float easeOutQuart(float num) {
  return 1 - pow(1 - num, 4);
}

void loop() {
  unsigned long now = millis();
  if (aniStartTime + aniDuration + 5000 < now) {
    if (aniStartColor == 80) {
      aniStartColor = 0;
      aniTargetColor = 80;
    } else {
      aniStartColor = 80;
      aniTargetColor = 0;
    }
    aniStartTime = now + 5000;
  }

  int newColor;
  if (now < aniStartTime) {
    newColor = aniStartColor;
  } else if (now > aniStartTime + aniDuration) {
    newColor = aniTargetColor;
  } else {
    newColor = (int) aniStartColor + ((aniTargetColor - aniStartColor) * easeOutQuart((now - aniStartTime) / aniDuration));
  }

  if(newColor != aniCurrentColor) {
    aniCurrentColor = newColor;
    if(newColor >= 0 && newColor < 256) {
      pixels.setPixelColor(0, pixels.Color(newColor, 0, 0));
    }

    pixels.show();   // Send the updated pixel colors to the hardware.
  }
}
