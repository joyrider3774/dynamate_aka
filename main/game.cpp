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
#include "platform.h"
#include "savestate.h"

static void MovePiece(int x, int y, dm_dir dir)
{
    dm_init_step(x, y, dir);
    while (dm_step() == 0)
    {
        GameMusic();

        if (dm_cmd() != DM_NOTHING)
        {
            switch (dm_cmd())
            {
                case DM_MOVE:
                    PlaySound(SND_MOVE);
                    break;

                case DM_EXPL:
                    PlaySound(SND_EXPLODE);
                    break;

                case DM_TELE:
                    PlaySound(SND_TELEPORT);
                    break;

                case DM_NOTHING:
                default:
                    break;
            }
            BlitSurface(Background, NULL, Screen, 0, 0);
            DrawLevel(Screen);
            DrawStats();
            Flip();
        }
        Delay(50);
    }
}

void Game(void)
{
    CInput *Input = new CInput(5);

    while (GameState == GSGAME)
    {
        GameMusic();

        Input->Update();

        VolumeKeys(Input);

        if (Input->Held(BUT_R))
        {
            if (MusicCount > 1)
            {
                SelectedMusic++;
                if (SelectedMusic >= MusicCount)
                    SelectedMusic = 1;
                HaltMusic();
                PlayMusic(SelectedMusic);
            }
            Input->Reset();
        }

        if (Input->Held(BUT_MENU))
        {
            GameState = GSINGAMEMENU;
            PlaySound(SND_SELECT);
        }

        const bool MoveKey = Input->Held(BUT_A), FollowKey = Input->Held(BUT_B);
        static const int Dirs[4] = {BUT_UP, BUT_DOWN, BUT_LEFT, BUT_RIGHT};
        static const dm_dir DmDirs[4] = {DM_UP, DM_DOWN, DM_LEFT, DM_RIGHT};

        // A and a direction moves the piece under the cursor
        for (int i = 0; i < 4; i++)
            if (Input->Ready() && MoveKey && Input->Held(Dirs[i]))
            {
                MovePiece(Cursor->GetX(), Cursor->GetY(), DmDirs[i]);
                Input->Delay();
            }

        // B and a direction moves it and the cursor goes along with it
        for (int i = 0; i < 4; i++)
            if (Input->Ready() && FollowKey && Input->Held(Dirs[i]))
            {
                MovePiece(Cursor->GetX(), Cursor->GetY(), DmDirs[i]);
                Cursor->SetXY(dm_srcx(), dm_srcy());
                Input->Delay();
            }

        if (Input->Ready() && !MoveKey && !FollowKey)
        {
            const bool Up = Input->Held(BUT_UP), Down = Input->Held(BUT_DOWN);
            const bool Left = Input->Held(BUT_LEFT), Right = Input->Held(BUT_RIGHT);
            int dx = 0, dy = 0;
            //diagonals first, as the original checked them
            if (Up && Left) { dx = -1; dy = -1; }
            else if (Up && Right) { dx = 1; dy = -1; }
            else if (Down && Left) { dx = -1; dy = 1; }
            else if (Down && Right) { dx = 1; dy = 1; }
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

        if (dm_state() != DM_NORMAL)
            switch (dm_state())
            {
                case DM_FINISHED:
                    if (StaticLevels)
                    {
                        if (HighScores[lvl] == 0)
                            HighScores[lvl] = dm_movecount();
                        else if (HighScores[lvl] > dm_movecount())
                            HighScores[lvl] = dm_movecount();
                        if (lvl + 1 < MAXSTATICLEVELS)
                            UnlockData[lvl + 1] = '1';
                        // the GP2X wrote the records when the game quit, the AKA is just switched off
                        SaveUnlockData();
                        SaveHighScores();
                    }
                    GameState = GSWINMENU;
                    break;
                case DM_OUT_OF_MOVES:
                case DM_HIT_DIE_PIECE:
                case DM_OUT_OF_FIELD:
                case DM_INFINITE_LOOP:
                    GameState = GSDIEDMENU;
                    break;
                default:
                    break;
            }

        BlitSurface(Background, NULL, Screen, 0, 0);
        DrawLevel(Screen);
        Cursor->Draw(Screen);
        DrawStats();
        PresentFrame();
    }
    delete Input;
}
