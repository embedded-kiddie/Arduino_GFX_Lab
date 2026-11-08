//=====================================================================================
// Calculate affine calibration coefficients using least squares and Cramer's rule
// Copyright (c) 2026, embeddedkiddie@gmail.com All Rights Reserved.
// Released under the MIT license (https://opensource.org/licenses/mit-license.php)
//=====================================================================================
#include "touch_calibration.h"

#include <float.h>
#include <math.h>

//-------------------------------------------------------------------------------------
// Example: Four reference coordinates and their corresponding observed coordinates
//-------------------------------------------------------------------------------------
/*
const TouchCalibrationPoint points[] = {
    {   0.0,   0.0,  120.0,  180.0 },
    { 319.0,   0.0, 3980.0,  160.0 },
    {   0.0, 239.0,  140.0, 3900.0 },
    { 319.0, 239.0, 3960.0, 3920.0 }
};
//*/

//-------------------------------------------------------------------------------------
// Explicit 3x3 determinant expansion; no matrix solver is used
//-------------------------------------------------------------------------------------
static FP_TYPE determinant3(
  FP_TYPE a11, FP_TYPE a12, FP_TYPE a13,
  FP_TYPE a21, FP_TYPE a22, FP_TYPE a23,
  FP_TYPE a31, FP_TYPE a32, FP_TYPE a33) {
  return a11 * (a22 * a33 - a23 * a32)
       - a12 * (a21 * a33 - a23 * a31)
       + a13 * (a21 * a32 - a22 * a31);
}

static int finite_point(const TouchCalibrationPoint *p) {
  return isfinite(p->x) && isfinite(p->y) &&
         isfinite(p->X) && isfinite(p->Y);
}

TouchCalibrationStatus touch_calibrate(
  const TouchCalibrationPoint *points,
  size_t n_points,
  TouchCalibration *affine
) {
  // Common sums for the coefficient matrix M
  FP_TYPE Sxx = 0.0;
  FP_TYPE Sxy = 0.0;
  FP_TYPE Syy = 0.0;
  FP_TYPE Sx = 0.0;
  FP_TYPE Sy = 0.0;

  // Right-hand-side sums for expected X and Y
  FP_TYPE Px = 0.0;
  FP_TYPE Py = 0.0;
  FP_TYPE P0 = 0.0;
  FP_TYPE Qx = 0.0;
  FP_TYPE Qy = 0.0;
  FP_TYPE Q0 = 0.0;

  FP_TYPE N;
  FP_TYPE delta;
  FP_TYPE normalized_delta;
  FP_TYPE scale;
  FP_TYPE Da, Db, Dc, Dd, De, Df;
  TouchCalibration result;

  if (points == NULL || n_points < 3 || affine == NULL) {
    return TOUCH_CALIBRATION_INVALID_ARGUMENT;
  }

  N = (FP_TYPE)n_points;

  for (int i = 0; i < n_points; ++i) {
    if (!finite_point(&points[i])) {
      return TOUCH_CALIBRATION_INVALID_ARGUMENT;
    }

    FP_TYPE X, Y, x, y;
    X = points[i].X;
    Y = points[i].Y;
    x = points[i].x;
    y = points[i].y;

    Sxx += x * x;
    Sxy += x * y;
    Syy += y * y;
    Sx  += x;
    Sy  += y;

    Px += x * X;
    Py += y * X;
    P0 += X;

    Qx += x * Y;
    Qy += y * Y;
    Q0 += Y;
  }

  if (!isfinite(Sxx) || !isfinite(Sxy) || !isfinite(Syy) ||
      !isfinite(Sx ) || !isfinite(Sy ) || !isfinite(Px ) ||
      !isfinite(Py ) || !isfinite(P0 ) || !isfinite(Qx ) ||
      !isfinite(Qy ) || !isfinite(Q0 )) {
    return TOUCH_CALIBRATION_NUMERIC_ERROR;
  }

  //-------------------------------------------------------------------------------------
  // M = [ Sxx Sxy Sx ]
  //     [ Sxy Syy Sy ]
  //     [ Sx  Sy  N  ]
  //
  // Normalize the determinant by the product of diagonal terms.
  // For this Gram matrix, det(M)/(Sxx*Syy*N) is dimensionless and
  // helps avoid a fixed absolute threshold that depends on coordinate
  // scale and number of samples.
  //-------------------------------------------------------------------------------------
  if (Sxx <= 0.0 || Syy <= 0.0 || N <= 0.0) {
    return TOUCH_CALIBRATION_DEGENERATE;
  }

  delta = determinant3(
    Sxx, Sxy, Sx,
    Sxy, Syy, Sy,
    Sx,  Sy,  N
  );

  scale = Sxx * Syy * N;
  if (!isfinite(delta) || !isfinite(scale) || scale <= 0.0) {
    return TOUCH_CALIBRATION_NUMERIC_ERROR;
  }

  normalized_delta = delta / scale;
  if (!isfinite(normalized_delta)) {
    return TOUCH_CALIBRATION_NUMERIC_ERROR;
  }

  //-------------------------------------------------------------------------------------
  // M is a Gram matrix and its exact determinant is nonnegative.
  // A small negative value can arise from FP_TYPEing-point round-off;
  // treat it as degenerate along with values below the threshold.
  //-------------------------------------------------------------------------------------
  if (normalized_delta < TOUCH_CALIBRATION_MIN_PRECISION) {
    return TOUCH_CALIBRATION_DEGENERATE;
  }

  // Cramer's-rule determinants for a, b, c
  Da = determinant3(
    Px, Sxy, Sx,
    Py, Syy, Sy,
    P0, Sy,  N
  );

  Db = determinant3(
    Sxx, Px, Sx,
    Sxy, Py, Sy,
    Sx,  P0, N
  );

  Dc = determinant3(
    Sxx, Sxy, Px,
    Sxy, Syy, Py,
    Sx,  Sy,  P0
  );

  // Cramer's-rule determinants for d, e, f
  Dd = determinant3(
    Qx, Sxy, Sx,
    Qy, Syy, Sy,
    Q0, Sy,  N
  );

  De = determinant3(
    Sxx, Qx, Sx,
    Sxy, Qy, Sy,
    Sx,  Q0, N
  );

  Df = determinant3(
    Sxx, Sxy, Qx,
    Sxy, Syy, Qy,
    Sx,  Sy,  Q0
  );

  if (!isfinite(Da) || !isfinite(Db) || !isfinite(Dc) ||
      !isfinite(Dd) || !isfinite(De) || !isfinite(Df)) {
    return TOUCH_CALIBRATION_NUMERIC_ERROR;
  }

  result.a = Da / delta;
  result.b = Db / delta;
  result.c = Dc / delta;
  result.d = Dd / delta;
  result.e = De / delta;
  result.f = Df / delta;

  if (!isfinite(result.a) || !isfinite(result.b) || !isfinite(result.c) ||
      !isfinite(result.d) || !isfinite(result.e) || !isfinite(result.f)) {
    return TOUCH_CALIBRATION_NUMERIC_ERROR;
  }

  *affine = result;
  return TOUCH_CALIBRATION_OK;
}