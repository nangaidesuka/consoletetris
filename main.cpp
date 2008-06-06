#include "console.h"

void main()
{
    console cs;
    cs.cursorvisible(false);
    cs.movewnd(rect(0,0,25,25));   
    cs.codepage(437);
    cs.drawcodepage();
}