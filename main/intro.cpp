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

void Intro()
{
    uint8_t alpha = 0;
    uint32_t Time1 = 0;
    int IntroScreenNr = 1;
    const int MaxAlpha = 150, AlphaIncrease = 20;
    const Surface *Images[4] = {&IMGIntro1, &IMGIntro2, &IMGIntro3, &IMGIntro4};
    CInput *Input = new CInput(10);
    Time1 = GetTicks();
    while (GameState == GSINTRO)
    {
        Input->Update();

        if (Input->Held(BUT_A) || Input->Held(BUT_B) || Input->Held(BUT_C) || Input->Held(BUT_MENU))
            GameState = GSTITLESCREEN;

        if (alpha < 255)
        {
            if (alpha + AlphaIncrease > MaxAlpha)
                alpha = 255;
            else
                alpha += AlphaIncrease;
        }

        // each picture fades in over what is on the screen, the previous one
        BlitSurface(*Images[IntroScreenNr - 1], NULL, Screen, 0, 0, alpha);
        PresentFrame();

        if (Time1 + 3700 < GetTicks())
        {
            alpha = 0;
            IntroScreenNr++;
            if (IntroScreenNr > 4)
                GameState = GSTITLESCREEN;
            Time1 = GetTicks();
        }
    }
    delete Input;
}
