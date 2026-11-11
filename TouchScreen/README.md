# 4-points calibration using affine transformation matrix

## Overview
- [4-points calibration using affine transformation matrix](wiki/4%E2%80%90points-calibration-using-affine-transformation-matrix)

## Description

The interface for the function `touch_calibrate()` is as follows:

```c
TouchCalibrationStatus touch_calibrate(
    const TouchCalibrationPoint *points,
    size_t count,
    TouchCalibration *out
);
```

The measurement points for the four corners are passed using the following structure:

```c
typedef struct {
float X; // Expected X-coordinate in the display coordinate system
float Y; // Expected Y-coordinate in the display coordinate system
float x; // Measured x-coordinate on the touch panel
float y; // Measured y-coordinate on the touch panel
} TouchCalibrationPoint;
```

The calculation results are stored in `TouchCalibration`, from which the affine transformation coefficients can be obtained.

```c
typedef struct {
float a, b, c, d, e, f;
} TouchCalibration;
```
