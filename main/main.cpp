// Dynamate for the Gamebuino AKA, ported from Willems Davy's GP2X version (main.cpp there).
//
// The game is the GP2X one: the same screens, menus, level editor and engine, at the same
// 320x240 the AKA's screen has, so every position is the one the GP2X used. What changed is
// what sat under it: SDL is gfx.*, SDL_mixer is sound.*, the GP2X buttons are input.*, and the
// files live in the DYNAMATE folder on the SD card. The USB joystick setup and the TV out
// screen position of the GP2X are gone, there is nothing on the AKA they would be for.
//
// Keys: d-pad or joystick, A select / move a piece, B move a piece and follow it with the
// cursor (back in the menus), MENU the game menu, L1/R1 the piece in the level editor (R1 the
// next music in the game), D + up/down the volume.

#include "states.h"
#include "commonvars.h"
#include "cursor.h"
#include "defines.h"
#include "gfx.h"
#include "platform.h"
#include "savestate.h"
#include "sound.h"
#include <stdlib.h>

static void Quit()
{
    HaltMusic();
    SaveHighScores();
    SaveUnlockData();
    SaveSettings();
    ReturnToLoader();
}

static void Run()
{
    PlatformInit();
    GfxInit();
    MakeDataDirs();
    SoundInit();
    Tmp = CreateSurface(Menu.w, Menu.h, true);
    TmpTitle = CreateSurface(LevelTitle.w, LevelTitle.h, true);
    UserLevelsFilenames = (char (*)[LEVELPATHMAX])AllocLarge((size_t)MAXUSERLEVELS * LEVELPATHMAX);
    Cursor = new CCursor();
    srand(GetTicks());
    LoadSettings();
    LoadUnlockData();
    LoadHighScores();
    SearchForLevels();
    while (GameState != GSQUIT)
    {
        switch (GameState)
        {
            case GSINTRO:
                Intro();
                break;
            case GSINGAMEMENU:
                IngameMenu();
                break;
            case GSGAME:
                Game();
                break;
            case GSDIEDMENU:
                DiedMenu();
                break;
            case GSWINMENU:
                WinMenu();
                break;
            case GSLEVELSELECTMENU:
                LevelSelectMenu();
                break;
            case GSTITLESCREEN:
                TitleScreenMenu();
                break;
            case GSLEVELTYPESELECT:
                LevelTypeMenu();
                break;
            case GSLEVELEDITOR:
                LevelEditor();
                break;
            case GSLEVELEDITORMENU:
                LevelEditorMenu();
                break;
            case GSLEVELEDITORLEVELSELECT:
                LevelEditorLevelSelect();
                break;
            default:
                break;
        }
    }
    Quit();
}

extern "C" void app_main(void)
{
    Run();
}
