#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dialogs.h"
#include "commonvars.h"
#include "defines.h"
#include "gamefuncs.h"
#include "gfx.h"
#include "input.h"
#include "sound.h"

bool GetName(char *Text)
{
    bool end = false, canceled = true;
    float size = 0.01;
    char Text1[512];
    int x = 0, y = 0, x1, y1;
    CInput *Input = new CInput(3);
    while (!end)
    {
        MenuMusic();

        if (size >= 1)
            Input->Update();

        VolumeKeys(Input);

        if (Input->Held(BUT_C))
            end = true;

        if (Input->Held(BUT_MENU))
        {
            end = true;
            canceled = false;
        }

        if (Input->Ready() && Input->Held(BUT_A))
        {
            if (((y * 11) + x) < 65)
            {
                if (strlen(Text) < 20)
                    sprintf(Text + strlen(Text), "%c", NameSymbols[(y * 11) + x]);
            }
            else if (((y * 11) + x) == 65)
            {
                if (strlen(Text) > 0)
                    Text[strlen(Text) - 1] = '\0';
            }
            else if ((y == 6) && (x == 0))
            {
                if ((strlen(Text) > 0) && Text[0] != ' ')
                {
                    PlaySound(SND_SELECT);
                    end = true;
                    canceled = false;
                }
            }
            else
            {
                PlaySound(SND_SELECT);
                end = true;
            }
            Input->Delay();
        }

        if (Input->Ready() && Input->Held(BUT_B))
        {
            if (strlen(Text) > 0)
            {
                Text[strlen(Text) - 1] = '\0';
                PlaySound(SND_SELECT);
            }
            Input->Delay();
        }

        if (Input->Ready() && Input->Held(BUT_RIGHT))
        {
            x++;
            if ((y >= 0) && (y <= 5))
            {
                if (x > 10)
                    x = 0;
            }
            else
            {
                if (x > 1)
                    x = 0;
            }
            Input->Delay();
        }

        if (Input->Ready() && Input->Held(BUT_LEFT))
        {
            x--;
            if ((y >= 0) && (y <= 5))
            {
                if (x < 0)
                    x = 10;
            }
            else if (x < 0)
                x = 1;
            Input->Delay();
        }

        if (Input->Ready() && Input->Held(BUT_UP))
        {
            if (y == 6)
                x = 0;
            y--;
            if (y < 0)
                y = 6;
            if (y == 6)
                x = 0;
            Input->Delay();
        }

        if (Input->Ready() && Input->Held(BUT_DOWN))
        {
            if (y == 5)
                x = 0;
            y++;
            if (y > 6)
            {
                y = 0;
                x = 0;
            }
            Input->Delay();
        }

        BlitSurface(BackgroundLevelEditor, NULL, Screen, 0, 0);

        snprintf(Text1, sizeof(Text1), "Level name: %s", Text);
        DrawLevelTitle(Text1, size);

        CopySurface(Tmp, Menu);
        for (y1 = 0; y1 < 6; y1++)
        {
            for (x1 = 0; x1 < 11; x1++)
            {
                if ((y1 * 11) + x1 >= 65)
                    break;
                sprintf(Text1, "%c", NameSymbols[(y1 * 11) + x1]);
                WriteText(Tmp, Text1, 7 + (x1 * 14), 20 + (y1 * 14), 0, DarkBlue, false);
            }
            if ((y1 * 11) + x1 >= 65)
                break;
        }
        WriteText(Tmp, "<", 7 + (10 * 14), 20 + (5 * 14), 0, DarkBlue, false);
        WriteText(Tmp, "OK", 50, 25 + (6 * 14), 0, DarkBlue, false);
        WriteText(Tmp, "CANCEL", 75, 25 + (6 * 14), 0, DarkBlue, false);

        if (y < 6)
            BoxRGBA(Tmp, 3 + (x * 14), 20 + (y * 14), 3 + ((x + 1) * 14), 20 + ((y + 1) * 14), 255, 255, 255, 128);
        else if (x == 0)
            BoxRGBA(Tmp, 45, 25 + (y * 14), 70, 25 + ((y + 1) * 14), 255, 255, 255, 128);
        else
            BoxRGBA(Tmp, 70, 25 + (y * 14), 120, 25 + ((y + 1) * 14), 255, 255, 255, 128);

        BlitPopup(Tmp, size, 120, WINDOW_HEIGHT / 2);

        DrawPieces(Screen, false, 0);
        PresentFrame();
        if (size < 1)
            size += SizeIncrease;
    }
    delete Input;
    return !canceled;
}

void PrintMsg(const char *Text, const char *Title)
{
    bool end = false;
    float size = 0.01;
    CInput *Input = new CInput(10);
    while (!end)
    {
        MenuMusic();

        if (size >= 1)
            Input->Update();

        VolumeKeys(Input);

        if (Input->Held(BUT_A) || Input->Held(BUT_B) || Input->Held(BUT_C) || Input->Held(BUT_MENU) ||
            Input->Held(BUT_L) || Input->Held(BUT_R))
        {
            PlaySound(SND_SELECT);
            end = true;
        }

        BlitSurface(BackgroundLevelEditor, NULL, Screen, 0, 0);
        DrawLevel(Screen);

        CopySurface(Tmp, Menu);
        WriteText(Tmp, Text, 0, 30, 0, DarkBlue, true);
        SFont_WriteCenter(Tmp, FontDark, 4, Title);
        BlitPopup(Tmp, size, 120, WINDOW_HEIGHT / 2);

        DrawPieces(Screen, false, 0);
        PresentFrame();
        if (size < 1)
            size += SizeIncrease;
    }
    delete Input;
}
