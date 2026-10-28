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

void DiedMenu()
{
    float size = 0.01;
    int Selected = 1;
    PlaySound(SND_LEVELFAIL);
    CInput *Input = new CInput(10);
    while (GameState == GSDIEDMENU)
    {
        GameMusic();

        if (size >= 1)
            Input->Update();

        VolumeKeys(Input);

        if (Input->Ready() && Input->Held(BUT_DOWN))
        {
            Selected++;
            if (Selected > 3)
                Selected = 1;
            PlaySound(SND_MENU);
            Input->Delay();
        }

        if (Input->Ready() && Input->Held(BUT_UP))
        {
            Selected--;
            if (Selected < 1)
                Selected = 3;
            PlaySound(SND_MENU);
            Input->Delay();
        }

        if (Input->Held(BUT_A))
        {
            PlaySound(SND_SELECT);
            switch (Selected)
            {
                case 1: // retry
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
        SFont_WriteCenter(Tmp, FontDark, 4, "You Died!");
        switch (dm_state())
        {
            case DM_OUT_OF_MOVES:
                MenuImage(OutOfMoves, 24);
                break;
            case DM_HIT_DIE_PIECE:
                MenuImage(DeathPiece, 24);
                break;
            case DM_OUT_OF_FIELD:
                MenuImage(PieceOutsidePlayfield, 24);
                break;
            case DM_INFINITE_LOOP:
                MenuImage(InfiniteLoop, 24);
                break;
            default:
                break;
        }
        MenuItem(59, "Retry", Selected == 1);
        MenuItem(82, "Select Level", Selected == 2);
        MenuItem(105, "Quit", Selected == 3);
        BlitPopup(Tmp, size, 120, WINDOW_HEIGHT / 2);

        DrawStats();
        PresentFrame();
        if (size < 1)
            size += SizeIncrease;
    }
    delete Input;
}
