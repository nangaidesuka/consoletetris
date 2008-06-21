#include "tinytetris.h"
#include <assert.h>

namespace tetris {

void block::reborn() 
{
    x = (COL-4) / 2;
    y = 0;
    type = (blocktype)(rand() % NUMBLOCKTYPES);
}

void block::move(movedir md)
{
    switch (md) {
        case moveleft:  x--; break;
        case moveright: x++; break;
        case movedown:  y++; break;
        default: assert(0);
    };
}

void block::transform()
{
    switch (type) {
        case IV:   type  = IH; break;
        case IH:   type  = IV; break;
        case TIAN:  type = TIAN;break;
        case L1:    type = L2;  break;
        case L2:    type = L3;  break;
        case L3:    type = L4;  break;
        case L4:    type = L1;  break;
        case RL1:   type = RL2; break;
        case RL2:   type = RL3; break;
        case RL3:   type = RL4; break;
        case RL4:   type = RL1; break;
        case T1:    type = T2;  break;
        case T2:    type = T3;  break;
        case T3:    type = T4;  break;
        case T4:    type = T1;  break;
        case Z1:    type = Z2;  break;
        case Z2:    type = Z1;  break;
        case RZ1:   type = RZ2; break;
        case RZ2:   type = RZ1; break;
        default: assert(0);
    };
}

void block::draw(console::draw* d) const
{
    for (int i=0; i<4; i++)
        for (int j=0; j<4; j++)
            if (blocklayouts[type][i][j])
                d->drawpixel(console::position(x+j, y+i), blockcolors[type]);
}

void panel::clear()
{
    for (int i=0; i<ROW; i++)
        for (int j=0; j<COL; j++)
            blocktypes[i][j] = -1;
}

void panel::draw(console::draw* d) const
{
    for (int i=0; i<ROW; i++)
        for (int j=0; j<COL; j++)
            if (blocktypes[i][j] != -1)
                d->drawpixel(console::position(j, i), blockcolors[blocktypes[i][j]]);
}

bool panel::collision(const block* blk) const
{    
   for (int i=0; i<4; i++)
        for (int j=0; j<4; j++)
            if (blocklayouts[blk->type][i][j]) {
                /* ºÏ≤‚”Î±ﬂΩÁ≈ˆ◊≤ */
                if (!(0 <= blk->x+j && blk->x+j <= COL-1 && blk->y+i <= ROW-1))
                    return true;
                /* ºÏ≤‚”Î√Ê∞Â≈ˆ◊≤ */
                if (blocktypes[blk->y+i][blk->x+j] != -1)
                    return true;
            }

    return false;
}

bool panel::canmove(const block* blk, movedir md) const
{
    block aftermove = *blk;
    aftermove.move(md);    
    return !collision(&aftermove);
}

bool panel::cantransform(const block* blk) const
{
    block aftertrans = *blk;
    aftertrans.transform();
    return !collision(&aftertrans);
}

int panel::land(block* blk)
{    
    int numlines_erased = 0;

    for (int i=0; i<4; i++)
        for (int j=0; j<4; j++)
            if (blocklayouts[blk->type][i][j])
                blocktypes[blk->y+i][blk->x+j] = blk->type;
    
    for (int i=0; i<4; i++) {
        if (blk->y+i < ROW) {
            if (linefull(blk->y+i)) {
                eraseline(blk->y+i); 
                numlines_erased++;
                MessageBeep(50);
            }
        }
        else 
            break;
    }

    if (!numlines_erased)
        MessageBeep(0);

    return numlines_erased;
}

bool panel::linefull(int row) const
{
    for (int j=0; j<COL; j++)
        if (blocktypes[row][j] == -1)
            return false;
    return true;
}

void panel::eraseline(int row)
{    
    MessageBeep(100);
    for (int i=row-1; i>=0; i--)
        memcpy(blocktypes[i+1], blocktypes[i], sizeof(blocktypes[0]));
    for (int j=0; j<COL; j++)
        blocktypes[0][j] = -1;    
}

int panel::fallatonce(block* blk)
{
    while (canmove(blk, movedown))
        blk->move(movedown);    
    return land(blk);
}

}