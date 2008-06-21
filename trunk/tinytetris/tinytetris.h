#pragma once

#include "console.h"

namespace tetris {

/* ��������� */
const int ROW = 14;
const int COL = 18;

/* ������ƶ����� */
enum movedir {
    moveleft, moveright, movedown
};

/* �������� */
enum blocktype {
    IV, IH,
    TIAN,
    L1, L2, L3, L4, /* ˳ʱ���α� */
    RL1, RL2, RL3, RL4,
    T1, T2, T3, T4, 
    Z1, Z2, 
    RZ1, RZ2, 
    NUMBLOCKTYPES
};

/* �����������ɫ */
const console::color blockcolors[NUMBLOCKTYPES] = {    
    console::red_i, console::red_i,
    console::white_i,
    console::green_i, console::green_i, console::green_i, console::green_i,
    console::blue_i, console::blue_i, console::blue_i, console::blue_i,
    console::yellow_i, console::yellow_i, console::yellow_i, console::yellow_i,
    console::cyan_i, console::cyan_i,
    console::magenta_i, console::magenta_i,
};

/* ��������Ͳ��� */
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
    /* ��ⷽ���Ƿ��������ײ */
    bool collision(const block* blk) const;
    /* ��ⷽ���Ƿ������ĳ�������ƶ� */
    bool canmove(const block* blk, movedir md) const;
    /* ��ⷽ���Ƿ���Ա��� */
    bool cantransform(const block* blk) const; 
    /* ������½�����ؿ���ȥ�Ĳ��� */
    int land(block* blk);
    /* ���ĳ���Ƿ����� */
    bool linefull(int row) const;
    /* ����ĳ�� */
    void eraseline(int row);
    /* �������£����ؿ���ȥ�Ĳ��� */
    int fallatonce(block* blk);

private:
    int blocktypes[ROW][COL];    
};

}