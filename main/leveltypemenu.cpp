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

void LevelTypeMenu(void)
{
    float size = 0.01;
    int Selected = 0;
    CInput *Input = new CInput(10);
    while (GameState == GSLEVELTYPESELECT)
    {
        MenuMusic();

        if (size >= 1)
            Input->Update();

        VolumeKeys(Input);

        if (Input->Held(BUT_B))
        {
            GameState = GSTITLESCREEN;
            PlaySound(SND_SELECT);
        }

        if (Input->Ready() && Input->Held(BUT_DOWN))
        {
            if (UserLevelCount > 0)
            {
                Selected++;
                if (Selected > 2)
                    Selected = 0;
            }
            else
            {
                if (Selected == 2)
                    Selected = 0;
                else if (Selected == 0)
                    Selected = 2;
            }
            PlaySound(SND_MENU);
            Input->Delay();
        }

        if (Input->Ready() && Input->Held(BUT_UP))
        {
            if (UserLevelCount > 0)
            {
                Selected--;
                if (Selected < 0)
                    Selected = 2;
            }
            else
            {
                if (Selected == 2)
                    Selected = 0;
                else if (Selected == 0)
                    Selected = 2;
            }
            PlaySound(SND_MENU);
            Input->Delay();
        }

        if (Input->Held(BUT_A))
        {
            PlaySound(SND_SELECT);
            switch (Selected)
            {
                case 0: // static
                    StaticLevels = true;
                    //set to last unlocked level!
                    lvl = GetLastUnLocked();
                    dm_init_level(dml_levels[lvl].field);
                    MaxLevels = MAXSTATICLEVELS;
                    GameState = GSGAME;
                    Cursor->SetXY(8, 8);
                    FadeOutMusic(500);
                    break;
                case 1: // user
                    StaticLevels = false;
                    lvl = 0;
                    MaxLevels = UserLevelCount;
                    InitCurrentLevel();
                    GameState = GSLEVELSELECTMENU;
                    FadeOutMusic(500);
                    break;
                case 2: // back
                    GameState = GSTITLESCREEN;
                    break;
            }
        }

        BlitSurface(TitleScreen, NULL, Screen, 0, 0);

        CopySurface(Tmp, Menu);
        SFont_WriteCenter(Tmp, FontDark, 3, "Leveltype");
        MenuItem(27, "Static Levels", Selected == 0);
        MenuItem(50, "User Levels", Selected == 1);
        MenuItem(73, "Back", Selected == 2);
        BlitPopup(Tmp, size, WINDOW_WIDTH / 2, 150);

        PresentFrame();
        if (size < 1)
            size += SizeIncrease;
    }
    delete Input;
}
