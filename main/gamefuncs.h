// What the states have in common: the volume keys, the music, drawing the level and its
// pieces, the menus
#ifndef GAMEFUNCS_H
#define GAMEFUNCS_H

#include "dynamate_base.h"
#include "gfx.h"
#include "input.h"

void IncVolume();
void DecVolume();
// the volume keys every screen had
void VolumeKeys(CInput *Input);
// the title music, which the menus keep playing
void MenuMusic();
// the other music, at random, while a level is on screen
void GameMusic();
void PresentFrame();
// a menu growing from nothing: Src zoomed by size, its centre at cx, cy
void BlitPopup(const Surface &Src, float size, int cx, int cy);
dm_u8 NextPiece(dm_u8 piece);
dm_u8 PreviousPiece(dm_u8 piece);
int PieceNr(dm_u8 piece);
void DrawPiece(Surface &Dst, dm_u8 piece, int x, int y);
void DrawLevel(Surface &Dst);
void DrawPieces(Surface &Dst, bool ShowSelected, dm_u8 SelectedPiece);
void DrawStats();
void DrawLevelTitle(const char *Text, float size);
void MenuItem(int y, const char *Text, bool Selected);
void MenuImage(const Surface &Image, int y);
void InitCurrentLevel();
int GetLastUnLocked();

#endif
