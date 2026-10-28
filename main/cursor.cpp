#include "cursor.h"

CCursor::CCursor() {
    Px = 0;
    Py = 0;
    MaxAnims = 4;
    NewFrame = 2;
    AnimCounter= 0;
    Anim = 0;
}

void CCursor::Draw(Surface &Dst) {
    Rect Src;
    Src.x = Anim * TILEWIDTH;
    Src.y = 0;
    Src.w = TILEWIDTH;
    Src.h = TILEHEIGHT;
    AnimCounter++;
    if(AnimCounter>NewFrame)
    {
        AnimCounter = 0;
        Anim++;
        if(Anim >= MaxAnims)
            Anim = 0;
    }
    BlitSurface(CursorImage,&Src,Dst,Px * TILEWIDTH,Py * TILEHEIGHT);
}
