#include "console.h"
#include "fpsctl.h"

void main()
{    
    console con;
    con.codepage(437);
    con.movewnd(rect(0,0,20,20));
    //con.fullscreen(true);
    input in;
    int i = 0;
    fpsctl fpsctl_(30);
    while (1) {        
        con.clearscreen(console::yellow);
        in.update();
        
        tchar buf[100];
        con.textout(_itoa(in.mousepos().x, buf, 10), point(0,0)); 
        con.textout(_itoa(in.mousepos().y, buf, 10), point(0,1));
        con.drawcodepage();

        fpsctl_.wait();
    }
}