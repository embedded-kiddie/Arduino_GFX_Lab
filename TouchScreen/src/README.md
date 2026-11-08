# Touch Panel Calibration Coefficient Calculation Function (C99)

## Usage example

```c
#include <stdio.h>
#include "touch_calibration.h"

int main(void)
{
    // Four reference coordinates and their corresponding observed coordinates
    const TouchCalibrationPoint points[] = {
        {   0.0,   0.0,  120.0,  180.0 },
        { 319.0,   0.0, 3980.0,  160.0 },
        {   0.0, 239.0,  140.0, 3900.0 },
        { 319.0, 239.0, 3960.0, 3920.0 }
    };
    TouchCalibration calibration;
    TouchCalibrationStatus status;

    status = touch_calibrate(
        points,
        sizeof(points) / sizeof(points[0]),
        &calibration,
    );

    if (status != TOUCH_CALIBRATION_OK) {
        printf("Calibration failed: status = %d\n", (int)status);
        return 1;
    }

    printf("a = % .6f, b = % .6f, c = % .6f\n", calibration.a, calibration.b, calibration.c);
    printf("d = % .6f, e = % .6f, f = % .6f\n", calibration.d, calibration.e, calibration.f);

    return 0;
}
```

Compile with a C99 compiler, for example:

```sh
cc -std=c99 -Wall -Wextra -pedantic main.c touch_calibration.c -lm
```

Some embedded toolchains provide `isfinite()` as part of their C math library; verify availability in the target toolchain.

## Algorithm

1. Calculate the sums used by the common coefficient matrix and the two right-hand sides.
2. Calculate `delta = det(M)` with an explicit 3x3 determinant expansion.
3. Calculate the dimensionless normalized determinant
   `delta / (Sxx * Syy * N)`.
4. If the normalized determinant is below the configured threshold, return
   `TOUCH_CALIBRATION_DEGENERATE`.
5. Calculate the six Cramer's-rule determinants directly and divide each by
   `delta` to obtain `a` through `f`.
