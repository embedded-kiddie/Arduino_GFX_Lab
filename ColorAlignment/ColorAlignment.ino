//=====================================================================================
// Checking the compatibility of Adafruit GFX and Arduino GFX using ILI9341/ST7789
//
// Target MCU board:
//  - Arduino UNO R4
//  - ESP32-2432S028R (aka CYD)
// Required libraries:
//  - https://github.com/adafruit/Adafruit-GFX-Library
//    - https://github.com/adafruit/adafruit_ili9341
//    - https://github.com/adafruit/Adafruit-ST7735-Library
//  - https://github.com/moononournation/Arduino_GFX
//  - https://github.com/Bodmer/TJpg_Decoder
//=====================================================================================
#include <Arduino.h>
#include <SPI.h>

//-------------------------------------------------------------------------------------
// Select one image file, or comment out all of them
//-------------------------------------------------------------------------------------
#include "panda_jpg.h"
//#include "panda_png.h"
//#include "OpenWeatherLogo.h" // contains png (top) and jpg (bottom) image

//-------------------------------------------------------------------------------------
// Graphic library configuration
// 93410 and 77890 are for 8-bit parallel MCUFRIEND-type LCDs.
//-------------------------------------------------------------------------------------
#define USE_ADAFRUIT    false // Select library: true or false
#define USE_TFT_DRIVER  9341  // Select Driver : 9341[0], 7789[0], 9486, 7796
#define USE_OPERATION   2     // Init operation: type 1 (default) or 2 (variant) for 9341[0] and 7789[0]

#include "gfx_config.hpp"

#if !USE_ADAFRUIT
  #define drawRGBBitmap draw16bitRGBBitmap
#endif

// Include the jpeg decoder library
#include <TJpg_Decoder.h>

#ifndef RGB565
#define RGB565(r, g, b) ((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | ((b) >> 3))
#endif

#define TFT_BLACK     0x0000
#define TFT_WHITE     0xFFFF
#define TFT_ORANGE    RGB565(233,  109,  73)

//-------------------------------------------------------------------------------------
// Callback function to draw image block for Tiny JPEG Decompressor
//-------------------------------------------------------------------------------------
static bool DrawTjpeg(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap) {
  // Stop further decoding as image is running off bottom of screen
  if (y >= GFX(height())) return false;

    // In ILI9341 library this function clips the image block at TFT boundaries
  GFX(drawRGBBitmap(x, y, bitmap, w, h));

  // Return true to decode next block
  return true;
}

//-------------------------------------------------------------------------------------
// Draw the specified images
//-------------------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  while(millis() < 1000);

  // Initialize the TFT
  gfx_setup();

  // Time recorded for test purposes
  uint32_t t = millis();

#if defined(_PANDA_JPG_H_) || defined(_PANDA_PNG_H_) || defined(_OPENWEATHERLOGO_H_)
  uint16_t x = 0, y = 0, dy = 0;

  #if defined(_OPENWEATHERLOGO_H_)
    GFX(fillScreen(TFT_WHITE));

    // Draw the image at center
    dy = (IMAGE_HEIGHT / 3) * 2;
    x = (GFX(width())  - IMAGE_WIDTH ) / 2;
    y = (GFX(height()) - IMAGE_HEIGHT) / 2 - dy;
  #endif

  #if defined(_PANDA_PNG_H_) || defined(_OPENWEATHERLOGO_H_)
    GFX(drawRGBBitmap(x, y, image_png, IMAGE_WIDTH, IMAGE_HEIGHT));
  #endif

  #if defined(_PANDA_JPG_H_) || defined(_OPENWEATHERLOGO_H_)
    // The jpeg image can be scaled by a factor of 1, 2, 4, or 8
    TJpgDec.setJpgScale(1);

    // The decoder must be given the exact name of the rendering function above
    TJpgDec.setCallback(DrawTjpeg);

    // Get the width and height in pixels of the jpeg if you wish
    TJpgDec.getJpgSize(&x, &y, image_jpg, sizeof(image_jpg));
    printf("Width: %d, Height: %d, Size: %d\n", x, y, sizeof(image_jpg));

    // Draw the image at center
    x = (GFX(width())  - x) / 2;
    y = (GFX(height()) - y) / 2 + dy;
    TJpgDec.drawJpg(x, y, image_jpg, sizeof(image_jpg));
  #endif

#else
  // Draw a rectangle to adjust `[col|row] offset [1|2]` in Arduino GFX
  GFX(setRotation(3)); // Set rotation to 0, 1, 2, 3
  GFX(fillScreen(TFT_BLACK));
  GFX(startWrite());
  GFX(writeFillRect(0, 0, GFX(width()), GFX(height()), TFT_ORANGE));
  GFX(endWrite());
  GFX(drawRect(0, 0, GFX(width()), GFX(height()), TFT_WHITE));
#endif

  // How much time did rendering take
  t = millis() - t;
  printf("%s: %d ms\n", GFX_NAME, t);
}

void loop() {}