//=====================================================================================
// GPIO pin assignment and Graphic Library Configuration for TFT LCDs
//
// Target MCU dev board:
//  - UNO R4 Minima, UNO R4 WiFi, Nano R4
//  - ESP32-2432S028R (CYD)
// Target Graphic library:
//  - Adafruit GFX (ILI9341, ST7789)
//  - Arduino GFX (ILI9341, ILI9486, ST7789, ST7796)
//
// Usage:
//  // 93410 and 77890 are for 8-bit parallel MCUFRIEND-type LCDs.
//  #define USE_ADAFRUIT    false // Select library: true or false
//  #define USE_TFT_DRIVER  9341  // Select Driver : 9341[0], 7789[0], 9486, 7796
//  #define USE_OPERATION   2     // Init operation: type 1 or 2 for 9341 and 7789
//  #inclue "gfx_config.h"
//=====================================================================================
#ifndef _GFX_CONFIG_HPP_
#define _GFX_CONFIG_HPP_

//-------------------------------------------------------------------------------------
// SPI GPIO pin assignment
//-------------------------------------------------------------------------------------
#if defined(ARDUINO_ARCH_RENESAS_UNO) || defined(ARDUINO_ARCH_AVR)
  // Arduino UNO R4 WiFi
  // https://github.com/arduino/ArduinoCore-renesas/blob/main/variants/UNOWIFIR4/pins_arduino.h
  // TFT_RST and TFT_DC should be configured based on your wiring
  #define TFT_RST       8         // D8
  #define TFT_DC        9         // D9
  #define TFT_CS        10        // CS
  #define TFT_MOSI      MOSI      // 11
  #define TFT_MISO      MISO      // 12
  #define TFT_SCLK      SCK       // 13
  #define TFT_BL        -1        // Connect directly to 3.3V
  #define TFT_FREQUENCY 24000000  // RA4M1: 24MHz, SPI: 12MHz

  #define TOUCH_IRQ     2
  #define TOUCH_CS      7
  #define TOUCH_MOSI    TFT_MOSI
  #define TOUCH_MISO    TFT_MISO
  #define TOUCH_SCLK    TFT_SCLK

#else  // ESP32
  // ESP32-2432S028R (CYD)
  // https://github.com/espressif/arduino-esp32/blob/master/variants/jczn_2432s028r/pins_arduino.h
  #define TFT_RST       -1
  #define TFT_DC        2
  #define TFT_MISO      12
  #define TFT_MOSI      13
  #define TFT_SCLK      14
  #define TFT_CS        15
  #define TFT_BL        21
  #define TFT_FREQUENCY 80000000  // ILI9341 for ESP32 (40MHz or 80MHz)

  #define TOUCH_IRQ     36
  #define TOUCH_CS      33
  #define TOUCH_MOSI    32
  #define TOUCH_MISO    39
  #define TOUCH_SCLK    25
#endif

//-------------------------------------------------------------------------------------
// Initial rotation and panel size
//-------------------------------------------------------------------------------------
#ifndef TFT_ROTATION
#define TFT_ROTATION  0
#endif

#ifndef TFT_WIDTH
  #if (USE_TFT_DRIVER == 9486) || (USE_TFT_DRIVER == 7796)
    #define TFT_WIDTH     (TFT_ROTATION & 1 ? 480 : 320)
    #define TFT_HEIGHT    (TFT_ROTATION & 1 ? 320 : 480)
  #else
    #define TFT_WIDTH     (TFT_ROTATION & 1 ? 320 : 240)
    #define TFT_HEIGHT    (TFT_ROTATION & 1 ? 240 : 320)
  #endif
#endif

#if defined(USE_ADAFRUIT) && USE_ADAFRUIT
  //-------------------------------------------------------------------------------------
  // Adafruit GFX
  //-------------------------------------------------------------------------------------
  #if (USE_TFT_DRIVER == 9341)
    #include "Adafruit_ILI9341.h"

  #elif (USE_TFT_DRIVER == 7789)
    #include "Adafruit_ST7789.h"

  #else
    #error "Adafruit GFX support only ILI9341 or ST7789."
  #endif

  #if defined(ARDUINO_ARCH_RENESAS_UNO) || defined(ARDUINO_ARCH_AVR)
    #if (USE_TFT_DRIVER == 9341)
    //Adafruit_ILI9341 gfx = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST, TFT_MISO); // Very slow
      Adafruit_ILI9341 gfx = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST); // Fast (TFT_RST: recommended)

    #else
    //Adafruit_ST7789 gfx = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST); // Display nothing or noise
      Adafruit_ST7789 gfx = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST); // Terribly slow (TFT_RST: Required)
    #endif

  #else // ESP32
    #if (USE_TFT_DRIVER == 9341)
      Adafruit_ILI9341 gfx = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST, TFT_MISO);

    #else
      Adafruit_ST7789 gfx = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
    #endif
  #endif

  //-----------------------------------------------------------------
  // GFX function
  //-----------------------------------------------------------------
  #define GFX(f)    gfx.f
  #define GFX_NAME  "Adafruit GFX"

  void gfx_setup(void) {
    if (TFT_BL > 0) {
      pinMode(TFT_BL, OUTPUT);
      digitalWrite(TFT_BL, HIGH);
    }

  #if (USE_TFT_DRIVER == 9341)
    GFX(begin(TFT_FREQUENCY));
    GFX(setRotation(TFT_ROTATION));
  #else
    // https://github.com/adafruit/Adafruit-GFX-Library/blob/master/Adafruit_SPITFT.h
    // https://github.com/adafruit/Adafruit-ST7735-Library/blob/master/Adafruit_ST77xx.h
    // https://github.com/adafruit/Adafruit-ST7735-Library/blob/master/Adafruit_ST7789.h
    GFX(init(TFT_WIDTH, TFT_HEIGHT, SPI_MODE0));
  //GFX(setSPISpeed(TFT_FREQUENCY)); // Adafruit_SPITFT::setSPISpeed()
    GFX(enableDisplay(true));
    GFX(invertDisplay(false));
    GFX(setRotation((TFT_ROTATION + 2) & 3));
  #endif
  }

#else
  //-------------------------------------------------------------------------------------
  // Arduino GFX
  //-------------------------------------------------------------------------------------
  #include <Arduino_GFX_Library.h>

  //-----------------------------------------------------------------
  // Initial Operation
  // Fix color misregistration compared to Adafruit GFX
  // https://github.com/moononournation/Arduino_GFX/discussions/820
  //-----------------------------------------------------------------
  #if (USE_OPERATION == 1)
    // Library default configuration
    #define GFX_CONFIG(bus, rst, rot) bus, rst, rot

  #elif (USE_TFT_DRIVER == 9341) || (USE_TFT_DRIVER == 93410)
    #define GFX_CONFIG(bus, rst, rot) bus, rst, rot, \
      false /* ips */, ILI9341_TFTWIDTH, ILI9341_TFTHEIGHT, \
      0 /* col offset 1 */,  0 /* row offset 1 */,  0 /* col offset 2 */,  0 /* row offset 2 */, \
      ili9341_type2_init_operations, sizeof(ili9341_type2_init_operations)

  #elif (USE_TFT_DRIVER == 7789) || (USE_TFT_DRIVER == 77890)
    // The library's default configuration does not produce the correct colors
    #define GFX_CONFIG(bus, rst, rot) bus, rst, rot, \
      false /* ips */, ST7789_TFTWIDTH, ST7789_TFTHEIGHT, \
      0 /* col offset 1 */,  0 /* row offset 1 */,  0 /* col offset 2 */,  0 /* row offset 2 */, \
      st7789_type2_init_operations, sizeof(st7789_type2_init_operations)

  #else
    #define GFX_CONFIG(bus, rst, rot) bus, rst, rot
  #endif

  //-----------------------------------------------------------------
  // Data bus class
  //-----------------------------------------------------------------
  #if defined(ARDUINO_ARCH_RENESAS_UNO) || defined(ARDUINO_ARCH_AVR)
    #if (USE_TFT_DRIVER == 9341)
      Arduino_DataBus *bus = new Arduino_HWSPI(TFT_DC, TFT_CS);

    #elif (USE_TFT_DRIVER == 7789)
      #if defined(ARDUINO_ARCH_RENESAS_UNO)
        Arduino_DataBus *bus = new Arduino_SWSPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, /* TFT_MISO */ GFX_NOT_DEFINED);
      #else
        Arduino_DataBus *bus = new Arduino_HWSPI(TFT_DC, TFT_CS); // Display glitches or nothing
      #endif

    #else
      // MCUFRIEND type display breakout board for Arduino UNO and Mega2560
      Arduino_DataBus *bus = new Arduino_UNOPAR8();
    #endif

  #else // ESP32
  //Arduino_DataBus *bus = new Arduino_HWSPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, TFT_MISO);          // 4-wires (8-bit) SPI (slow)
    Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, TFT_MISO, HSPI); // 4-wires (8-bit) SPI (fast)
  //Arduino_DataBus *bus = new Arduino_ESP32SPI(GFX_NOT_DEFINED, TFT_CS, TFT_SCLK, TFT_MOSI, TFT_MISO); // 3-wires (9bit) SPI
  #endif

  //-----------------------------------------------------------------
  // Display class
  //-----------------------------------------------------------------
  #if (USE_TFT_DRIVER == 9341)
    // 2.4/2.8inch SPI Module ILI9341
    // https://www.lcdwiki.com/2.4inch_SPI_Module_ILI9341_SKU:MSP2402
    // https://www.lcdwiki.com/2.8inch_SPI_Module_ILI9341_SKU:MSP2807
    Arduino_GFX *gfx = new Arduino_ILI9341(GFX_CONFIG(bus, TFT_RST, TFT_ROTATION));

  #elif (USE_TFT_DRIVER == 7789)
    Arduino_GFX *gfx = new Arduino_ST7789(GFX_CONFIG(bus, TFT_RST, TFT_ROTATION));

  #elif (USE_TFT_DRIVER == 9486)
    // 3.5inch 8-bit parallel ILI9486
    // https://www.lcdwiki.com/3.5inch_Arduino_Display-UNO
    Arduino_GFX *gfx = new Arduino_ILI9486(GFX_CONFIG(bus, A4, (TFT_ROTATION + 2) & 3));

  #elif (USE_TFT_DRIVER == 7796)
    // 3.5inch 8-bit parallel ST7789
    // https://www.lcdwiki.com/3.5inch_Arduino_Display-UNO
    Arduino_GFX *gfx = new Arduino_ST7796(GFX_CONFIG(bus, A4, (TFT_ROTATION + 2) & 3));

  #elif (USE_TFT_DRIVER == 93410)
    // 2.4inch 8-bit parallel ILI9341
    // https://www.lcdwiki.com/2.4inch_Arduino_Display
    Arduino_GFX *gfx = new Arduino_ILI9341(GFX_CONFIG(bus, A4, (TFT_ROTATION + 2) & 3));

  #elif (USE_TFT_DRIVER == 77890)
    // 2.4inch 8-bit parallel ILI9341
    // https://www.lcdwiki.com/2.4inch_Arduino_Display
    Arduino_GFX *gfx = new Arduino_ST7789(GFX_CONFIG(bus, A4, (TFT_ROTATION + 2) & 3));
  #endif

  //-----------------------------------------------------------------
  // GFX function
  //-----------------------------------------------------------------
  #define GFX(f)    gfx->f
  #define GFX_NAME  "Arduino GFX"

  void gfx_setup(void) {
    if (TFT_BL > 0) {
      pinMode(TFT_BL, OUTPUT);
      digitalWrite(TFT_BL, HIGH);
    }

    if (!GFX(begin(TFT_FREQUENCY))) {
      Serial.println("gfx->begin() failed!");
    }
  }
#endif // Adafruit GFX or Arduino GFX

//-----------------------------------------------------------------
// Color definition
//-----------------------------------------------------------------
#define TFT_BLACK             0x0000  //   0,   0,   0
#define TFT_NAVY              0x000F  //   0,   0, 128
#define TFT_DARKGREEN         0x03E0  //   0, 128,   0
#define TFT_DARKCYAN          0x03EF  //   0, 128, 128
#define TFT_MAROON            0x7800  // 128,   0,   0
#define TFT_PURPLE            0x780F  // 128,   0, 128
#define TFT_OLIVE             0x7BE0  // 128, 128,   0
#define TFT_LIGHTGREY         0xD69A  // 211, 211, 211
#define TFT_LIGHTGRAY         0xD69A  // 211, 211, 211
#define TFT_DARKGREY          0x7BEF  // 128, 128, 128
#define TFT_DARKGRAY          0x7BEF  // 128, 128, 128
#define TFT_BLUE              0x001F  //   0,   0, 255
#define TFT_GREEN             0x07E0  //   0, 255,   0
#define TFT_CYAN              0x07FF  //   0, 255, 255
#define TFT_RED               0xF800  // 255,   0,   0
#define TFT_MAGENTA           0xF81F  // 255,   0, 255
#define TFT_YELLOW            0xFFE0  // 255, 255,   0
#define TFT_WHITE             0xFFFF  // 255, 255, 255
#define TFT_ORANGE            0xFDA0  // 255, 180,   0
#define TFT_GREENYELLOW       0xB7E0  // 180, 255,   0
#define TFT_PINK              0xFE19  // 255, 192, 203
#define TFT_BROWN             0x9A60  // 150,  75,   0
#define TFT_GOLD              0xFEA0  // 255, 215,   0
#define TFT_SILVER            0xC618  // 192, 192, 192
#define TFT_SKYBLUE           0x867D  // 135, 206, 235
#define TFT_VIOLET            0x915C  // 180,  46, 226
#define TFT_ALICEBLUE         0xF7DF  // 240, 248, 255
#define TFT_ANTIQUEWHITE      0xFF5A  // 250, 235, 215
#define TFT_AQUAMARINE        0x7FFA  // 127, 255, 212
#define TFT_AZURE             0xF7FF  // 240, 255, 255
#define TFT_BEIGE             0xF5F5  // 245, 245, 220
#define TFT_BISQUE            0xFFB5  // 255, 228, 196
#define TFT_BLANCHEDALMOND    0xFF7B  // 255, 235, 205
#define TFT_BLUEVIOLET        0x901A  // 138,  43, 226
#define TFT_BURLYWOOD         0xDEB8  // 222, 184, 135
#define TFT_CADETBLUE         0x5F9E  //  95, 158, 160
#define TFT_CHARTREUSE        0x7FFF  // 127, 255,   0
#define TFT_CHOCOLATE         0xD269  // 210, 105,  30
#define TFT_CORAL             0xFF7F  // 255, 127,  80
#define TFT_CORNFLOWERBLUE    0x6495  // 100, 149, 237
#define TFT_CORNSILK          0xFFDB  // 255, 248, 220
#define TFT_CRIMSON           0xDC14  // 220,  20,  60
#define TFT_DARKGOLDENROD     0xB886  // 184, 134,  11
#define TFT_DARKKHAKI         0xBDB7  // 189, 183, 107
#define TFT_DARKMAGENTA       0x8B00  // 139,   0, 139
#define TFT_DARKOLIVEGREEN    0x556B  //  85, 107,  47
#define TFT_DARKORANGE        0xFF8C  // 255, 140,   0
#define TFT_DARKORCHID        0x9932  // 153,  50, 204
#define TFT_DARKRED           0x8B00  // 139,   0,   0
#define TFT_DARKSALMON        0xE996  // 233, 150, 122
#define TFT_DARKSEAGREEN      0x8FBC  // 143, 188, 143
#define TFT_DARKSLATEBLUE     0x483D  //  72,  61, 139
#define TFT_DARKSLATEGRAY     0x2F4F  //  47,  79,  79
#define TFT_DARKTURQUOISE     0x00CE  //   0, 206, 209
#define TFT_DARKVIOLET        0x9400  // 148,   0, 211
#define TFT_DEEPPINK          0xFF14  // 255,  20, 147
#define TFT_DEEPSKYBLUE       0x00BF  //   0, 191, 255
#define TFT_DIMGRAY           0x6969  // 105, 105, 105
#define TFT_DODGERBLUE        0x1E90  //  30, 144, 255
#define TFT_FIREBRICK         0xB222  // 178,  34,  34
#define TFT_FLORALWHITE       0xFFFA  // 255, 250, 240
#define TFT_FORESTGREEN       0x228B  //  34, 139,  34
#define TFT_GAINSBORO         0xDCDc  // 220, 220, 220
#define TFT_GHOSTWHITE        0xF8F8  // 248, 248, 255
#define TFT_GOLDENROD         0xDAA5  // 218, 165,  32
#define TFT_GOLDENRODYELLOW   0xFAF0  // 250, 250, 210
#define TFT_HONEYDEW          0xF0FF  // 240, 255, 240
#define TFT_HOTPINK           0xFF69  // 255, 105, 180
#define TFT_INDIANRED         0xCD5C  // 205,  92,  92
#define TFT_INDIGO            0x4B00  //  75,   0, 132
#define TFT_IVORY             0xFFFF  // 255, 255, 240
#define TFT_KHAKI             0xF0E6  // 240, 230, 140
#define TFT_LAVENDER          0xE6E6  // 230, 230, 250
#define TFT_LAVENDERBLUSH     0xFFF0  // 255, 240, 245
#define TFT_LAWNGREEN         0x7CFC  // 124, 252,   0
#define TFT_LEMONCHIFFON      0xFFAC  // 255, 250, 205
#define TFT_LIGHTBLUE         0xADD8  // 173, 216, 230
#define TFT_LIGHTCORAL        0xF080  // 240, 128, 128
#define TFT_LIGHTCYAN         0xE0FF  // 224, 255, 255
#define TFT_LIGHTGREEN        0x90EE  // 144, 238, 144
#define TFT_LIGHTPINK         0xFFB6  // 255, 182, 193
#define TFT_LIGHTSALMON       0xFFA0  // 255, 160, 122
#define TFT_LIGHTSEAGREEN     0x20B2  //  32, 178, 170
#define TFT_LIGHTSKYBLUE      0x87CE  // 135, 206, 250
#define TFT_LIGHTSLATEGRAY    0x7788  // 119, 136, 153
#define TFT_LIGHTSTEELBLUE    0xB0C4  // 176, 196, 222
#define TFT_LIGHTYELLOW       0xFFFF  // 255, 255, 224
#define TFT_LIME              0x07E0  //   0, 255,   0
#define TFT_LIMEGREEN         0x32CD  //  50, 205,  50
#define TFT_LINEN             0xFAF0  // 250, 240, 230
#define TFT_MEDIUMAQUAMARINE  0x66CD  // 102, 205, 170
#define TFT_MEDIUMBLUE        0x000C  //   0,   0, 205
#define TFT_MEDIUMORCHID      0xBA55  // 186,  85, 213
#define TFT_MEDIUMPURPLE      0x9370  // 147, 112, 219
#define TFT_MEDIUMSEAGREEN    0x3CB3  //  60, 179, 113
#define TFT_MEDIUMSLATEBLUE   0x7B68  // 123, 104, 238
#define TFT_MEDIUMSPRINGGREEN 0x00FA  //   0, 250, 154
#define TFT_MEDIUMTURQUOISE   0x48D1  //  72, 209, 204
#define TFT_MEDIUMVIOLETRED   0xC715  // 199,  21, 133
#define TFT_MIDNIGHTBLUE      0x1919  //  25,  25, 112
#define TFT_MINTCREAM         0xF5FF  // 245, 255, 250
#define TFT_MISTYROSE         0xFFE4  // 255, 228, 225
#define TFT_MOCCASIN          0xFFE4  // 255, 228, 181
#define TFT_NAVAJOWHITE       0xFFDE  // 255, 222, 173
#define TFT_OLDLACE           0xFDF5  // 253, 245, 230
#define TFT_OLIVEDRAB         0x6B8E  // 107, 142,  35
#define TFT_ORANGERED         0xFF45  // 255,  69,   0
#define TFT_ORCHID            0xDA70  // 218, 112, 214
#define TFT_PALEGOLDENROD     0xEEE8  // 238, 232, 170
#define TFT_PALEGREEN         0x98FB  // 152, 251, 152
#define TFT_PALETURQUOISE     0xAFEE  // 175, 238, 238
#define TFT_PALEVIOLETRED     0xDB70  // 219, 112, 147
#define TFT_PAPAYAWHIP        0xFFEF  // 255, 239, 213
#define TFT_PEACHPUFF         0xFFDA  // 255, 218, 185
#define TFT_PERU              0xCD85  // 205, 133,  63
#define TFT_PLUM              0xDDA0  // 221, 160, 221
#define TFT_POWDERBLUE        0xB0E0  // 176, 224, 230
#define TFT_REBECCAPURPLE     0x6633  // 102,  51, 153
#define TFT_ROSYBROWN         0xBC8F  // 188, 143, 143
#define TFT_ROYALBLUE         0x4169  //  65, 105, 225
#define TFT_SADDLEBROWN       0x8B45  // 139,  69,  19
#define TFT_SALMON            0xFA80  // 250, 128, 114
#define TFT_SANDYBROWN        0xF4A4  // 244, 164,  96
#define TFT_SEAGREEN          0x2E8B  //  46, 139,  87
#define TFT_SEASHELL          0xFFF5  // 255, 245, 238
#define TFT_SIENNA            0xA052  // 160,  82,  45
#define TFT_SLATEBLUE         0x6A5A  // 106,  90, 205
#define TFT_SLATEGRAY         0x7080  // 112, 128, 144
#define TFT_SLATEGREY         0x7080  // 112, 128, 144
#define TFT_GRAY              0x8410  // 128, 128, 128
#define TFT_GREY              0x8410  // 128, 128, 128
#define TFT_SNOW              0xFFFA  // 255, 250, 250
#define TFT_SPRINGGREEN       0x00FF  //   0, 255, 127
#define TFT_STEELBLUE         0x4682  //  70, 130, 180
#define TFT_TAN               0xD2B4  // 210, 180, 140
#define TFT_THISTLE           0xD8BF  // 216, 191, 216
#define TFT_TOMATO            0xFF63  // 255,  99,  71
#define TFT_TURQUOISE         0x40E0  //  64, 224, 208
#define TFT_WHEAT             0xF5DE  // 245, 222, 179
#define TFT_WHITESMOKE        0xF5F5  // 245, 245, 245
#define TFT_YELLOWGREEN       0x9ACD  // 154, 205,  50

#endif // _GFX_CONFIG_HPP_