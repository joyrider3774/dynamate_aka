// CInput of the GP2X version on the AKA's keys.
//
// As there, a key only counts once it went down after the CInput was made (or Reset): the
// original flushed SDL's events first, so a key still held from the previous screen did nothing
// until it was pressed again. Ready() and Delay() are its key repeat, counted in frames.
//
// D held turns up and down into volume up and down (the GP2X had volume keys).
#ifndef CINPUT_H
#define CINPUT_H

#include <stdint.h>

enum Buttons {
    BUT_UP, BUT_RIGHT, BUT_DOWN, BUT_LEFT, BUT_A, BUT_B, BUT_C, BUT_L, BUT_R, BUT_MENU,
    BUT_VOLUP, BUT_VOLMIN, NROFBUTTONS
};

class CInput {
    public:
        CInput(int UpdateCounterDelay);
        void Update();
        void Reset();
        bool Held(int Button) { return (PHeld >> Button) & 1; };
        bool Ready() { return (UpdateCounter == 0); };
        void Delay() { UpdateCounter = PUpdateCounterDelay; };
    private:
        uint32_t ReadButtons();
        uint32_t PHeld, Ignored;
        int UpdateCounter;
        int PUpdateCounterDelay;
};

#endif
