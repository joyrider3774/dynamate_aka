// The part of SDL, SDL_gfx, SDL_ttf and SFont the game used, on the AKA library's framebuffer.
//
// A surface is 16 bit pixels in the panel's own order (see lcd_color_rgb) and, when it is not
// opaque, an alpha plane next to them. Blitting onto a surface that has an alpha plane leaves
// that plane alone, as SDL 1.2 did when it blitted onto an RGBA surface.
#ifndef GFX_H
#define GFX_H

#include <stdint.h>

struct Surface {
    int16_t w, h;
    uint16_t* px;
    uint8_t* a;     // NULL: opaque
};

struct Rect {
    int16_t x, y, w, h;
};

struct Color {
    uint8_t r, g, b;
};

struct SFont_Font {
    Surface surface;
    const int16_t* CharPos;
    int MaxPos;
};

// the screen, the library's framebuffer
extern Surface Screen;

extern Surface LevelData, Background, BackgroundLevelEditor, Menu, LevelTitle, IMGIntro1, IMGIntro2,
    IMGIntro3, IMGIntro4, Unlocked, Locked, PieceOutsidePlayfield, InfiniteLoop, DeathPiece, OutOfMoves,
    LevelCompleted, AllLevelsCompleted, TitleScreen, CursorImage;
extern SFont_Font FontDark, FontLight;

void GfxInit();
// sends the screen to the panel
void Flip();

uint16_t MapRGB(uint8_t r, uint8_t g, uint8_t b);
Surface CreateSurface(int w, int h, bool alpha);
// copies src into dst, which has to be at least as big (zoomSurface(src, 1, 1, 0) of the game)
void CopySurface(Surface& dst, const Surface& src);
void FillRect(Surface& dst, uint16_t color);
// SDL_BlitSurface; alpha is SDL_SetAlpha's surface alpha on top of src's own
void BlitSurface(const Surface& src, const Rect* srcRect, Surface& dst, int x, int y, uint8_t alpha = 255);
// src zoomed by factor (zoomSurface without smoothing) with its top left corner at x, y
void BlitZoomed(const Surface& src, float factor, Surface& dst, int x, int y);
// the size zoomSurface gives a side of len pixels
int ZoomedSize(int len, float factor);
// boxRGBA: x1,y1 to x2,y2 inclusive, blended with alpha a
void BoxRGBA(Surface& dst, int x1, int y1, int x2, int y2, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

void SFont_Write(Surface& dst, const SFont_Font& Font, int x, int y, const char* text);
int SFont_TextWidth(const SFont_Font& Font, const char* text);
int SFont_TextHeight(const SFont_Font& Font);
void SFont_WriteCenter(Surface& dst, const SFont_Font& Font, int y, const char* text);

// WriteText of Functions.cpp with font.ttf at 12 pixels: lines split on '\n', centred on dst
// when Centered
void WriteText(Surface& dst, const char* Tekst, int X, int Y, int YSpacing, Color ColorIn, bool Centered);

#endif
