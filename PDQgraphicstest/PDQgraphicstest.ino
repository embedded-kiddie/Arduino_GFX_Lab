/*
  Adapted from the Adafruit and Xark's PDQ graphicstest sketch.

  See end of file for original header text and MIT license info.
*/
#if   false
/*******************************************************************************
 * Start of Arduino_GFX setting
 ******************************************************************************/
#include <Arduino_GFX_Library.h>

/* OPTION 1: Uncomment a dev device in Arduino_GFX_dev_device.h */
#include "Arduino_GFX_dev_device.h"

/* Check if not Uncommented any GFX_DEV_DEVICE in OPTION 1, include OPTION 2 header files */
#ifndef GFX_DEV_DEVICE

/* OPTION 2: Manual define hardware */

/* Step 1: Define pins in Arduino_GFX_databus.h */
#include "Arduino_GFX_pins.h"

/* Step 2: Uncomment your databus in Arduino_GFX_databus.h */
#include "Arduino_GFX_databus.h"

/* Step 3: Uncomment your display driver in Arduino_GFX_display.h */
#include "Arduino_GFX_display.h"

#endif /* Manual define hardware */

#define GFX(f)    gfx->f
#define GFX_NAME  "Arduino GFX"

void gfx_setup(void) {
  // Init Display
  if (!gfx->begin())
  // if (!gfx->begin(80000000)) /* specify data bus speed */
  {
    Serial.println("gfx->begin() failed!");
  }

#ifdef GFX_BL
  pinMode(GFX_BL, OUTPUT);
  digitalWrite(GFX_BL, HIGH);
#endif
}
/*******************************************************************************
 * End of Arduino_GFX setting
 ******************************************************************************/
#else

//-------------------------------------------------------------------------------------
// Graphic library configuration
// 93410 and 77890 are for 8-bit parallel MCUFRIEND-type LCDs.
//-------------------------------------------------------------------------------------
#define USE_ADAFRUIT    false // Select library: true or false
#define USE_TFT_DRIVER  9341  // Select Driver : 9341[0], 7789[0], 9486, 7796
#define USE_OPERATION   2     // Init operation: type 1 (default) or 2 (variant) for 9341[0] and 7789[0]

#include "gfx_config.hpp"
#include "colors.h"
#endif

#ifdef ESP32
#undef F
#define F(s) (s)
#endif

#define INTERVAL  100
int32_t w, h, n, n1, cx, cy, cx1, cy1, cn, cn1;
uint8_t tsa, tsb, tsc, ds;

void setup() {
#ifdef DEV_DEVICE_INIT
  DEV_DEVICE_INIT();
#endif

  Serial.begin(115200);
  while(millis() < 1000);
  Serial.println(F(GFX_NAME " PDQ"));

  gfx_setup();

  w = GFX(width());
  h = GFX(height());
  n = min(w, h);
  n1 = n - 1;
  cx = w / 2;
  cy = h / 2;
  cx1 = cx - 1;
  cy1 = cy - 1;
  cn = min(cx1, cy1);
  cn1 = cn - 1;
  tsa = ((w <= 176) || (h <= 160)) ? 1 : (((w <= 240) || (h <= 240)) ? 2 : 3); // text size A
  tsb = ((w <= 272) || (h <= 220)) ? 1 : 2;                                    // text size B
  tsc = ((w <= 220) || (h <= 220)) ? 1 : 2;                                    // text size C
  ds = (w <= 160) ? (9+1) : ((w <= 280) ? (10+1) : (12+1));                    // digit size
}

static inline uint32_t micros_start() __attribute__((always_inline));
static inline uint32_t micros_start() {
  uint8_t oms = millis();
  while ((uint8_t)millis() == oms);
  return micros();
}

void loop(void) {
  Serial.println(F("Benchmark\tmicro-secs"));

  int32_t usecFillScreen = testFillScreen();
  serialOut(F("Screen fill\t"), usecFillScreen, INTERVAL, true);

  int32_t usecText = testText();
  serialOut(F("Text\t"), usecText, 3000, true);

  int32_t usecPixels = testPixels();
  serialOut(F("Pixels\t"), usecPixels, INTERVAL, true);

  int32_t usecLines = testLines();
  serialOut(F("Lines\t"), usecLines, INTERVAL, true);

  int32_t usecFastLines = testFastLines();
  serialOut(F("Horiz/Vert Lines\t"), usecFastLines, INTERVAL, true);

  int32_t usecFilledRects = testFilledRects();
  serialOut(F("Rectangles (filled)\t"), usecFilledRects, INTERVAL, false);

  int32_t usecRects = testRects();
  serialOut(F("Rectangles (outline)\t"), usecRects, INTERVAL, true);

  int32_t usecFilledTrangles = testFilledTriangles();
  serialOut(F("Triangles (filled)\t"), usecFilledTrangles, INTERVAL, false);

  int32_t usecTriangles = testTriangles();
  serialOut(F("Triangles (outline)\t"), usecTriangles, INTERVAL, true);

  int32_t usecFilledCircles = testFilledCircles(10);
  serialOut(F("Circles (filled)\t"), usecFilledCircles, INTERVAL, false);

  int32_t usecCircles = testCircles(10);
  serialOut(F("Circles (outline)\t"), usecCircles, INTERVAL, true);

  int32_t usecFilledArcs = testFillArcs();
  serialOut(F("Arcs (filled)\t"), usecFilledArcs, INTERVAL, false);

  int32_t usecArcs = testArcs();
  serialOut(F("Arcs (outline)\t"), usecArcs, INTERVAL, true);

  int32_t usecFilledRoundRects = testFilledRoundRects();
  serialOut(F("Rounded rects (filled)\t"), usecFilledRoundRects, INTERVAL, false);

  int32_t usecRoundRects = testRoundRects();
  serialOut(F("Rounded rects (outline)\t"), usecRoundRects, INTERVAL, true);

#ifdef CANVAS
  uint32_t start = micros_start();
  GFX(flush());
  int32_t usecFlush = micros() - start;
  serialOut(F("flush (Canvas only)\t"), usecFlush, 0, false);
#endif

  Serial.println(F("Done!"));

  uint16_t c = 4;
  int8_t d = 1;
  for (int32_t i = 0; i < h; i++) {
    GFX(drawFastHLine(0, i, w, c));
    c += d;
    if (c <= 4 || c >= 11) {
      d = -d;
    }
  }

  GFX(setCursor(0, 0));

  GFX(setTextSize(tsa));
  GFX(setTextColor(RGB565_MAGENTA));
  GFX(println(F(GFX_NAME " PDQ")));

  if (h > w) {
    GFX(setTextSize(tsb));
    GFX(setTextColor(RGB565_LIME));
    GFX(print(F("\nBenchmark ")));
    GFX(setTextSize(tsc));
    if (ds == 12) {
      GFX(print(F("   ")));
    }
    GFX(println(F("  micro-secs")));
  }

  printnice(F("Screen fill "), usecFillScreen);
  printnice(F("Text        "), usecText);
  printnice(F("Pixels      "), usecPixels);
  printnice(F("Lines       "), usecLines);
  printnice(F("H/V Lines   "), usecFastLines);
  printnice(F("Rectangles F"), usecFilledRects);
  printnice(F("Rectangles  "), usecRects);
  printnice(F("Triangles F "), usecFilledTrangles);
  printnice(F("Triangles   "), usecTriangles);
  printnice(F("Circles F   "), usecFilledCircles);
  printnice(F("Circles     "), usecCircles);
  printnice(F("Arcs F      "), usecFilledArcs);
  printnice(F("Arcs        "), usecArcs);
  printnice(F("RoundRects F"), usecFilledRoundRects);
  printnice(F("RoundRects  "), usecRoundRects);

  if ((h > w) || (h > 240)) {
    GFX(setTextSize(tsc));
    GFX(setTextColor(RGB565_LIME));
    GFX(print(F("\nBenchmark Complete!")));
  }

#ifdef CANVAS
  GFX(flush());
#endif

  delay(60 * 1000L);
}

#ifdef ESP32
void serialOut(const char *item, int32_t v, uint32_t d, bool clear)
#else
void serialOut(const __FlashStringHelper *item, int32_t v, uint32_t d, bool clear)
#endif
{
#ifdef CANVAS
  GFX(flush());
#endif
  Serial.print(item);
  if (v < 0) {
    Serial.println(F("N/A"));
  } else {
    Serial.println(v);
  }
  delay(d);
  if (clear) {
    GFX(fillScreen(RGB565_BLACK));
  }
}

#ifdef ESP32
void printnice(const char *item, long int v)
#else
void printnice(const __FlashStringHelper *item, long int v)
#endif
{
  GFX(setTextSize(tsb));
  GFX(setTextColor(RGB565_CYAN));
  GFX(print(item));

  GFX(setTextSize(tsc));
  GFX(setTextColor(RGB565_YELLOW));
  if (v < 0) {
    GFX(println(F("        N/A")));
  } else {
    char str[32+1] = { 0 };
#ifdef RTL8722DM
    sprintf(str, "%d", (int)v);
#else
    sprintf(str, "%ld", v);
#endif
    for (char *p = (str + strlen(str)) - 3; p > str; p -= 3) {
      memmove(p + 1, p, strlen(p) + 1);
      *p = ',';
    }
    while (strlen(str) < ds) {
      memmove(str + 1, str, strlen(str) + 1);
      *str = ' ';
    }
    GFX(println(str));
  }
}

int32_t testFillScreen() {
  uint32_t start = micros_start();
  // Shortened this tedious test!
  GFX(fillScreen(RGB565_WHITE));
  GFX(fillScreen(RGB565_RED));
  GFX(fillScreen(RGB565_LIME));
  GFX(fillScreen(RGB565_BLUE));
  GFX(fillScreen(RGB565_BLACK));

  return micros() - start;
}

int32_t testText() {
  uint32_t start = micros_start();
  GFX(setCursor(0, 0));

  GFX(setTextSize(1));
  GFX(setTextColor(RGB565_WHITE, RGB565_BLACK));
  GFX(println(F("Hello World!")));

  GFX(setTextSize(2));
  GFX(setTextColor(GFX(color565(0xff, 0x00, 0x00))));
  GFX(print(F("RED ")));
  GFX(setTextColor(GFX(color565(0x00, 0xff, 0x00))));
  GFX(print(F("GREEN ")));
  GFX(setTextColor(GFX(color565(0x00, 0x00, 0xff))));
  GFX(println(F("BLUE")));

  GFX(setTextSize(tsa));
  GFX(setTextColor(RGB565_YELLOW));
  GFX(println(1234.56));

  GFX(setTextColor(RGB565_WHITE));
  GFX(println((w > 128) ? 0xDEADBEEF : 0xDEADBEE, HEX));

  GFX(setTextColor(RGB565_CYAN, RGB565_WHITE));
  GFX(println(F("Groop,")));

  GFX(setTextSize(tsc));
  GFX(setTextColor(RGB565_MAGENTA, RGB565_WHITE));
  GFX(println(F("I implore thee,")));

  GFX(setTextSize(1));
  GFX(setTextColor(RGB565_NAVY, RGB565_WHITE));
  GFX(println(F("my foonting turlingdromes.")));

  GFX(setTextColor(RGB565_DARKGREEN, RGB565_WHITE));
  GFX(println(F("And hooptiously drangle me")));

  GFX(setTextColor(RGB565_DARKCYAN, RGB565_WHITE));
  GFX(println(F("with crinkly bindlewurdles,")));

  GFX(setTextColor(RGB565_MAROON, RGB565_WHITE));
  GFX(println(F("Or I will rend thee")));

  GFX(setTextColor(RGB565_PURPLE, RGB565_WHITE));
  GFX(println(F("in the gobberwartsb")));

  GFX(setTextColor(RGB565_OLIVE, RGB565_WHITE));
  GFX(println(F("with my blurglecruncheon,")));

  GFX(setTextColor(RGB565_DARKGREY, RGB565_WHITE));
  GFX(println(F("see if I don't!")));

  GFX(setTextSize(2));
  GFX(setTextColor(RGB565_RED));
  GFX(println(F("Size 2")));

  GFX(setTextSize(3));
  GFX(setTextColor(RGB565_ORANGE));
  GFX(println(F("Size 3")));

  GFX(setTextSize(4));
  GFX(setTextColor(RGB565_YELLOW));
  GFX(println(F("Size 4")));

  GFX(setTextSize(5));
  GFX(setTextColor(RGB565_GREENYELLOW));
  GFX(println(F("Size 5")));

  GFX(setTextSize(6));
  GFX(setTextColor(RGB565_LIME));
  GFX(println(F("Size 6")));

  GFX(setTextSize(7));
  GFX(setTextColor(RGB565_BLUE));
  GFX(println(F("Size 7")));

  GFX(setTextSize(8));
  GFX(setTextColor(RGB565_PURPLE));
  GFX(println(F("Size 8")));

  GFX(setTextSize(9));
  GFX(setTextColor(RGB565_LIGHTPINK));
  GFX(println(F("Size 9")));

  return micros() - start;
}

int32_t testPixels() {
  uint32_t start = micros_start();

  for (int16_t y = 0; y < h; y++) {
    for (int16_t x = 0; x < w; x++) {
      GFX(drawPixel(x, y, GFX(color565(x << 3, y << 3, x * y))));
    }
#ifdef ESP8266
    yield(); // avoid long run triggered ESP8266 WDT restart
#endif
  }

  return micros() - start;
}

int32_t testLines() {
  uint32_t start;
  int32_t x1, y1, x2, y2;

  start = micros_start();

  x1 = y1 = 0;
  y2 = h - 1;
  for (x2 = 0; x2 < w; x2 += 6) {
    GFX(drawLine(x1, y1, x2, y2, RGB565_BLUE));
  }
#ifdef ESP8266
  yield(); // avoid long run triggered ESP8266 WDT restart
#endif

  x2 = w - 1;
  for (y2 = 0; y2 < h; y2 += 6) {
    GFX(drawLine(x1, y1, x2, y2, RGB565_BLUE));
  }
#ifdef ESP8266
  yield(); // avoid long run triggered ESP8266 WDT restart
#endif

  x1 = w - 1;
  y1 = 0;
  y2 = h - 1;
  for (x2 = 0; x2 < w; x2 += 6) {
    GFX(drawLine(x1, y1, x2, y2, RGB565_BLUE));
  }
#ifdef ESP8266
  yield(); // avoid long run triggered ESP8266 WDT restart
#endif

  x2 = 0;
  for (y2 = 0; y2 < h; y2 += 6) {
    GFX(drawLine(x1, y1, x2, y2, RGB565_BLUE));
  }
#ifdef ESP8266
  yield(); // avoid long run triggered ESP8266 WDT restart
#endif

  x1 = 0;
  y1 = h - 1;
  y2 = 0;
  for (x2 = 0; x2 < w; x2 += 6) {
    GFX(drawLine(x1, y1, x2, y2, RGB565_BLUE));
  }
#ifdef ESP8266
  yield(); // avoid long run triggered ESP8266 WDT restart
#endif

  x2 = w - 1;
  for (y2 = 0; y2 < h; y2 += 6) {
    GFX(drawLine(x1, y1, x2, y2, RGB565_BLUE));
  }
#ifdef ESP8266
  yield(); // avoid long run triggered ESP8266 WDT restart
#endif

  x1 = w - 1;
  y1 = h - 1;
  y2 = 0;
  for (x2 = 0; x2 < w; x2 += 6) {
    GFX(drawLine(x1, y1, x2, y2, RGB565_BLUE));
  }
#ifdef ESP8266
  yield(); // avoid long run triggered ESP8266 WDT restart
#endif

  x2 = 0;
  for (y2 = 0; y2 < h; y2 += 6) {
    GFX(drawLine(x1, y1, x2, y2, RGB565_BLUE));
  }
#ifdef ESP8266
  yield(); // avoid long run triggered ESP8266 WDT restart
#endif

  return micros() - start;
}

int32_t testFastLines() {
  uint32_t start;
  int32_t x, y;

  start = micros_start();

  for (y = 0; y < h; y += 5) {
    GFX(drawFastHLine(0, y, w, RGB565_RED));
  }
  for (x = 0; x < w; x += 5) {
    GFX(drawFastVLine(x, 0, h, RGB565_BLUE));
  }

  return micros() - start;
}

int32_t testFilledRects() {
  uint32_t start;
  int32_t i, i2;

  start = micros_start();

  for (i = n; i > 0; i -= 6) {
    i2 = i / 2;

    GFX(fillRect(cx - i2, cy - i2, i, i, GFX(color565(i, i, 0))));
  }

  return micros() - start;
}

int32_t testRects() {
  uint32_t start;
  int32_t i, i2;

  start = micros_start();
  for (i = 2; i < n; i += 6) {
    i2 = i / 2;
    GFX(drawRect(cx - i2, cy - i2, i, i, RGB565_LIME));
  }

  return micros() - start;
}

int32_t testFilledCircles(uint8_t radius) {
  uint32_t start;
  int32_t x, y, r2 = radius * 2;

  start = micros_start();

  for (x = radius; x < w; x += r2) {
    for (y = radius; y < h; y += r2) {
      GFX(fillCircle(x, y, radius, RGB565_MAGENTA));
    }
  }

  return micros() - start;
}

int32_t testCircles(uint8_t radius) {
  uint32_t start;
  int32_t x, y, r2 = radius * 2;
  int32_t w1 = w + radius;
  int32_t h1 = h + radius;

  // Screen is not cleared for this one -- this is
  // intentional and does not affect the reported time.
  start = micros_start();

  for (x = 0; x < w1; x += r2) {
    for (y = 0; y < h1; y += r2) {
      GFX(drawCircle(x, y, radius, RGB565_WHITE));
    }
  }

  return micros() - start;
}

int32_t testFillArcs() {
#ifdef _ARDUINO_GFX_LIBRARIES_H_
  int16_t i, r = (360 > cn) ? (360 / cn) : 1;
  uint32_t start = micros_start();

  for (i = 6; i < cn; i += 6) {
    GFX(fillArc(cx1, cy1, i, i - 3, 0, i * r, RGB565_RED));
  }

  return micros() - start;
#else
  return -1;
#endif
}

int32_t testArcs() {
#ifdef _ARDUINO_GFX_LIBRARIES_H_
  int16_t i, r = (360 > cn) ? (360 / cn) : 1;
  uint32_t start = micros_start();

  for (i = 6; i < cn; i += 6) {
    GFX(drawArc(cx1, cy1, i, i - 3, 0, i * r, RGB565_WHITE));
  }

  return micros() - start;
#else
  return -1;
#endif
}

int32_t testFilledTriangles() {
  uint32_t start;
  int32_t i;

  start = micros_start();

  for (i = cn1; i > 10; i -= 5) {
    GFX(fillTriangle(cx1, cy1 - i, cx1 - i, cy1 + i, cx1 + i, cy1 + i, GFX(color565(0, i, i))));
  }

  return micros() - start;
}

int32_t testTriangles() {
  uint32_t start;
  int32_t i;

  start = micros_start();

  for (i = 0; i < cn; i += 5) {
    GFX(drawTriangle(
      cx1, cy1 - i,      // peak
      cx1 - i, cy1 + i,  // bottom left
      cx1 + i, cy1 + i,  // bottom right
      GFX(color565(0, 0, i))));
  }

  return micros() - start;
}

int32_t testFilledRoundRects() {
  uint32_t start;
  int32_t i, i2;

  start = micros_start();

  for (i = n1; i > 20; i -= 6) {
    i2 = i / 2;
    GFX(fillRoundRect(cx - i2, cy - i2, i, i, i / 8, GFX(color565(0, i, 0))));
  }

  return micros() - start;
}

int32_t testRoundRects() {
  uint32_t start;
  int32_t i, i2;

  start = micros_start();

  for (i = 20; i < n1; i += 6) {
    i2 = i / 2;
    GFX(drawRoundRect(cx - i2, cy - i2, i, i, i / 8, GFX(color565(i, 0, 0))));
  }

  return micros() - start;
}

/***************************************************
  Original sketch text:

  This is an example sketch for the Adafruit 2.2" SPI display.
  This library works with the Adafruit 2.2" TFT Breakout w/SD card
  ----> http://www.adafruit.com/products/1480

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/