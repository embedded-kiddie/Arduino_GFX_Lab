//=====================================================================================
// Calculate affine calibration coefficients using least squares and Cramer's rule
// Copyright (c) 2026, embeddedkiddie@gmail.com All Rights Reserved.
// Released under the MIT license (https://opensource.org/licenses/mit-license.php)
//=====================================================================================
#ifndef _TOUCH_CALIBRATION_H_
#define _TOUCH_CALIBRATION_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

//-------------------------------------------------------------------------------------
// Result codes returned by touch_calibrate()
//-------------------------------------------------------------------------------------
typedef enum {
    TOUCH_CALIBRATION_OK = 0,
    TOUCH_CALIBRATION_INVALID_ARGUMENT,
    TOUCH_CALIBRATION_DEGENERATE,
    TOUCH_CALIBRATION_NUMERIC_ERROR,
} TouchCalibrationStatus;

//-------------------------------------------------------------------------------------
// Floating-point type and the precision required to prevent division by zero
//-------------------------------------------------------------------------------------
#if   0
#define FP_TYPE double
#define TOUCH_CALIBRATION_MIN_PRECISION (1.0e-10)
#else
#define FP_TYPE float
#define TOUCH_CALIBRATION_MIN_PRECISION (1.0e-6)
#endif

//-------------------------------------------------------------------------------------
// One calibration sample:
//   (X, Y) = Expected coordinates as reference
//   (x, y) = Observed touch-panel coordinates (typically 0..4095)
//-------------------------------------------------------------------------------------
typedef struct {
    FP_TYPE X;  // Expected coordinate X
    FP_TYPE Y;  // Expected coordinate Y
    FP_TYPE x;  // Observed coordinate x
    FP_TYPE y;  // Observed coordinate y
} TouchCalibrationPoint;

//-------------------------------------------------------------------------------------
// Affine transform:
//   X = a*x + b*y + c
//   Y = d*x + e*y + f
//-------------------------------------------------------------------------------------
typedef struct {
    FP_TYPE a;
    FP_TYPE b;
    FP_TYPE c;
    FP_TYPE d;
    FP_TYPE e;
    FP_TYPE f;
} TouchCalibration;

//-------------------------------------------------------------------------------------
// Calculate affine calibration coefficients using least squares and Cramer's rule
//
// points:
//   Array of N measurements. At least 3 are required, and the observed
//   (x, y) points must not all lie on one straight line.
//
// n_points:
//   Number of measurements.
//
// affine:
//   Receives the six coefficients when the function succeeds.
//   It is left unchanged on failure.
//
// Returns:
//   TOUCH_CALIBRATION_OK on success;
//   TOUCH_CALIBRATION_INVALID_ARGUMENT for invalid or non-finite input points;
//   TOUCH_CALIBRATION_DEGENERATE when impossible to determine a unique affine;
//   TOUCH_CALIBRATION_NUMERIC_ERROR when intermediate arithmetic is not finite;
//-------------------------------------------------------------------------------------
TouchCalibrationStatus touch_calibrate(
  const TouchCalibrationPoint *points,
  size_t n_points,
  TouchCalibration *affine
);

#ifdef __cplusplus
}
#endif

#endif /* _TOUCH_CALIBRATION_H_ */