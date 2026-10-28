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
#include "savestate.h"

void LevelEditorLevelSelect(void)
{
    float size = 0.01;
    int Selected = 0;
    char Text[300];
    SaveEnabled = false;
    CInput *Input = new CInput(10);
    while (GameState == GSLEVELEDITORLEVELSELECT)
    {
        MenuMusic();

        if (size >= 1)
            Input->Update();

        VolumeKeys(Input);

        if (Input->Held(BUT_B))
        {
            GameState = GSLEVELEDITORMENU;
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

        if (Input->Ready() && Input->Held(BUT_A))
        {
            PlaySound(SND_SELECT);
            switch (Selected)
            {
                case 0: // Next
                    lvl++;
                    if (lvl > UserLevelCount - 1)
                        lvl = 0;
                    LoadLevel(UserLevelsFilenames[lvl]);
                    dm_init_level(UserLevel.field);
                    break;
                case 1: // previous
                    lvl--;
                    if (lvl < 0)
                        lvl = UserLevelCount - 1;
                    LoadLevel(UserLevelsFilenames[lvl]);
                    dm_init_level(UserLevel.field);
                    break;
                case 2: //edit
                    SaveEnabled = true;
                    Cursor->SetXY(8, 8);
                    GameState = GSLEVELEDITOR;
                    break;
                case 3: //back
                    memset(UserLevel.field, 0, 256);
                    memset(UserLevel.name, 0, 256);
                    dm_init_level(UserLevel.field);
                    GameState = GSLEVELEDITORMENU;
                    break;
            }
            Input->Delay();
        }

        BlitSurface(BackgroundLevelEditor, NULL, Screen, 0, 0);
        DrawLevel(Screen);

        snprintf(Text, sizeof(Text), "Level %d: %s", lvl + 1, UserLevel.name);
        DrawLevelTitle(Text, size);

        CopySurface(Tmp, Menu);
        SFont_WriteCenter(Tmp, FontDark, 4, "Select Level");
        MenuItem(24, "Next", Selected == 0);
        MenuItem(44, "Previous", Selected == 1);
        MenuItem(64, "Edit", Selected == 2);
        MenuItem(84, "Back", Selected == 3);
        BlitPopup(Tmp, size, 120, WINDOW_HEIGHT / 2);

        DrawPieces(Screen, false, 0);
        PresentFrame();
        if (size < 1)
            size += SizeIncrease;
    }
    delete Input;
}
