#include "console.h"

void main()
{
    console cs;
    cs.cursorvisible(false);
    cs.movewnd(rect(0,0,25,25));   
    cs.codepage(437);    
    cs.drawrect(rect(0,0,3,2), COLOR_RED|COLOR_GREEN);
    cs.setpixcel(point(1, 1), COLOR_GREEN);
    cs.textout(L"abc", point(0,1), COLOR_RED, BK_TRANSPARENT);
}