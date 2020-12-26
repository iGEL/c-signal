#include <Adafruit_NeoPixel.h>
#include <math.h>

#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define PIN 7
#define NUMPIXELS 3

#define NUMLEDS 7
#define HYELLOW 0
#define HGREEN 1
#define HRED 2

#define VTGREEN 4
#define VBGREEN 6
#define VTYELLOW 5
#define VBYELLOW 3

#define DEBOUNCE 200

typedef struct {
  unsigned char current;
  unsigned char start;
  unsigned char target;
  unsigned long startTime;
  unsigned int duration;
} led;

byte rows[] = {2, 3};
const int rowCount = sizeof(rows)/sizeof(rows[0]);

byte cols[] = {4, 5, 6};
const int colCount = sizeof(cols)/sizeof(cols[0]);

bool buttons[9];
unsigned long lastChange = 0;

led leds[NUMLEDS] = {
 {0, 0, 0, 0, 0},
 {0, 0, 0, 0, 0},
 {0, 0, 255, 0, 300},
 {0, 0, 0, 0, 0},
 {0, 0, 0, 0, 0},
 {0, 0, 0, 0, 0},
 {0, 0, 0, 0, 0}
};
unsigned char vorsignal = 0;
unsigned char hauptsignal = 0;

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void log(int i, unsigned char v) {
  if(leds[i].current != v) {
    if (i == HRED) {
      Serial.print("HRED: ");
    } else if (i == HGREEN) {
      Serial.print("HGREEN: ");
    } else if (i == HYELLOW) {
      Serial.print("HYELLOW: ");
    } else if (i == VTGREEN) {
      Serial.print("VTGREEN: ");
    } else if (i == VBGREEN) {
      Serial.print("VBGREEN: ");
    } else if (i == VTYELLOW) {
      Serial.print("VTYELLOW: ");
    } else if (i == VBYELLOW) {
      Serial.print("VBYELLOW: ");
    }
    Serial.println(v);
  }
}

void animate(int idx, unsigned char target, unsigned int duration, unsigned long startDelay) {
  /* log(idx, target); */
  leds[idx] = (led) {leds[idx].current, leds[idx].current, target, millis() + startDelay, duration};
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
  for(int x=0; x<rowCount; x++) {
    pinMode(rows[x], INPUT);
  }

  for (int x=0; x<colCount; x++) {
    pinMode(cols[x], INPUT_PULLUP);
  }
}

void hp0() {
  int offset;
  animate(HYELLOW, 0, 300, 0);
  animate(HGREEN, 0, 300, 0);
  animate(HRED, 255, 300, 250);
  setVr(0);
  hauptsignal = 0;
}

void hp1() {
  int offset;

  if (hauptsignal == 0) {
    setVr(1);
    animate(HRED, 0, 300, 1000);
    offset = 1250;
  } else {
    animate(HYELLOW, 0, 300, 0);
    offset = 0;
  }
  animate(HGREEN, 255, 300, offset);
  hauptsignal = 1;
}

void hp2() {
  int offset;

  if (hauptsignal == 0) {
    setVr(2);
    animate(HRED, 0, 300, 1000);
    offset = 1250;
  } else {
    offset = 0;
  }
  animate(HGREEN, 255, 300, offset);
  animate(HYELLOW, 255, 300, offset + 50);
  hauptsignal = 2;
}

void vrOff() {
  animate(VTGREEN, 0, 300, 1300);
  animate(VBGREEN, 0, 300, 1300);
  animate(VTYELLOW, 0, 300, 1300);
  animate(VBYELLOW, 0, 300, 1300);
}

void vr0() {
  animate(VTGREEN, 0, 300, 0);
  animate(VBGREEN, 0, 300, 240);
  animate(VTYELLOW, 255, 300, 360);
  animate(VBYELLOW, 255, 300, 600);
}

void vr1() {
  animate(VTGREEN, 255, 300, 360);
  animate(VBGREEN, 255, 300, 600);
  animate(VTYELLOW, 0, 300, 0);
  animate(VBYELLOW, 0, 300, 240);
}

void vr2() {
  animate(VTGREEN, 255, 300, 360);
  animate(VBGREEN, 0, 300, 240);
  animate(VTYELLOW, 0, 300, 0);
  animate(VBYELLOW, 255, 300, 600);
}

void setVr(int h) {
  if (h == 0) {
    vrOff();
  } else if (vorsignal == 0) {
    vr0();
  } else if (vorsignal == 1) {
    vr1();
  } else if (vorsignal == 2) {
    vr2();
  }
}

void readMatrix() {
  if (millis() > lastChange + DEBOUNCE) {
    for(int colIdx = 0; colIdx < colCount; colIdx++) {
      byte curCol = cols[colIdx];
      pinMode(curCol, OUTPUT);
      digitalWrite(curCol, LOW);

      for(int rowIdx = 0; rowIdx < rowCount; rowIdx++) {
        byte curRow = rows[rowIdx];
        pinMode(curRow, INPUT_PULLUP);
        byte buttonIdx = colIdx * rowCount + rowIdx;
        buttons[buttonIdx] = !digitalRead(curRow);
        pinMode(curRow, INPUT);
      }
      pinMode(curCol, INPUT);
      lastChange = millis();
    }
  }
}

void loop() {
  readMatrix();

  if (buttons[4] && hauptsignal != 0) {
    hp0();
  } else if (buttons[2] && hauptsignal != 1) {
    hp1();
  } else if (buttons[0] && hauptsignal != 2) {
    hp2();
  } else if (buttons[5] && vorsignal != 0) {
    vorsignal = 0;
    setVr(hauptsignal);
  } else if (buttons[3] && vorsignal != 1) {
    vorsignal = 1;
    setVr(hauptsignal);
  } else if (buttons[1] && vorsignal != 2) {
    vorsignal = 2;
    setVr(hauptsignal);
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
