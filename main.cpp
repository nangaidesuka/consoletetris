#include "console.h"
#include "fpscontrol.h"
#include <assert.h>
#include <stdio.h>
#include <vector>
#include <set>

void main()
{       
    
    fpscontrol fpsctl;     
    console con;
    input i;
    con.movewnd(rect(0,0,40,25));
    int y = 0;
    while (1) { 
        i.update();
        con.clearscreen(console::yellow);
        con.drawcodepage();
        con.drawcolortab();        
        con.cursorpos(point(0, 0));

        char buffer[100];
        char buffer2[100];        
        sprintf_s(buffer, 100, "(%d,%d)", i.mousepos().x, i.mousepos().y);
        con.textout(buffer, point(0, y++), console::green);
        if ( y>30) y=0;
        //printf("%d, %d", i.mousepos().x, i.mousepos().y);
        /*con.textout("", point(0,0), */

        con.flip();
        fpsctl.wait();        
    }    
    
}
