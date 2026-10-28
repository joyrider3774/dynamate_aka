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

void WinMenu()
{
    float size = 0.01;
    int Selected;
    if (lvl == MaxLevels - 1)
        Selected = 3;
    else
        Selected = 1;
    PlaySound(SND_LEVELPASS);
    CInput *Input = new CInput(10);
    while (GameState == GSWINMENU)
    {
        GameMusic();

        if (size >= 1)
            Input->Update();

        VolumeKeys(Input);

        if (Input->Ready() && Input->Held(BUT_DOWN))
        {
            Selected++;
            if (lvl == MaxLevels - 1)
            {
                if (Selected > 3)
                    Selected = 2;
            }
            else
            {
                if (Selected > 3)
                    Selected = 1;
            }
            PlaySound(SND_MENU);
            Input->Delay();
        }

        if (Input->Ready() && Input->Held(BUT_UP))
        {
            Selected--;
            if (lvl == MaxLevels - 1)
            {
                if (Selected < 2)
                    Selected = 3;
            }
            else
            {
                if (Selected < 1)
                    Selected = 3;
            }
            PlaySound(SND_MENU);
            Input->Delay();
        }

        if (Input->Held(BUT_A))
        {
            PlaySound(SND_SELECT);
            switch (Selected)
            {
                case 1: // Next
                    lvl++;
                    InitCurrentLevel();
                    GameState = GSGAME;
                    Cursor->SetXY(8, 8);
                    break;
                case 2: //level select
                    GameState = GSLEVELSELECTMENU;
                    break;
                case 3: //quit
                    GameState = GSTITLESCREEN;
                    break;
            }
        }

        BlitSurface(Background, NULL, Screen, 0, 0);
        DrawLevel(Screen);

        CopySurface(Tmp, Menu);
        SFont_WriteCenter(Tmp, FontDark, 4, "You Win!");
        MenuImage((lvl == MaxLevels - 1) ? AllLevelsCompleted : LevelCompleted, 24);
        if (lvl < MaxLevels - 1)
            MenuItem(59, "Next", Selected == 1);
        MenuItem(82, "SelectLevel", Selected == 2);
        MenuItem(105, "Quit", Selected == 3);
        BlitPopup(Tmp, size, 120, WINDOW_HEIGHT / 2);

        DrawStats();
        PresentFrame();
        if (size < 1)
            size += SizeIncrease;
    }
    delete Input;
}
