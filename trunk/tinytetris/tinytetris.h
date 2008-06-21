#pragma once

#include "console.h"

namespace tetris {

/* 面板行列数 */
const int ROW = 14;
const int COL = 18;

/* 方块可移动方向 */
enum movedir {
    moveleft, moveright, movedown
};

/* 方块类型 */
enum blocktype {
    IV, IH,
    TIAN,
    L1, L2, L3, L4, /* 顺时针形变 */
    RL1, RL2, RL3, RL4,
    T1, T2, T3, T4, 
    Z1, Z2, 
    RZ1, RZ2, 
    NUMBLOCKTYPES
};

/* 方块各类型颜色 */
const console::color blockcolors[NUMBLOCKTYPES] = {    
    console::red_i, console::red_i,
    console::white_i,
    console::green_i, console::green_i, console::green_i, console::green_i,
    console::blue_i, console::blue_i, console::blue_i, console::blue_i,
    console::yellow_i, console::yellow_i, console::yellow_i, console::yellow_i,
    console::cyan_i, console::cyan_i,
    console::magenta_i, console::magenta_i,
};

/* 方块各类型布局 */
const int blocklayouts[NUMBLOCKTYPES][4][4] = {
    { { 1, 0, 0, 0 }, /* IV */
      { 1, 0, 0, 0 },
      { 1, 0, 0, 0 },
      { 1, 0, 0, 0 } },
    { { 1, 1, 1, 1 }, /* IH */
      { 0, 0, 0, 0 },
      { 0, 0, 0, 0 },
      { 0, 0, 0, 0 } },
    { { 1, 1, 0, 0 }, /* TIAN */
      { 1, 1, 0, 0 },
      { 0, 0, 0, 0 },
      { 0, 0, 0, 0 } },
    { { 1, 0, 0, 0 }, /* L1 */
      { 1, 0, 0, 0 },
      { 1, 1, 0, 0 },
      { 0, 0, 0, 0 } },
    { { 1, 1, 1, 0 }, /* L2 */
      { 1, 0, 0, 0 },
      { 0, 0, 0, 0 },
      { 0, 0, 0, 0 } },
    { { 1, 1, 0, 0 }, /* L3 */
      { 0, 1, 0, 0 },
      { 0, 1, 0, 0 },
      { 0, 0, 0, 0 } },
    { { 0, 0, 1, 0 }, /* L4 */
      { 1, 1, 1, 0 },
      { 0, 0, 0, 0 },
      { 0, 0, 0, 0 } },
    { { 0, 1, 0, 0 }, /* RL1 */
      { 0, 1, 0, 0 },
      { 1, 1, 0, 0 },
      { 0, 0, 0, 0 } }, 
    { { 1, 0, 0, 0 }, /* RL2 */
      { 1, 1, 1, 0 },
      { 0, 0, 0, 0 },
      { 0, 0, 0, 0 } }, 
    { { 1, 1, 0, 0 }, /* RL3 */
      { 1, 0, 0, 0 },
      { 1, 0, 0, 0 },
      { 0, 0, 0, 0 } }, 
    { { 1, 1, 1, 0 }, /* RL4 */
      { 0, 0, 1, 0 },
      { 0, 0, 0, 0 },
      { 0, 0, 0, 0 } }, 
    { { 1, 1, 1, 0 }, /* T1 */
      { 0, 1, 0, 0 },
      { 0, 0, 0, 0 },
      { 0, 0, 0, 0 } }, 
    { { 0, 1, 0, 0 }, /* T2 */
      { 1, 1, 0, 0 },
      { 0, 1, 0, 0 },
      { 0, 0, 0, 0 } }, 
    { { 0, 1, 0, 0 }, /* T3 */
      { 1, 1, 1, 0 },
      { 0, 0, 0, 0 },
      { 0, 0, 0, 0 } }, 
    { { 1, 0, 0, 0 }, /* T4 */
      { 1, 1, 0, 0 },
      { 1, 0, 0, 0 },
      { 0, 0, 0, 0 } }, 
    { { 1, 1, 0, 0 }, /* Z1 */
      { 0, 1, 1, 0 },
      { 0, 0, 0, 0 },
      { 0, 0, 0, 0 } }, 
    { { 0, 1, 0, 0 }, /* Z2 */
      { 1, 1, 0, 0 },
      { 1, 0, 0, 0 },
      { 0, 0, 0, 0 } }, 
    { { 0, 1, 1, 0 }, /* RZ1 */
      { 1, 1, 0, 0 },
      { 0, 0, 0, 0 },
      { 0, 0, 0, 0 } }, 
    { { 1, 0, 0, 0 }, /* RZ2 */
      { 1, 1, 0, 0 },
      { 0, 1, 0, 0 },
      { 0, 0, 0, 0 } }, 
};

class block {
public:
    void reborn();
    void move(movedir md); 
    void transform();       
    void draw(console::draw* d) const;

private:
    blocktype type;
    int x;
    int y;

    friend class panel;
};

class panel {
public:
    void clear();
    void draw(console::draw* d) const;
    /* 检测方块是否与面板碰撞 */
    bool collision(const block* blk) const;
    /* 检测方块是否可以往某个方向移动 */
    bool canmove(const block* blk, movedir md) const;
    /* 检测方块是否可以变形 */
    bool cantransform(const block* blk) const; 
    /* 方块着陆，返回可消去的层数 */
    int land(block* blk);
    /* 检测某行是否已满 */
    bool linefull(int row) const;
    /* 擦除某行 */
    void eraseline(int row);
    /* 立刻落下，返回可消去的层数 */
    int fallatonce(block* blk);

private:
    int blocktypes[ROW][COL];    
};

}