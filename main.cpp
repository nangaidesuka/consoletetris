#include "console1.h"
#include "fpsctl.h"
#include <assert.h>
#include <stdio.h>
#include <vector>
#include <set>

void main()
{
    fpsctl thefpsctl;
    using namespace console;
    draw con(size(20, 15));
    input i;
    int y = 0;
    int x = 0;
    while (1) { 
        i.update();
        con.clear(yellow);
        con.drawpixel(position(x, y++), green_i);
        if (i.keydown(VK_LEFT)) 
            --x;
        if (i.keydown(VK_RIGHT))
            ++x;

        char buffer[100];
        char buffer2[100];
        sprintf_s(buffer, 100, "(%d,%d)", i.mousepos().x, i.mousepos().y);
        //con.textout(buffer, point(0, y++), console::green);
        if ( y>30) y=0;
        //printf("%d, %d", i.mousepos().x, i.mousepos().y);
        /*con.textout("", point(0,0), */

        con.flip();
        thefpsctl.wait();
    }    
    
}
