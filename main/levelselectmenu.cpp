#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "states.h"
#include "commonvars.h"
#include "defines.h"
#include "gamefuncs.h"
#include "gfx.h"
#include "input.h"
#include "sound.h"
#include "levels.h"

void LevelSelectMenu()
{
    float size = 0.01;
    int Selected = 0;
    char Text[300];
    CInput *Input = new CInput(10);
    InitCurrentLevel();
    while (GameState == GSLEVELSELECTMENU)
    {
        GameMusic();

        if (size >= 1)
            Input->Update();

        VolumeKeys(Input);

        if (Input->Held(BUT_B))
        {
            GameState = GSTITLESCREEN;
            PlaySound(SND_SELECT);
            FadeOutMusic(500);
        }

        if (Input->Ready() && Input->Held(BUT_DOWN))
        {
            Selected++;
            if (Selected > 3)
                Selected = 0;
            PlaySound(SND_MENU);
            Input->Delay();
        }

        if (Input->Ready() && Input->Held(BUT_UP))
        {
            Selected--;
            if (Selected < 0)
                Selected = 3;
            PlaySound(SND_MENU);
            Input->Delay();
        }

        if (Input->Ready() && Input->Held(BUT_A))
        {
            switch (Selected)
            {
                case 0: // Next
                    lvl++;
                    if (lvl > MaxLevels - 1)
                        lvl = 0;
                    InitCurrentLevel();
                    PlaySound(SND_SELECT);
                    break;
                case 1: // previous
                    lvl--;
                    if (lvl < 0)
                        lvl = MaxLevels - 1;
                    InitCurrentLevel();
                    PlaySound(SND_SELECT);
                    break;
                case 2:
                    if (StaticLevels && (UnlockData[lvl] != '1'))
                        PlaySound(SND_ERROR);
                    else
                    {
                        GameState = GSGAME;
                        Cursor->SetXY(8, 8);
                        PlaySound(SND_SELECT);
                    }
                    break;
                case 3: //quit
                    GameState = GSTITLESCREEN;
                    PlaySound(SND_SELECT);
                    FadeOutMusic(500);
                    break;
            }
            Input->Delay();
        }

        BlitSurface(Background, NULL, Screen, 0, 0);
        DrawLevel(Screen);

        if (StaticLevels)
            snprintf(Text, sizeof(Text), "Level %d: %s", lvl + 1, dml_levels[lvl].name);
        else
            snprintf(Text, sizeof(Text), "Level %d: %s", lvl + 1, UserLevel.name);
        DrawLevelTitle(Text, size);

        CopySurface(Tmp, Menu);
        SFont_WriteCenter(Tmp, FontDark, 4, "Select Level");
        MenuItem(24, "Next", Selected == 0);
        MenuItem(44, "Previous", Selected == 1);
        MenuItem(64, "Play", Selected == 2);
        MenuItem(84, "Quit", Selected == 3);
        if (StaticLevels)
            MenuImage((UnlockData[lvl] == '1') ? Unlocked : Locked, 108);
        BlitPopup(Tmp, size, 120, WINDOW_HEIGHT / 2);

        DrawStats();
        PresentFrame();
        if (size < 1)
            size += SizeIncrease;
    }
    delete Input;
}
