#ifndef _COLORS_H_
#define _COLORS_H_

// https://github.com/moononournation/Arduino_GFX/blob/master/src/Arduino_GFX.h
#ifndef RGB565
#define RGB565(r, g, b) ((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | ((b) >> 3))

#define RGB565_MAGENTA      RGB565(248,   0, 248)
#define RGB565_LIME         RGB565(  0, 252,   0)
#define RGB565_BLACK        RGB565(  0,   0,   0)
#define RGB565_CYAN         RGB565(  0, 252, 248)
#define RGB565_YELLOW       RGB565(248, 252,   0)
#define RGB565_WHITE        RGB565(248, 252, 248)
#define RGB565_RED          RGB565(248,   0,   0)
#define RGB565_BLUE         RGB565(  0,   0, 248)
#define RGB565_MAGENTA      RGB565(248,   0, 248)
#define RGB565_NAVY         RGB565(  0,   0, 128)
#define RGB565_DARKGREEN    RGB565(  0, 100,   0)
#define RGB565_DARKCYAN     RGB565(  0, 140, 136)
#define RGB565_MAROON       RGB565(128,   0,   0)
#define RGB565_PURPLE       RGB565(128,   0, 128)
#define RGB565_OLIVE        RGB565(128, 128,   0)
#define RGB565_DARKGREY     RGB565(168, 168, 168)
#define RGB565_ORANGE       RGB565(248, 164,   0)
#define RGB565_GREENYELLOW  RGB565(176, 252,  48)
#define RGB565_LIGHTPINK    RGB565(248, 184, 192)

#endif // RGB565

#endif // _COLORS_H_