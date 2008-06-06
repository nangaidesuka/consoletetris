#include "console.h"

void main()
{
    console cs;
    cs.movewnd(rect(0,0,20,20));
    //cs.cursorpos(point(10, 1));
    cs.codepage();
    //
    //cs.drawrect(rect(-1,1,20,1), COLOR_BLUE|COLOR_GREEN|COLOR_INTENSITY);
    //cs.textout(L"abc", point(1, 1));        
    //cs.textout(0x0A, point(1, 4), COLOR_RED);
}