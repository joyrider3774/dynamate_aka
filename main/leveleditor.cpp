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

void LevelEditor(void)
{
    static dm_u8 SelectedPiece = NextPiece(DM_P_EMPTY);
    CInput *Input = new CInput(5);
    while (GameState == GSLEVELEDITOR)
    {
        MenuMusic();

        Input->Update();

        VolumeKeys(Input);

        if (Input->Held(BUT_MENU))
        {
            GameState = GSLEVELEDITORMENU;
            PlaySound(SND_SELECT);
        }

        if (Input->Held(BUT_A))
        {
            UserLevel.field[(Cursor->GetY() * TILEROWS) + Cursor->GetX()] = SelectedPiece;
            dm_init_level(UserLevel.field);
        }

        if (Input->Held(BUT_B) || Input->Held(BUT_C))
        {
            UserLevel.field[(Cursor->GetY() * TILEROWS) + Cursor->GetX()] = DM_P_EMPTY;
            dm_init_level(UserLevel.field);
        }

        if (Input->Ready() && Input->Held(BUT_L))
        {
            SelectedPiece = PreviousPiece(SelectedPiece);
            Input->Delay();
        }

        if (Input->Ready() && Input->Held(BUT_R))
        {
            SelectedPiece = NextPiece(SelectedPiece);
            Input->Delay();
        }

        if (Input->Ready())
        {
            const bool Up = Input->Held(BUT_UP), Down = Input->Held(BUT_DOWN);
            const bool Left = Input->Held(BUT_LEFT), Right = Input->Held(BUT_RIGHT);
            int dx = 0, dy = 0;
            if (Up && Left) { dx = -1; dy = -1; }
            else if (Down && Left) { dx = -1; dy = 1; }
            else if (Right && Up) { dx = 1; dy = -1; }
            else if (Right && Down) { dx = 1; dy = 1; }
            else if (Up) dy = -1;
            else if (Down) dy = 1;
            else if (Left) dx = -1;
            else if (Right) dx = 1;
            if (dx || dy)
            {
                Cursor->SetY(Cursor->GetY() + dy);
                Cursor->SetX(Cursor->GetX() + dx);
                Input->Delay();
            }
        }

        BlitSurface(BackgroundLevelEditor, NULL, Screen, 0, 0);
        DrawLevel(Screen);
        Cursor->Draw(Screen);
        DrawPieces(Screen, true, SelectedPiece);
        PresentFrame();
    }
    delete Input;
}
