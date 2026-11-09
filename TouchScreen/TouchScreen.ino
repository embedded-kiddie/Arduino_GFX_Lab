//=====================================================================================
// A sample sketch for touch panel calibration
// using a wrapper class for the XPT2046_Touchscreen library.
//
// **IMPORTANT**
// The 2 MHz SPI bus clock configured in the XPT2046_Touchscreen library 
// may cause unstable operation. So please reconfigure it to 1 MHz.
// See https://github.com/PaulStoffregen/XPT2046_Touchscreen/issues/60
//=====================================================================================
#if   0
#define DBG_EXEC(x) x
#else
#define DBG_EXEC(x)
#endif

//-------------------------------------------------------------------------------------
// Graphic library configuration
// 93410 and 77890 are for 8-bit parallel MCUFRIEND-type LCDs.
//-------------------------------------------------------------------------------------
#define USE_ADAFRUIT    false // Select library: true (Adafruit GFX) or false (Arduino GFX)
#define USE_TFT_DRIVER  9341  // Select Driver : 9341[0], 7789[0], 9486, 7796
#define USE_OPERATION   2     // Init operation: type 1 (default) or 2 for 9341[0], 7789[0]

//-------------------------------------------------------------------------------------
// Rotation of LCD panel / Touch panel
// **WARNING**
// The touch panels of the following ILI9341 LCDs have different rotations.
// - 2.4" (https://www.lcdwiki.com/2.4inch_SPI_Module_ILI9341_SKU:MSP2402)
// - 2.8" (https://www.lcdwiki.com/2.8inch_SPI_Module_ILI9341_SKU:MSP2807)
//-------------------------------------------------------------------------------------
#define TFT_ROTATION    3     // Display panel ([0|2]: Portrait, [1|3]: Landscape)
#define TOUCH_ROTATION  0     // Touch panel (2.4":(TFT + 1) % 4, 2.8":(TFT + 2) % 4)

//-------------------------------------------------------------------------------------
// Include after configuration is complete
//-------------------------------------------------------------------------------------
#include "gfx_config.hpp"

//-------------------------------------------------------------------------------------
// Offset for the box and the size of the pen tip used for plotting in `loop()`
//-------------------------------------------------------------------------------------
#define BOX_OFFSET      30
#define PLOT_SIZE       5

//-------------------------------------------------------------------------------------
// Include the wrapper class for XPT2046_Touchscreen library
//-------------------------------------------------------------------------------------
#define USE_4POINTS_AFFINE true
#include "XPT2046_TouchCalibrate.h"
XPT2046_TouchCalibrate ts(TOUCH_CS, TOUCH_IRQ);

//-------------------------------------------------------------------------------------
// Callback function to draw the markers
// marker_pos: top-left(0), top-right(1), bottom-left(2), bottom-right(3)
//-------------------------------------------------------------------------------------
void drawMarker(uint16_t X, uint16_t Y, uint8_t marker_pos, uint16_t color) {
  GFX(setTextSize(2)); // Font: 12(w) x 16(h)
  GFX(setCursor((GFX(width()) - 192) / 2, (GFX(height()) - 16) / 2));
  GFX(print(F("Touch the corner"))); // Text: 192(w) x 16(h)

  uint16_t dx = marker_pos & 1 ? 1 : 0;
  uint16_t dy = marker_pos & 2 ? 1 : 0;

  GFX(fillRect(X - MARKER_WIDTH  * dx, Y - MARKER_HEIGHT * dy, MARKER_WIDTH,  MARKER_HEIGHT, color));
  GFX(fillRect(X - MARKER_HEIGHT * dx, Y - MARKER_WIDTH  * dy, MARKER_HEIGHT, MARKER_WIDTH,  color));
}

void setup() {
  Serial.begin(115200);
  while (!Serial || millis() < 1000);

  gfx_setup();
  GFX(fillScreen(TFT_BLACK));
  GFX(setRotation(TFT_ROTATION));

#if defined(ARDUINO_ARCH_RENESAS_UNO) || defined(ARDUINO_ARCH_AVR)
  ts.begin(GFX(width()), GFX(height()));
  ts.setRotation(TOUCH_ROTATION);

#else // ESP32-2432S028R (CYD)
  static SPIClass touch_spi(VSPI); // HSPI is assigned for LCD in "gfx_config.h"
  touch_spi.begin(TOUCH_SCLK, TOUCH_MISO, TOUCH_MOSI, TOUCH_CS);
  ts.begin(GFX(width()), GFX(height()), touch_spi);
  ts.setRotation(TOUCH_ROTATION);
#endif

#if   true
  Calibration_t cal;
  TouchCalibrationStatus status = ts.calibrate(drawMarker, TFT_WHITE, TFT_BLACK, &cal);
  if (status != TOUCH_CALIBRATION_OK) {
    Serial.print(F("Calibration failed: ")); Serial.println((int)status);
  }

#elif USE_4POINTS_AFFINE
  /* Calibration data for 4-points Affine Transformation */
  const Calibration_t cal = { .a = 0.088273, .b = 0.000411, .c = -19.460238, .d = -0.001096, .e = 0.074518, .f = -29.452278 };
  ts.setCalibration(&cal);

#else
  /* Calibration data for Proportional Mapping */
  const Calibration_t cal = { .min_x = 220, .max_x = 3800, .min_y = 460, .max_y = 3600 };
  ts.setCalibration(&cal);
#endif

  GFX(fillScreen(TFT_BLACK));
}

void loop(void) {
  static bool redraw = true;

  uint16_t x, y, z;
  if (ts.getTouch(&x, &y, &z)) {
    redraw = true;
    GFX(fillRect(x - PLOT_SIZE/2, y - PLOT_SIZE/2, PLOT_SIZE, PLOT_SIZE, TFT_WHITE));
    DBG_EXEC({
      Serial.print(x); Serial.print(",");
      Serial.print(y); Serial.print(",");
      Serial.print(z); Serial.println();
    });
  }

  else if (redraw) {
    redraw = false;
    uint16_t W = GFX(width());
    uint16_t H = GFX(height());
    GFX(drawRect(BOX_OFFSET-1, BOX_OFFSET-1, W - (BOX_OFFSET-1)*2, H - (BOX_OFFSET-1)*2, TFT_RED));
    GFX(drawRect(BOX_OFFSET,   BOX_OFFSET,   W - (BOX_OFFSET  )*2, H - (BOX_OFFSET  )*2, TFT_RED));
    GFX(drawRect(BOX_OFFSET+1, BOX_OFFSET+1, W - (BOX_OFFSET+1)*2, H - (BOX_OFFSET+1)*2, TFT_RED));
  }

  else if (Serial.available()) {
    redraw = true;
    Serial.readStringUntil('\n');
    GFX(fillScreen(TFT_BLACK));
  }
}