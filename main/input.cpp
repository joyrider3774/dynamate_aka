#include "input.h"

#include "gb_common.h"
#include "platform.h"

CInput::CInput(int UpdateCounterDelay)
{
    PUpdateCounterDelay = UpdateCounterDelay;
    UpdateCounter = 0;
    Reset();
}

uint32_t CInput::ReadButtons()
{
    const uint16_t Keys = PollKeys();
    uint32_t Result = 0;
    if (Keys & GB_KEY_D)
    {
        if (Keys & GB_KEY_UP) Result |= 1 << BUT_VOLUP;
        if (Keys & GB_KEY_DOWN) Result |= 1 << BUT_VOLMIN;
    }
    else
    {
        if (Keys & GB_KEY_UP) Result |= 1 << BUT_UP;
        if (Keys & GB_KEY_DOWN) Result |= 1 << BUT_DOWN;
    }
    if (Keys & GB_KEY_LEFT) Result |= 1 << BUT_LEFT;
    if (Keys & GB_KEY_RIGHT) Result |= 1 << BUT_RIGHT;
    if (Keys & GB_KEY_A) Result |= 1 << BUT_A;
    if (Keys & GB_KEY_B) Result |= 1 << BUT_B;
    if (Keys & GB_KEY_C) Result |= 1 << BUT_C;
    if (Keys & GB_KEY_L1) Result |= 1 << BUT_L;
    if (Keys & GB_KEY_R1) Result |= 1 << BUT_R;
    if (Keys & GB_KEY_MENU) Result |= 1 << BUT_MENU;
    return Result;
}

void CInput::Update()
{
    if (UpdateCounter > 0)
        UpdateCounter--;
    const uint32_t Now = ReadButtons();
    // a key that was held when the input started counts again once it was let go
    Ignored &= Now;
    PHeld = Now & ~Ignored;
}

void CInput::Reset()
{
    PHeld = 0;
    Ignored = ReadButtons();
}
