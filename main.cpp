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
        con.drawrect(rectangle(x, y, 5+x, 5+y), blue);
        con.drawpixel(position(0, 1), green_i);
        con.drawlinev(point(1,1), 7, magenta);
        if (i.keydown('a')) 
            --x;
        if (i.keydown('d'))
            ++x;
        if (i.keydown(VK_UP)) 
            --y;
        if (i.keydown(VK_DOWN))
            ++y;
        con.drawtext(L"d·Àfdafdsafµ¯", position(0,0), red);        
        char buffer[100];
        sprintf_s(buffer, 100, "(%d,%d)", i.mousepos().x, i.mousepos().y);
        //con.textout(buffer, point(0, y++), console::green);
        if ( y>20) y=0;
        //printf("%d, %d", i.mousepos().x, i.mousepos().y);
        /*con.textout("", point(0,0), */

        con.flip();
        thefpsctl.wait();
    }    
    
}
