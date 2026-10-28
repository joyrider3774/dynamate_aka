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

void IngameMenu()
{
    float size = 0.01;
    int Selected = 0;
    CInput *Input = new CInput(10);
    while (GameState == GSINGAMEMENU)
    {
        GameMusic();

        if (size >= 1)
            Input->Update();

        VolumeKeys(Input);

        if (Input->Held(BUT_MENU) || Input->Held(BUT_B))
        {
            GameState = GSGAME;
            PlaySound(SND_SELECT);
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

        if (Input->Held(BUT_A))
        {
            PlaySound(SND_SELECT);
            switch (Selected)
            {
                case 0: // Resume play
                    GameState = GSGAME;
                    break;
                case 1: // retry
                    InitCurrentLevel();
                    Cursor->SetXY(8, 8);
                    GameState = GSGAME;
                    break;
                case 2: //level select
                    GameState = GSLEVELSELECTMENU;
                    break;
                case 3: //quit
                    FadeOutMusic(500);
                    GameState = GSTITLESCREEN;
                    break;
            }
        }

        BlitSurface(Background, NULL, Screen, 0, 0);
        DrawLevel(Screen);

        CopySurface(Tmp, Menu);
        SFont_WriteCenter(Tmp, FontDark, 4, "Game Menu");
        MenuItem(27, "Resume Play", Selected == 0);
        MenuItem(50, "Retry", Selected == 1);
        MenuItem(73, "Select Level", Selected == 2);
        MenuItem(96, "Quit", Selected == 3);
        BlitPopup(Tmp, size, 120, WINDOW_HEIGHT / 2);

        DrawStats();
        PresentFrame();
        if (size < 1)
            size += SizeIncrease;
    }
    delete Input;
}
