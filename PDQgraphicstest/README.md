# A benchmark based on PDQgraphicstest, extended for Adafruit GFX and Arduino GFX.

This repository is based on [PDQgraphicstest][1].

## Requirements

### Graphic Libraries
- [Arduino_GFX][2]
- [Adafruit-GFX-Library][3]
  - [Adafruit_ILI9341][4]
  - [Adafruit-ST7735-Library][5]

### Target MCU boards
- [ArduinoCore-renesas][6] 1.6.0 or later
- [arduino-esp32][7] 3.3.12 or later

## Sample Results

### ILI9341 / Arduino UNO R4 WiFi

| Benchmark item [msec]   | Adafruit GFX | Arduino GFX 12MHz | vs Adafruit | Arduino GFX 24MHz | vs Adafruit |
| ----------------------- | -----------: | ----------------: | ----------: | ----------------: | ----------: |
| Screen fill             |    1,203,542 |           634,214 |        1.90 |           358,586 |        3.36 |
| Text                    |      253,111 |            69,719 |        3.63 |            52,867 |        4.79 |
| Pixels                  |    2,079,712 |         2,102,500 |        0.99 |         1,871,040 |        1.11 |
| Lines                   |    2,901,661 |         1,947,032 |        1.49 |         1,659,398 |        1.75 |
| Horiz/Vert Lines        |       99,076 |            53,533 |        1.85 |            31,229 |        3.17 |
| Rectangles (filled)     |    2,499,381 |         1,317,816 |        1.90 |           745,556 |        3.35 |
| Rectangles (outline)    |       64,015 |            35,969 |        1.78 |            21,816 |        2.93 |
| Triangles (filled)      |      816,981 |           474,307 |        1.72 |           295,822 |        2.76 |
| Triangles (outline)     |      164,926 |           110,819 |        1.49 |            93,449 |        1.76 |
| Circles (filled)        |      310,746 |           213,795 |        1.45 |           151,732 |        2.05 |
| Circles (outline)       |      318,964 |           183,797 |        1.74 |           157,789 |        2.02 |
| Arcs (filled)           |          N/A |           148,367 |         N/A |           125,878 |         N/A |
| Arcs (outline)          |          N/A |           290,474 |         N/A |           264,986 |         N/A |
| Rounded rects (filled)  |    2,468,541 |         1,342,219 |        1.84 |           769,934 |        3.21 |
| Rounded rects (outline) |      157,607 |            95,652 |        1.65 |            75,322 |        2.09 |

### ILI9341 / Cheap Yellow Display (aka CYD)

| Benchmark item [msec]   | HWSPI 40MHz |  ESP32SPI 40MHz | HWSPI 80MHz | ESP32SPI 80MHz |
| ----------------------- | -----------:| ---------------:| -----------:| --------------:|
| Screen fill             |     187,668 |         170,054 |     111,215 |     **93,581** |
| Text                    |      24,782 |          21,528 |      20,059 |     **16,718** |
| Pixels                  |   1,580,274 |       1,040,225 |   1,527,948 |    **984,635** |
| Lines                   |     718,043 |         545,106 |     653,460 |    **472,797** |
| Horiz/Vert Lines        |      17,248 |          15,018 |      11,076 |      **8,837** |
| Rectangles (filled)     |     390,375 |         353,485 |     231,657 |    **194,737** |
| Rectangles (outline)    |      11,421 |           9,937 |       7,519 |      **6,012** |
| Triangles (filled)      |     145,726 |         127,308 |      96,679 |     **77,949** |
| Triangles (outline)     |      40,448 |          30,890 |      36,498 |     **26,524** |
| Circles (filled)        |      70,957 |          58,415 |      54,304 |     **41,419** |
| Circles (outline)       |      67,194 |          51,228 |      61,179 |     **44,611** |
| Arcs (filled)           |      48,120 |          38,364 |      42,401 |     **32,411** |
| Arcs (outline)          |      96,685 |          76,891 |      90,704 |     **70,485** |
| Rounded rects (filled)  |     400,001 |         360,549 |     241,507 |    **201,819** |
| Rounded rects (outline) |      33,537 |          26,367 |      28,429 |     **20,976** |

[1]: https://github.com/moononournation/Arduino_GFX/tree/master/examples/PDQgraphicstest "Arduino_GFX/examples/PDQgraphicstest at master · moononournation/Arduino_GFX"
[2]: https://github.com/moononournation/Arduino_GFX "moononournation/Arduino_GFX: Arduino GFX developing for various color displays and various data bus interfaces"
[3]: https://github.com/adafruit/Adafruit-GFX-Library "adafruit/Adafruit-GFX-Library: Adafruit GFX graphics core Arduino library, this is the &#x27;core&#x27; class that all our other graphics libraries derive from"
[4]: https://github.com/adafruit/Adafruit_ILI9341 "adafruit/Adafruit_ILI9341: Library for Adafruit ILI9341 displays"
[5]: https://github.com/adafruit/Adafruit-ST7735-Library "adafruit/Adafruit-ST7735-Library: This is a library for the Adafruit 1.8&quot; SPI display http://www.adafruit.com/products/358 and http://www.adafruit.com/products/618"

[6]: https://github.com/arduino/ArduinoCore-renesas "arduino/ArduinoCore-renesas"
[7]: https://github.com/espressif/arduino-esp32 "espressif/arduino-esp32: Arduino core for the ESP32 family of SoCs"
