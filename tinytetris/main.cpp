#include "console.h"
#include "fpsctl.h"
#include <assert.h>
#include <stdio.h>
#include <vector>
#include <set>

void main()
{try{

    fpsctl thefpsctl;
    using namespace console;
    draw con(size(20, 25));
    
    input i;
    int y = 0;
    int x = 0;
    while (1) { 
        i.update();
        //con.clear(yellow);                
        con.drawcodepage();

        if (i.keydown('a')) 
            --x;
        if (i.keydown('d'))
            ++x;
        if (i.keydown(VK_UP)) 
            --y;
        if (i.keydown(VK_DOWN))
            ++y;

        char buffer[100];
        sprintf_s(buffer, 100, "(%d,%d)", i.mousepos().x, i.mousepos().y);
        
        if ( y>20) y=0;        

        con.flip();
        thefpsctl.wait();
    }    

}catch(const std::exception& e){
    MessageBoxA(0,e.what(),0,0);
}
}
