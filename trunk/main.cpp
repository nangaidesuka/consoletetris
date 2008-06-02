#include "console.h"

void main()
{
    console cs;
    cs.movewnd(rect(0,0,20,20));
    cs.drawrect(rect(-1,1,20,1), CC_BLUE|CC_GREEN|CC_INTENSITY);
    cs.textout(L"abc", point(1, 1));        
    cs.putc(L'a', point(1, 4), CC_BLUE, CC_RED|CC_INTENSITY);    
}