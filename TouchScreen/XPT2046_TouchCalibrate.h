//=====================================================================================
// A wrapper class for XPT2046_Touchscreen that adds touch panel calibration.
// Copyright (c) 2026, embeddedkiddie@gmail.com All Rights Reserved.
// Released under the MIT license (https://opensource.org/licenses/mit-license.php)
//=====================================================================================
#ifndef _XPT2046_TOUCH_CALIBRATE_H_
#define _XPT2046_TOUCH_CALIBRATE_H_

#include "XPT2046_Touchscreen.h"      // https://github.com/PaulStoffregen/XPT2046_Touchscreen
#include "./src/touch_calibration.h"  // 4-points affine transformation calibration

#ifdef __cplusplus
extern "C" {
#endif

//-------------------------------------------------------------------------------------
// Configuration of a calibration algorithm
// - true : 4-points affine transformation calibration using the least-squares method
// - false: Proportional mapping based on the min/max values ​​of raw data
//-------------------------------------------------------------------------------------
#ifndef USE_4POINTS_AFFINE
#define USE_4POINTS_AFFINE  true
#endif

#if USE_4POINTS_AFFINE
  #include "./src/touch_calibration.hpp"
  typedef TouchCalibration Calibration_t;
#else
  typedef struct {
    uint16_t min_x, max_x, min_y, max_y;
  } Calibration_t;
#endif

//-------------------------------------------------------------------------------------
// Size and position of calibration marker
//-------------------------------------------------------------------------------------
#define MARKER_WIDTH      30    // The long side of a rectangle [pixel]
#define MARKER_HEIGHT     5     // The shorter side of a rectangle [pixel]
#if USE_4POINTS_AFFINE
#define MARKER_OFFSET     20    // Offset from the corner of the panel [pixel]
#else
#define MARKER_OFFSET     0     // Must be at the very edge of the touch panel
#endif

//-------------------------------------------------------------------------------------
// Configuration for sampling the touched points
//-------------------------------------------------------------------------------------
#define XTC_Z_THRESHOLD   400   // Minimum pressure at the touch point
#define XTC_SAMPLE_POINTS 5     // Number of samples per marker
#define XTC_SAMPLE_PERIOD 1     // Sampling period [msec]

//-------------------------------------------------------------------------------------
// First-order low-pass filter applied to raw data
//-------------------------------------------------------------------------------------
#define XTC_FILTER_DT     ((float)XTC_SAMPLE_PERIOD / 1000.0f)  // Sampling period [sec]
#define XTC_FILTER_TAU    0.01f   // Time constant for approaching the final value [sec]
#define XTC_CALIB_THRESH  10      // Threshold ​​for raw and filtered data for calibration
#define XTC_TOUCH_THRESH  400     // Threshold ​​for raw and filtered data for getTouch()
#define XTC_TOUCH_LIMIT   30      // Sampling time limit for getTouch() [msec]

class XTC_Filter {
private:
  static constexpr float alpha = XTC_FILTER_TAU / (XTC_FILTER_DT + XTC_FILTER_TAU);
  float output = 0.0f;
public:
  float filter(float input) {
    return (output = alpha * output + (1.0f - alpha) * input);
  }
};

//-------------------------------------------------------------------------------------
// Defining the class that inherits from XPT2046 libraries
//-------------------------------------------------------------------------------------
class XPT2046_TouchCalibrate : public XPT2046_Touchscreen {
  // Inheriting constructor from the parent class
  using XPT2046_Touchscreen::XPT2046_Touchscreen;

private:
  bool calibrated = false;
  Calibration_t cal = { 0, };
  uint16_t max_X = 0, max_Y = 0;
  uint16_t z_threshold = XTC_Z_THRESHOLD;

public:
  bool begin(uint16_t width, uint16_t height, SPIClass &spi = SPI) {
    max_X = width  - 1;
    max_Y = height - 1;
    return XPT2046_Touchscreen::begin(spi);
  }

  void setThreshold(uint16_t threshold) {
    z_threshold = threshold;
  }

  bool getTouch(uint16_t *x, uint16_t *y, uint16_t *z = nullptr) {
    int n = 0;
    XTC_Filter fx, fy;
    while (calibrated && touched() && ++n < XTC_TOUCH_LIMIT) {
      auto p = XPT2046_Touchscreen::getPoint();
      if (p.z > z_threshold &&
          abs((int)p.x - (int)roundf(fx.filter(p.x))) < XTC_TOUCH_THRESH &&
          abs((int)p.y - (int)roundf(fy.filter(p.y))) < XTC_TOUCH_THRESH) {
#if USE_4POINTS_AFFINE
        int xx = (int)roundf(cal.a * (float)p.x + cal.b * (float)p.y + cal.c);
        int yy = (int)roundf(cal.d * (float)p.x + cal.e * (float)p.y + cal.f);
#else
        int xx = map(p.x, cal.min_x, cal.max_x, 0, max_X);
        int yy = map(p.y, cal.min_y, cal.max_y, 0, max_Y);
#endif
        *x = (uint16_t)constrain(xx, 0, max_X);
        *y = (uint16_t)constrain(yy, 0, max_Y);
        if (z) { *z = p.z; }
        return true;
      }
      delay(1);
    }
    return false;
  }

  bool setCalibration(const Calibration_t *_cal) {
    cal = *_cal;
    return calibrated = true;
  }

private:
  int samplePoints(
    uint16_t X, uint16_t Y, TouchCalibrationPoint *points,
    void (*draw)(uint16_t X, uint16_t Y, uint8_t marker_pos, uint16_t color),
    uint8_t marker_pos, uint16_t fg, uint16_t bg
  ) {
    draw(X, Y, marker_pos, fg);
    while (touched());

    int n = 0;
    XTC_Filter fx, fy, fz;

    while (n < XTC_SAMPLE_POINTS) {
      while (!touched());

      TS_Point p = XPT2046_Touchscreen::getPoint();

      DBG_EXEC({
        Serial.print(p.x); Serial.print(",");
        Serial.print(p.y); Serial.print(",");
        Serial.print(p.z); Serial.println();
      });

      if (abs((int)p.z - (int)roundf(fz.filter(p.z))) < XTC_CALIB_THRESH &&
          abs((int)p.x - (int)roundf(fx.filter(p.x))) < XTC_CALIB_THRESH &&
          abs((int)p.y - (int)roundf(fy.filter(p.y))) < XTC_CALIB_THRESH) {
        points->X = (float)X;    // Expected coordinate X
        points->Y = (float)Y;    // Expected coordinate Y
        points->x = (float)p.x;  // Observed coordinate y
        points->y = (float)p.y;  // Observed coordinate y
        points++;
        n++;
      }

      delay(XTC_SAMPLE_PERIOD);
    }

    draw(X, Y, marker_pos, bg);
    while (touched());
    delay(250);
    return n;
  }

public:
  //-------------------------------------------------------------------------------------
  // Example of a function to draw the markers
  //
  // void draw(uint16_t x, uint16_t y, uint8_t marker_pos, uint16_t color) {
  //   int dx = marker_pos & 1 ? 1 : 0;
  //   int dy = marker_pos & 2 ? 1 : 0;
  //   GFX(fillRect(x - MARKER_WIDTH  * dx, y - MARKER_HEIGHT * dy, MARKER_WIDTH,  MARKER_HEIGHT, color));
  //   GFX(fillRect(x - MARKER_HEIGHT * dx, y - MARKER_WIDTH  * dy, MARKER_HEIGHT, MARKER_WIDTH,  color));
  // }
  //-------------------------------------------------------------------------------------
  TouchCalibrationStatus calibrate(
    void (*draw)(uint16_t X, uint16_t Y, uint8_t marker_pos, uint16_t color),
    uint16_t fg_color, uint16_t bg_color,
    Calibration_t *_cal
  ) {
    int z = MARKER_OFFSET;
    int x = max_X - z;
    int y = max_Y - z;

    int n_points = 0;
    TouchCalibrationPoint points[XTC_SAMPLE_POINTS * 4] = { { 0.0f, }, };

    n_points += samplePoints(z, z, &points[n_points], draw, 0, fg_color, bg_color); // top-left
    n_points += samplePoints(x, z, &points[n_points], draw, 1, fg_color, bg_color); // top-right
    n_points += samplePoints(z, y, &points[n_points], draw, 2, fg_color, bg_color); // bottom-left
    n_points += samplePoints(x, y, &points[n_points], draw, 3, fg_color, bg_color); // bottom-right

    //assert(n_points == XTC_SAMPLE_POINTS * 4);

#if USE_4POINTS_AFFINE
    //-------------------------------------------------------------------------------------
    // 4-points affine transformation calibration using the least-squares method
    //-------------------------------------------------------------------------------------
    TouchCalibrationStatus status = touch_calibrate(points, n_points, &cal);

    if (status == TOUCH_CALIBRATION_OK) {
      *_cal = cal;
      calibrated = true;

      Serial.print(F("\n/* Calibration data for 4-points Affine Transformation */\n"));
      Serial.print(F("const Calibration_t cal = {"));
      Serial.print(F( " .a = ")); Serial.print(cal.a, 6);
      Serial.print(F(", .b = ")); Serial.print(cal.b, 6);
      Serial.print(F(", .c = ")); Serial.print(cal.c, 6);
      Serial.print(F(", .d = ")); Serial.print(cal.d, 6);
      Serial.print(F(", .e = ")); Serial.print(cal.e, 6);
      Serial.print(F(", .f = ")); Serial.print(cal.f, 6);
      Serial.print(F(" };\n\n"));
    }

    DBG_EXEC({
      GFX(fillScreen(TFT_BLACK));
      GFX(drawRect(BOX_OFFSET, BOX_OFFSET, max_X+1-BOX_OFFSET*2, max_Y+1-BOX_OFFSET*2, TFT_RED));
      for (int n = 0, i = 0; i < 4; i++) {
        int dx = i & 1 ? 1 : 0;
        int dy = i & 2 ? 1 : 0;
        int dz = BOX_OFFSET - MARKER_OFFSET;
        for (int j = 0; j < XTC_SAMPLE_POINTS; j++, n++) {
          int x = (int)roundf(cal.a * points[n].x + cal.b * points[n].y + cal.c) + (dx ? -dz : dz);
          int y = (int)roundf(cal.d * points[n].x + cal.e * points[n].y + cal.f) + (dy ? -dz : dz);
          printf("n:%2d, x:%4d, y:%4d, X:%3d, Y:%3d --> %3d, %3d\n",
            n, (int)points[n].x, (int)points[n].y, (int)points[n].X, (int)points[n].Y, x, y);
          GFX(fillRect(x - PLOT_SIZE/2, y - PLOT_SIZE/2, PLOT_SIZE, PLOT_SIZE, TFT_WHITE));
        }
      }
      GFX(setTextSize(2)); // Font: 12(w) x 16(h)
      GFX(setCursor((GFX(width()) - 192) / 2, (GFX(height()) - 16) / 2));
      GFX(print(F("Touch the screen"))); // Text: 192(w) x 16(h)
      while (!touched());
    });

    return status;
#else
    //-------------------------------------------------------------------------------------
    // Proportional mapping based on the min/max values ​​of raw data
    //-------------------------------------------------------------------------------------
    #define SAMPLE_OFFSET(p)  (p * XTC_SAMPLE_POINTS)

    float s[4] = { 0.0f, };
    for (int i = 0; i < XTC_SAMPLE_POINTS; i++) {
      s[0] += (points[SAMPLE_OFFSET(0) + i].x + points[SAMPLE_OFFSET(2) + i].x); // min x (top-left    + bottom-left )
      s[1] += (points[SAMPLE_OFFSET(1) + i].x + points[SAMPLE_OFFSET(3) + i].x); // max x (top-right   + bottom-right)
      s[2] += (points[SAMPLE_OFFSET(0) + i].y + points[SAMPLE_OFFSET(1) + i].y); // min y (top-left    + top-right   )
      s[3] += (points[SAMPLE_OFFSET(2) + i].y + points[SAMPLE_OFFSET(3) + i].y); // max y (bottom-left + bottom-right)
    }

    cal.min_x = (uint16_t)roundf(s[0] / (XTC_SAMPLE_POINTS * 2.0f));
    cal.max_x = (uint16_t)roundf(s[1] / (XTC_SAMPLE_POINTS * 2.0f));
    cal.min_y = (uint16_t)roundf(s[2] / (XTC_SAMPLE_POINTS * 2.0f));
    cal.max_y = (uint16_t)roundf(s[3] / (XTC_SAMPLE_POINTS * 2.0f));

    Serial.print(F("\n/* Calibration data for Proportional Mapping */\n"));
    Serial.print(F("const Calibration_t cal = {"));
    Serial.print(F( " .min_x = ")); Serial.print(cal.min_x);
    Serial.print(F(", .max_x = ")); Serial.print(cal.max_x);
    Serial.print(F(", .min_y = ")); Serial.print(cal.min_y);
    Serial.print(F(", .max_y = ")); Serial.print(cal.max_y);
    Serial.print(F(" };\n\n"));

    DBG_EXEC({
      GFX(fillScreen(TFT_BLACK));
      GFX(drawRect(BOX_OFFSET, BOX_OFFSET, max_X+1-BOX_OFFSET*2, max_Y+1-BOX_OFFSET*2, TFT_RED));
      for (int n = 0, i = 0; i < 4; i++) {
        int dx = i & 1 ? 1 : 0;
        int dy = i & 2 ? 1 : 0;
        int dz = BOX_OFFSET - MARKER_OFFSET;
        for (int j = 0; j < XTC_SAMPLE_POINTS; j++, n++) {
          int x = map(points[n].x, cal.min_x, cal.max_x, 0, max_X) + (dx ? -dz : dz);
          int y = map(points[n].y, cal.min_y, cal.max_y, 0, max_Y) + (dy ? -dz : dz);
          printf("n:%2d, x:%4d, y:%4d, X:%3d, Y:%3d --> %3d, %3d\n",
            n, (int)points[n].x, (int)points[n].y, (int)points[n].X, (int)points[n].Y, x, y);
          GFX(fillRect(x - PLOT_SIZE/2, y - PLOT_SIZE/2, PLOT_SIZE, PLOT_SIZE, TFT_WHITE));
        }
      }
      GFX(setTextSize(2)); // Font: 12(w) x 16(h)
      GFX(setCursor((GFX(width()) - 192) / 2, (GFX(height()) - 16) / 2));
      GFX(print(F("Touch the screen"))); // Text: 192(w) x 16(h)
      while (!touched());
    });

    *_cal = cal;
    calibrated = true;
    return TOUCH_CALIBRATION_OK;
#endif
  }
};

#ifdef __cplusplus
}
#endif

#endif /* _XPT2046_TOUCH_CALIBRATE_H_ */