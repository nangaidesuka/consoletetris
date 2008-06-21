#include <time.h>
#include "console.h"
#include "fpsctl.h"
#include "tinytetris.h"

void main()
{try{

    using namespace console;
    using namespace tetris;

    SetConsoleTitleW(L"Tiny Tetris by Jin Sun");

    srand((UINT)time(0));
    fpsctl thefpsctl;    
    draw con(size(COL, ROW));
    input in;
    block blk;
    panel pnl;
    int   score;
    
    blk.reborn();
    pnl.clear();
    score = 0;

    /* 游戏开始 */
    con.clear();
    con.drawtext(L"==================", position(0, 0), white_i);
    con.drawtext(L"=超迷你俄罗斯方块=", position(0, 1), white_i);
    con.drawtext(L"==================", position(0, 2), white_i);
    con.drawtext(L"  左: A",            position(2, 4), green);
    con.drawtext(L"  右: D",            position(2, 5), green);    
    con.drawtext(L"  下: S",            position(2, 6), green);        
    con.drawtext(L"变形: W",            position(2, 7), green);    
    con.drawtext(L"慢下: U",            position(2, 8), green);
    con.drawtext(L"开始: 回车",         position(2, 10), green_i);
    con.flip();
    while (1) {
        Sleep(100);
        in.update();
        if (in.keydown(VK_RETURN))
            break;        
    }

    /* 游戏主循环 */
    while (1) {          
        con.clear();        
        in.update();
        
        bool gameover = false;
        bool leavegame = false;
        const int frametoupinput = 2;

        if (in.keydown('A')) {
            static int i = 0; 
            if (i++ == frametoupinput) {
                if (pnl.canmove(&blk, moveleft))
                    blk.move(moveleft);
                i = 0;
            }
        }
        if (in.keydown('D')) {
            static int i = 0; 
            if (i++ == frametoupinput) {
                if (pnl.canmove(&blk, moveright))
                    blk.move(moveright);
                i = 0;
            }
        }            
        if (in.keyclick('W')) {
            if (pnl.cantransform(&blk))
                blk.transform();
        }

        bool domovedown = false;
        static int downtimer = 30;
        if (--downtimer == 0) {
            domovedown = true;
            downtimer = 30;
        }        
        if (in.keydown('U')) {
            static int i = 0; 
            if (i++ == frametoupinput) { 
                domovedown = true;
                i = 0;
            }
        }
        
        if (domovedown) {
            if (pnl.canmove(&blk, movedown))
                blk.move(movedown);
            else {
                score += pnl.land(&blk);
                blk.reborn();
                if (pnl.collision(&blk))
                    gameover = true;
            }
        }

        if (in.keyclick('S')) {
            score += pnl.fallatonce(&blk);
            blk.reborn();
            if (pnl.collision(&blk))
                    gameover = true;
        }

        /* 处理游戏失败 */
        if (gameover) {
            con.clear();
            con.drawtext(L"失败! 继续(Y/N)?", draw::homepos(), red_i);
            con.flip();
            while (1) {
                Sleep(100);
                in.update();
                if (in.keydown('Y'))
                    break;
                if (in.keydown('N')) {
                    leavegame = true;
                    break;
                }
            }
            con.clear();
            blk.reborn();
            pnl.clear();
        }

        /* 离开游戏 */
        if (leavegame) 
            break;

        /* 绘图 */
        wchar_t buf[100];
        wsprintf(buf, L"得分: %d", score);
        con.drawtext(buf, draw::homepos(), green_i);
        pnl.draw(&con);
        blk.draw(&con);

        con.flip();
        thefpsctl.wait();
    }    

}catch(const std::exception& e){
    MessageBoxA(0,e.what(),0,0);
}
}