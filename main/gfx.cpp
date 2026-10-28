#include "gfx.h"

#include <stdlib.h>
#include <string.h>

#include "gamebuino.h"
#include "assets.h"
#include "platform.h"

// assets.S: assets.bin, made by tools/convert_assets.py
extern const uint8_t dm_assets[];

static gb_graphics gfx;

Surface Screen = {SCREEN_WIDTH, SCREEN_HEIGHT, framebuffer, NULL};

Surface LevelData, Background, BackgroundLevelEditor, Menu, LevelTitle, IMGIntro1, IMGIntro2, IMGIntro3,
    IMGIntro4, Unlocked, Locked, PieceOutsidePlayfield, InfiniteLoop, DeathPiece, OutOfMoves, LevelCompleted,
    AllLevelsCompleted, TitleScreen, CursorImage;
SFont_Font FontDark, FontLight;

static Surface FromAsset(const AssetSurface& Asset)
{
    Surface s;
    s.w = Asset.w;
    s.h = Asset.h;
    s.px = (uint16_t*)(dm_assets + Asset.pixels);
    s.a = (Asset.alpha < 0) ? NULL : (uint8_t*)(dm_assets + Asset.alpha);
    return s;
}

static SFont_Font FontFromAsset(const AssetSFont& Asset)
{
    SFont_Font f;
    f.surface = FromAsset(Asset.surface);
    f.CharPos = Asset.charPos;
    f.MaxPos = Asset.maxPos;
    return f;
}

void GfxInit()
{
    gfx.set_backlight_percent(80);
    LevelData = FromAsset(ASSET_PIECES);
    CursorImage = FromAsset(ASSET_CURSOR);
    Background = FromAsset(ASSET_BACKGROUND);
    BackgroundLevelEditor = FromAsset(ASSET_BACKGROUNDLEVELEDITOR);
    Menu = FromAsset(ASSET_MENU);
    LevelTitle = FromAsset(ASSET_LEVELTITLE);
    Unlocked = FromAsset(ASSET_UNLOCKED);
    Locked = FromAsset(ASSET_LOCKED);
    PieceOutsidePlayfield = FromAsset(ASSET_PIECEOUTSIDEPLAYFIELD);
    InfiniteLoop = FromAsset(ASSET_INFINITELOOP);
    DeathPiece = FromAsset(ASSET_DEATHPIECE);
    OutOfMoves = FromAsset(ASSET_OUTOFMOVES);
    LevelCompleted = FromAsset(ASSET_LEVELCOMPLETED);
    AllLevelsCompleted = FromAsset(ASSET_ALLLEVELSCOMPLETED);
    TitleScreen = FromAsset(ASSET_TITLESCREEN);
    IMGIntro1 = FromAsset(ASSET_INTRO1);
    IMGIntro2 = FromAsset(ASSET_INTRO2);
    IMGIntro3 = FromAsset(ASSET_INTRO3);
    IMGIntro4 = FromAsset(ASSET_INTRO4);
    FontDark = FontFromAsset(ASSET_FONTDARK);
    FontLight = FontFromAsset(ASSET_FONTLIGHT);
    FillRect(Screen, 0);
    Flip();
}

void Flip()
{
    gfx.update();
}

uint16_t MapRGB(uint8_t r, uint8_t g, uint8_t b)
{
    return lcd_color_rgb(r, g, b);
}

Surface CreateSurface(int w, int h, bool alpha)
{
    Surface s;
    s.w = w;
    s.h = h;
    s.px = (uint16_t*)AllocLarge((size_t)w * h * 2);
    s.a = alpha ? (uint8_t*)AllocLarge((size_t)w * h) : NULL;
    return s;
}

void CopySurface(Surface& dst, const Surface& src)
{
    dst.w = src.w;
    dst.h = src.h;
    memcpy(dst.px, src.px, (size_t)src.w * src.h * 2);
    if (dst.a)
    {
        if (src.a)
            memcpy(dst.a, src.a, (size_t)src.w * src.h);
        else
            memset(dst.a, 255, (size_t)src.w * src.h);
    }
}

void FillRect(Surface& dst, uint16_t color)
{
    for (int i = 0; i < dst.w * dst.h; i++)
        dst.px[i] = color;
}

// s over d with alpha 0..255. The panel's order has green in the middle as RGB565 does, so the
// usual trick of spreading the three fields over 32 bits works for it as it stands
static inline uint16_t Blend(uint16_t s, uint16_t d, uint32_t alpha)
{
    uint32_t a = (alpha + 4) >> 3;
    uint32_t sx = (s | ((uint32_t)s << 16)) & 0x07E0F81F;
    uint32_t dx = (d | ((uint32_t)d << 16)) & 0x07E0F81F;
    uint32_t r = (dx + (((sx - dx) * a) >> 5)) & 0x07E0F81F;
    return (uint16_t)(r | (r >> 16));
}

static inline void PutPixel(uint16_t* dp, uint16_t s, uint32_t alpha)
{
    if (alpha >= 255)
        *dp = s;
    else if (alpha)
        *dp = Blend(s, *dp, alpha);
}

void BlitSurface(const Surface& src, const Rect* srcRect, Surface& dst, int x, int y, uint8_t alpha)
{
    int sx = 0, sy = 0, w = src.w, h = src.h;
    if (srcRect)
    {
        sx = srcRect->x;
        sy = srcRect->y;
        w = srcRect->w;
        h = srcRect->h;
    }
    if (sx < 0) { w += sx; x -= sx; sx = 0; }
    if (sy < 0) { h += sy; y -= sy; sy = 0; }
    if (sx + w > src.w) w = src.w - sx;
    if (sy + h > src.h) h = src.h - sy;
    if (x < 0) { w += x; sx -= x; x = 0; }
    if (y < 0) { h += y; sy -= y; y = 0; }
    if (x + w > dst.w) w = dst.w - x;
    if (y + h > dst.h) h = dst.h - y;
    if ((w <= 0) || (h <= 0))
        return;
    for (int row = 0; row < h; row++)
    {
        const uint16_t* sp = src.px + (sy + row) * src.w + sx;
        uint16_t* dp = dst.px + (y + row) * dst.w + x;
        if (!src.a && (alpha == 255))
        {
            memcpy(dp, sp, w * 2);
            continue;
        }
        const uint8_t* ap = src.a ? src.a + (sy + row) * src.w + sx : NULL;
        for (int col = 0; col < w; col++)
            PutPixel(dp + col, sp[col], ap ? (ap[col] * (uint32_t)alpha + 127) / 255 : alpha);
    }
}

int ZoomedSize(int len, float factor)
{
    int size = (int)((double)len * factor);
    return (size < 1) ? 1 : size;
}

void BlitZoomed(const Surface& src, float factor, Surface& dst, int x, int y)
{
    const int dw = ZoomedSize(src.w, factor), dh = ZoomedSize(src.h, factor);
    for (int row = 0; row < dh; row++)
    {
        const int dy = y + row;
        if ((dy < 0) || (dy >= dst.h))
            continue;
        const int sy = row * src.h / dh;
        for (int col = 0; col < dw; col++)
        {
            const int dx = x + col;
            if ((dx < 0) || (dx >= dst.w))
                continue;
            const int si = sy * src.w + col * src.w / dw;
            PutPixel(dst.px + dy * dst.w + dx, src.px[si], src.a ? src.a[si] : 255);
        }
    }
}

void BoxRGBA(Surface& dst, int x1, int y1, int x2, int y2, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    if (x1 > x2) { int t = x1; x1 = x2; x2 = t; }
    if (y1 > y2) { int t = y1; y1 = y2; y2 = t; }
    if (x1 < 0) x1 = 0;
    if (y1 < 0) y1 = 0;
    if (x2 >= dst.w) x2 = dst.w - 1;
    if (y2 >= dst.h) y2 = dst.h - 1;
    const uint16_t c = MapRGB(r, g, b);
    for (int y = y1; y <= y2; y++)
        for (int x = x1; x <= x2; x++)
            PutPixel(dst.px + y * dst.w + x, c, a);
}

// ---------------------------------------------------------------------------- SFont
// SFont: a simple font-library that uses special bitmaps as fonts
// Copyright (C) 2003 Karl Bartel, LGPL (http://www.linux-games.com/sfont/)
// Its drawing functions, on this file's surfaces.

void SFont_Write(Surface& dst, const SFont_Font& Font, int x, int y, const char* text)
{
    if (text == NULL)
        return;
    Rect srcrect;
    // the converter left out the marker row SFont skipped with srcrect.y = 1
    srcrect.y = 0;
    srcrect.h = Font.surface.h;
    for (const char* c = text; *c != '\0' && x <= dst.w; c++)
    {
        int charoffset = ((int)(*c - 33)) * 2 + 1;
        // skip spaces and nonprintable characters
        if (*c == ' ' || charoffset < 0 || charoffset > Font.MaxPos)
        {
            x += Font.CharPos[2] - Font.CharPos[1];
            continue;
        }
        srcrect.w = (Font.CharPos[charoffset + 2] + Font.CharPos[charoffset + 1]) / 2 -
                    (Font.CharPos[charoffset] + Font.CharPos[charoffset - 1]) / 2;
        srcrect.x = (Font.CharPos[charoffset] + Font.CharPos[charoffset - 1]) / 2;
        int dstx = x - (int)((float)(Font.CharPos[charoffset] - Font.CharPos[charoffset - 1]) / 2);
        BlitSurface(Font.surface, &srcrect, dst, dstx, y);
        x += Font.CharPos[charoffset + 1] - Font.CharPos[charoffset];
    }
}

int SFont_TextWidth(const SFont_Font& Font, const char* text)
{
    int width = 0;
    if (text == NULL)
        return 0;
    for (const char* c = text; *c != '\0'; c++)
    {
        int charoffset = ((int)*c - 33) * 2 + 1;
        // skip spaces and nonprintable characters
        if (*c == ' ' || charoffset < 0 || charoffset > Font.MaxPos)
        {
            width += Font.CharPos[2] - Font.CharPos[1];
            continue;
        }
        width += Font.CharPos[charoffset + 1] - Font.CharPos[charoffset];
    }
    return width;
}

int SFont_TextHeight(const SFont_Font& Font)
{
    return Font.surface.h;
}

void SFont_WriteCenter(Surface& dst, const SFont_Font& Font, int y, const char* text)
{
    SFont_Write(dst, Font, dst.w / 2 - SFont_TextWidth(Font, text) / 2, y, text);
}

// ---------------------------------------------------------------------------- font.ttf

static const AssetGlyph* Glyph(char c)
{
    if ((c < 32) || (c > 126))
        c = '?';
    return &TTF_GLYPHS[c - 32];
}

static int LineWidth(const char* Line, int Len)
{
    int w = 0;
    for (int i = 0; i < Len; i++)
        w += Glyph(Line[i])->advance;
    return w;
}

static void DrawLine(Surface& dst, const char* Line, int Len, int x, int y, uint16_t c)
{
    for (int i = 0; i < Len; i++)
    {
        const AssetGlyph* g = Glyph(Line[i]);
        if (g->alpha >= 0)
        {
            const uint8_t* ap = dm_assets + g->alpha;
            for (int row = 0; row < TTF_HEIGHT; row++)
            {
                const int dy = y + row;
                if ((dy < 0) || (dy >= dst.h))
                    continue;
                for (int col = 0; col < g->w; col++)
                {
                    const int dx = x + g->x + col;
                    if ((dx >= 0) && (dx < dst.w))
                        PutPixel(dst.px + dy * dst.w + dx, c, ap[row * g->w + col]);
                }
            }
        }
        x += g->advance;
    }
}

void WriteText(Surface& dst, const char* Tekst, int X, int Y, int YSpacing, Color ColorIn, bool Centered)
{
    const uint16_t c = MapRGB(ColorIn.r, ColorIn.g, ColorIn.b);
    int Line = 0;
    const char* Start = Tekst;
    for (;;)
    {
        const char* End = strchr(Start, '\n');
        int Len = End ? (int)(End - Start) : (int)strlen(Start);
        if (Len > 0)
        {
            int x = Centered ? (dst.w / 2) - (LineWidth(Start, Len) / 2) : X;
            DrawLine(dst, Start, Len, x, Y + Line * TTF_LINESKIP + Line * YSpacing, c);
        }
        if (!End)
            break;
        Start = End + 1;
        Line++;
    }
}
