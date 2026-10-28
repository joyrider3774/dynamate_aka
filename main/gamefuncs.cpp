#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gamefuncs.h"
#include "commonvars.h"
#include "levels.h"
#include "defines.h"
#include "gfx.h"
#include "platform.h"
#include "savestate.h"
#include "sound.h"

// increase the volume with 8
void IncVolume()
{
    if (Volume < 128)
    {
        Volume += 8;
        SetVolume(Volume);
    }
}

// decrease the volume with 8
void DecVolume()
{
    if (Volume > 0)
    {
        Volume -= 8;
        SetVolume(Volume);
    }
}

// the volume keys every screen had
void VolumeKeys(CInput *Input)
{
    if (Input->Ready() && Input->Held(BUT_VOLMIN))
    {
        DecVolume();
        SaveSettings();
        Input->Delay();
    }

    if (Input->Ready() && Input->Held(BUT_VOLUP))
    {
        IncVolume();
        SaveSettings();
        Input->Delay();
    }
}

// the title music, which the menus keep playing
void MenuMusic()
{
    if (MusicCount > 0)
        if (!MusicPlaying())
        {
            SelectedMusic = 0;
            PlayMusic(SelectedMusic);
        }
}

// the other music, at random, while a level is on screen
void GameMusic()
{
    if (!MusicPlaying())
    {
        if (MusicCount > 1)
        {
            SelectedMusic = 1 + rand() % (MusicCount - 1);
            PlayMusic(SelectedMusic);
        }
        else if (MusicCount == 1)
        {
            SelectedMusic = 0;
            PlayMusic(SelectedMusic);
        }
    }
}

void PresentFrame()
{
    Flip();
    FramerateDelay();
}

// a menu growing from nothing: Tmp zoomed by size, its centre at cx, cy
void BlitPopup(const Surface &Src, float size, int cx, int cy)
{
    float zoom = (size < 1) ? size : 1;
    BlitZoomed(Src, zoom, Screen, cx - (ZoomedSize(Src.w, zoom) / 2), cy - (ZoomedSize(Src.h, zoom) / 2));
}

dm_u8 NextPiece(dm_u8 piece)
{
    switch (piece)
    {
        case DM_P_EMPTY             :return(0x80 | DM_P_VERT);
        case (0x80 | DM_P_VERT)     :return(0x80 | DM_P_HORI);
        case (0x80 | DM_P_HORI)     :return(0x80 | DM_P_TELE1);
        case (0x80 | DM_P_TELE1)    :return(0x80 | DM_P_TELE2);
        case (0x80 | DM_P_TELE2)    :return(0x80 | DM_P_ROTCOL);
        case (0x80 | DM_P_ROTCOL)   :return(0x80 | DM_P_ROT_CW);
        case (0x80 | DM_P_ROT_CW)   :return(0x80 | DM_P_ROT_CCW);
        case (0x80 | DM_P_ROT_CCW)  :return(0x80 | DM_P_GRAY);
        case (0x80 | DM_P_GRAY)     :return(0x80 | DM_P_DIE);
        case (0x80 | DM_P_DIE)      :return(0x80 | DM_P_BLACK);
        case (0x80 | DM_P_BLACK)    :return(0x80 | DM_P_YELLOW);
        case (0x80 | DM_P_YELLOW)   :return(0x80 | DM_P_RED);
        case (0x80 | DM_P_RED)      :return(0x80 | DM_P_GREEN);
        case (0x80 | DM_P_GREEN)    :return(0x80 | DM_P_BLUE);
        case (0x80 | DM_P_BLUE)     :return DM_P_VERT;

        case DM_P_VERT      :return DM_P_HORI;
        case DM_P_HORI      :return DM_P_TELE1;
        case DM_P_TELE1     :return DM_P_TELE2;
        case DM_P_TELE2     :return DM_P_ROTCOL;
        case DM_P_ROTCOL    :return DM_P_ROT_CW;
        case DM_P_ROT_CW    :return DM_P_ROT_CCW;
        case DM_P_ROT_CCW   :return DM_P_GRAY;
        case DM_P_GRAY      :return DM_P_DIE;
        case DM_P_DIE       :return DM_P_BLACK;
        case DM_P_BLACK     :return DM_P_YELLOW;
        case DM_P_YELLOW    :return DM_P_RED;
        case DM_P_RED       :return DM_P_GREEN;
        case DM_P_GREEN     :return DM_P_BLUE;
        case DM_P_BLUE      :return(0x80 | DM_P_VERT);

        default             :return DM_P_EMPTY;
    }
}

dm_u8 PreviousPiece(dm_u8 piece)
{
    dm_u8 prevPiece, newPiece = piece;

    do
    {
        prevPiece = newPiece;
        newPiece = NextPiece(newPiece);
    } while (newPiece != piece);

    return prevPiece;
}

// where a piece is in pieces.png, 0 for none: the solid ones first, then the moving ones
int PieceNr(dm_u8 piece)
{
    int nr;
    switch (L7(piece))
    {
        case DM_P_RED:      nr = 14; break;
        case DM_P_BLUE:     nr = 16; break;
        case DM_P_GREEN:    nr = 15; break;
        case DM_P_YELLOW:   nr = 13; break;
        case DM_P_BLACK:    nr = 12; break;
        case DM_P_HORI:     nr = 4;  break;
        case DM_P_VERT:     nr = 3;  break;
        case DM_P_TELE1:    nr = 5;  break;
        case DM_P_TELE2:    nr = 6;  break;
        case DM_P_ROTCOL:   nr = 7;  break;
        case DM_P_ROT_CW:   nr = 8;  break;
        case DM_P_ROT_CCW:  nr = 9;  break;
        case DM_P_GRAY:     nr = 10; break;
        case DM_P_DIE:      nr = 11; break;
        default:            return 0;
    }
    return SOLID(piece) ? nr : nr + 16;
}

void DrawPiece(Surface &Dst, dm_u8 piece, int x, int y)
{
    int piecenr = PieceNr(piece);
    if (piecenr != 0)
    {
        Rect Src = {0, (int16_t)(piecenr * TILEHEIGHT), TILEWIDTH, TILEHEIGHT};
        BlitSurface(LevelData, &Src, Dst, x, y);
    }
}

void DrawLevel(Surface &Dst)
{
    int x, y;
    for (y = 0; y < TILEROWS; y++)
        for (x = 0; x < TILECOLUMNS; x++)
            DrawPiece(Dst, dm_field()[(y * TILEROWS) + x], x * TILEWIDTH, y * TILEHEIGHT);
}

// every piece of the level editor in the lower right corner, the selected one marked when
// there is one
void DrawPieces(Surface &Dst, bool ShowSelected, dm_u8 SelectedPiece)
{
    dm_u8 TmpPiece = NextPiece(DM_P_EMPTY);
    int x, y;
    for (y = 0; y < 6; y++)
    {
        for (x = 0; x < 5; x++)
        {
            if (PieceNr(TmpPiece) != 0)
            {
                DrawPiece(Dst, TmpPiece, (x * TILEWIDTH) + 243, (y * TILEHEIGHT) + 150);
                if (ShowSelected && (SelectedPiece == TmpPiece))
                    BoxRGBA(Dst, (x * TILEWIDTH) + 243, (y * TILEHEIGHT) + 150, (x * TILEWIDTH) + 243 + TILEWIDTH - 1,
                            (y * TILEHEIGHT) + 150 + TILEHEIGHT - 1, 0, 0, 255, 128);
            }

            TmpPiece = NextPiece(TmpPiece);
            if (((y) * 5) + (x) >= 27)
                break;
        }
        if (((y) * 5) + (x) >= 27)
            break;
    }
}

// the level, record, moves and stones in the right panel
void DrawStats()
{
    char Text[256];
    if (StaticLevels)
        sprintf(Text, "Level: %d\nRecord: %d\nMoves: %d\nStones: %d", lvl + 1, HighScores[lvl], dm_movecount(), dm_stones_left());
    else
        sprintf(Text, "Level: %d\nMoves: %d\nStones: %d", lvl + 1, dm_movecount(), dm_stones_left());
    WriteText(Screen, Text, 242, 182, 0, Brown, false);
}

// the level name at the top of the playfield
void DrawLevelTitle(const char *Text, float size)
{
    CopySurface(TmpTitle, LevelTitle);
    WriteText(TmpTitle, Text, 0, 8, 0, DarkBlue, true);
    float zoom = (size < 1) ? size : 1;
    BlitZoomed(TmpTitle, zoom, Screen, 120 - (ZoomedSize(TmpTitle.w, zoom) / 2), 15);
}

// a menu entry: dark when it is the selected one, light otherwise
void MenuItem(int y, const char *Text, bool Selected)
{
    SFont_WriteCenter(Tmp, Selected ? FontDark : FontLight, y, Text);
}

// an image centred on the menu
void MenuImage(const Surface &Image, int y)
{
    BlitSurface(Image, NULL, Tmp, (Tmp.w / 2) - (Image.w / 2), y);
}

// the current level again: the built in one or the user level from the card
void InitCurrentLevel()
{
    if (StaticLevels)
        dm_init_level(dml_levels[lvl].field);
    else
    {
        LoadLevel(UserLevelsFilenames[lvl]);
        dm_init_level(UserLevel.field);
    }
}

int GetLastUnLocked()
{
    if (!StaticLevels)
        return 0;

    int teller = 0;
    while ((teller < MAXSTATICLEVELS) && (UnlockData[teller] == '1'))
        teller++;
    return teller - 1;
}
