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
#include "dialogs.h"
#include "platform.h"
#include "savestate.h"

static int CheckForLevelErrors()
{
    int x, y, NumTeleport1 = 0, NumTeleport2 = 0, NumColors = 0, NumObjects = 0, tmp;
    for (y = 0; y < TILEROWS; y++)
    {
        for (x = 0; x < TILECOLUMNS; x++)
        {
            tmp = dm_field()[(y * TILEROWS) + x];
            switch (L7(tmp))
            {
                case DM_P_DIE:
                case DM_P_GRAY:
                case DM_P_ROT_CCW:
                case DM_P_ROT_CW:
                case DM_P_ROTCOL:
                case DM_P_VERT:
                case DM_P_HORI:
                    NumObjects++;
                    break;

                case DM_P_BLACK:
                case DM_P_YELLOW:
                case DM_P_GREEN:
                case DM_P_BLUE:
                case DM_P_RED:
                    NumObjects++;
                    if (!SOLID(tmp))
                        NumColors++;
                    break;
                case DM_P_TELE1:
                    NumTeleport1++;
                    NumObjects++;
                    break;

                case DM_P_TELE2:
                    NumTeleport2++;
                    NumObjects++;
                    break;
                default:
                    break;
            }
        }
    }

    if (NumTeleport1 > 0)
        if (NumTeleport1 != 2)
            return 0; //Teleport 1 invalid number
    if (NumTeleport2 > 0)
        if (NumTeleport2 != 2)
            return 1; //teleport 2 invalid number
    if (NumObjects == 0)
        return 2; // no pieces in the field
    if (NumColors < 2)
        return 3; // there must be at least 2 colored pieces
    return -1; //all good
}

void LevelEditorMenu(void)
{
    char Text[256], Text1[512];
    int LevelError = -1;
    float size = 0.01;
    int Selected = 0;
    CInput *Input = new CInput(10);
    while (GameState == GSLEVELEDITORMENU)
    {
        MenuMusic();

        if (size >= 1)
            Input->Update();

        VolumeKeys(Input);

        if (Input->Held(BUT_MENU) || Input->Held(BUT_B))
        {
            SaveEnabled = true;
            GameState = GSLEVELEDITOR;
            PlaySound(SND_SELECT);
        }

        if (Input->Ready() && Input->Held(BUT_DOWN))
        {
            Selected++;
            if (Selected > 4)
                Selected = 0;
            if (!SaveEnabled)
                if (Selected == 2)
                    Selected = 4;
            if (UserLevelCount == 0)
            {
                if (Selected == 1)
                {
                    if (SaveEnabled)
                        Selected = 2;
                    else
                        Selected = 4;
                }
            }
            PlaySound(SND_MENU);
            Input->Delay();
        }

        if (Input->Ready() && Input->Held(BUT_UP))
        {
            Selected--;
            if (Selected < 0)
                Selected = 4;
            if (!SaveEnabled)
                if (Selected == 3)
                    Selected = 1;

            if (UserLevelCount == 0)
            {
                if (Selected == 1)
                    Selected = 0;
            }
            PlaySound(SND_MENU);
            Input->Delay();
        }

        if (Input->Held(BUT_A))
        {
            PlaySound(SND_SELECT);
            switch (Selected)
            {
                case 0: // new
                    memset(&UserLevel, 0, sizeof(UserLevel));
                    dm_init_level(UserLevel.field);
                    SaveEnabled = true;
                    GameState = GSLEVELEDITOR;
                    Cursor->SetXY(8, 8);
                    break;
                case 1: //level select
                    lvl = 0;
                    LoadLevel(UserLevelsFilenames[lvl]);
                    dm_init_level(UserLevel.field);
                    GameState = GSLEVELEDITORLEVELSELECT;
                    break;
                case 2:
                    LevelError = CheckForLevelErrors();
                    if (LevelError >= 0)
                    {
                        switch (LevelError)
                        {
                            case 0:
                            case 1:
                                PrintMsg("When using the teleporters there\nshould be two teleporter pieces\nof each kind!\n\nPlease fix this error before saving!", "Level Error");
                                break;
                            case 2:
                                PrintMsg("There are no pieces in this level!\n\nPlease fix this error before saving!", "Level Error");
                                break;
                            case 3:
                                PrintMsg("There should be at least 2 colored\npieces in a level !\n\nPlease fix this error before saving!", "Level Error");
                                break;
                        }
                    }
                    else
                    {
                        snprintf(Text, sizeof(Text), "%s", UserLevel.name);
                        if (GetName(Text))
                        {
                            memset(UserLevel.name, 0, 256);
                            snprintf(UserLevel.name, sizeof(UserLevel.name), "%s", Text);
                            snprintf(Text1, sizeof(Text1), DATA_DIR "/userlevels/%s.lev", Text);
                            SaveLevel(Text1);
                            UpdateUserLevelFilenames(Text1);
                        }
                    }
                    Input->Reset();
                    break;
                case 3:
                    GameState = GSLEVELEDITOR;
                    break;
                case 4: //Quit
                    GameState = GSTITLESCREEN;
                    break;
            }
        }

        BlitSurface(BackgroundLevelEditor, NULL, Screen, 0, 0);
        DrawLevel(Screen);

        CopySurface(Tmp, Menu);
        SFont_WriteCenter(Tmp, FontDark, 4, "Level Editor");
        MenuItem(27, "New", Selected == 0);
        MenuItem(46, "Select Level", Selected == 1);
        MenuItem(65, "Save", Selected == 2);
        MenuItem(83, "Back", Selected == 3);
        MenuItem(101, "Quit", Selected == 4);
        BlitPopup(Tmp, size, 120, WINDOW_HEIGHT / 2);

        DrawPieces(Screen, false, 0);
        PresentFrame();
        if (size < 1)
            size += SizeIncrease;
    }
    delete Input;
}
