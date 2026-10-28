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

void TitleScreenMenu(void)
{
    float size = 0.01;
    int Selected = 0;
    CInput *Input = new CInput(10);
    while (GameState == GSTITLESCREEN)
    {
        if (size >= 1)
            Input->Update();

        VolumeKeys(Input);

        if (Input->Ready() && Input->Held(BUT_DOWN))
        {
            Selected++;
            if (Selected > 2)
                Selected = 0;
            PlaySound(SND_MENU);
            Input->Delay();
        }

        if (Input->Ready() && Input->Held(BUT_UP))
        {
            Selected--;
            if (Selected < 0)
                Selected = 2;
            PlaySound(SND_MENU);
            Input->Delay();
        }

        if (Input->Held(BUT_A))
        {
            PlaySound(SND_SELECT);
            switch (Selected)
            {
                case 0: // play
                    GameState = GSLEVELTYPESELECT;
                    break;
                case 1: // Level editor
                    memset(&UserLevel, 0, sizeof(UserLevel));
                    dm_init_level(UserLevel.field);
                    SaveEnabled = false;
                    StaticLevels = false;
                    Cursor->SetXY(8, 8);
                    GameState = GSLEVELEDITORMENU;
                    break;
                case 2: //Quit
                    GameState = GSQUIT;
                    break;
            }
        }

        MenuMusic();

        BlitSurface(TitleScreen, NULL, Screen, 0, 0);

        CopySurface(Tmp, Menu);
        SFont_WriteCenter(Tmp, FontDark, 4, "Main Menu");
        MenuItem(27, "Play", Selected == 0);
        MenuItem(50, "Level Editor", Selected == 1);
        MenuItem(73, "Quit", Selected == 2);
        BlitPopup(Tmp, size, WINDOW_WIDTH / 2, 150);

        PresentFrame();
        if (size < 1)
            size += SizeIncrease;
    }
    delete Input;
}
