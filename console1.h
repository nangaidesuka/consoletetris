#pragma once

#include <windows.h>
#include <string>

namespace console {

/*========================= exception =========================*/

class invalidhandle : public std::exception {
public:
    invalidhandle(HANDLE handle);
    const char* what() const;

private:
    HANDLE m_handle;
};

/*========================= point =========================*/

class point {
public:
    point();
    point(short x, short y);
    point(COORD coord);
    operator COORD() const;
    short x;
    short y;
};

typedef point position;

/*========================= size ==========================*/

class size {
public:
    size();
    size(short w, short h);
    size(COORD coord);
    operator COORD() const;
    long area() const;
    short w;
    short h;
};

/*======================= rectangle =======================*/

/*
** 矩形: 包括左上角, 不包括右下角
*/
class rectangle {
public:
    rectangle();
    rectangle(short left, short top, short right, short bottom);
    rectangle(point topleft, point bottomright);
    rectangle(point topleft, size sz);
    rectangle(const SMALL_RECT& smallrect);
    operator SMALL_RECT() const;
    
    point topleft() const;
    point bottomright() const;
    point topright() const;
    point bottomleft() const;    

    short width() const;
    short height() const;
    long area() const;

    bool pointin(point pt) const;
    bool intersect(const rectangle& src, rectangle& dest) const;

    short left;
    short top;
    short right;
    short bottom;
};

/*========================= color =========================*/

enum { intensity = 0x0008 };
enum color {
    black       = 0x00,
    red         = 0x04,
    green       = 0x02,
    blue        = 0x01,
    yellow      = red | green,
    cyan        = green | blue,
    magenta     = blue | red,
    white       = red | green | blue,
    black_i     = black     | intensity,
    red_i       = red       | intensity,
    green_i     = green     | intensity,
    blue_i      = blue      | intensity,
    yellow_i    = yellow    | intensity,
    cyan_i      = cyan      | intensity,
    magenta_i   = magenta   | intensity,
    white_i     = white     | intensity,
    transparent = 0x10,
};

/*========================= draw ==========================*/

/*
** 具有双缓冲的绘图窗口
*/
class draw {
public:
    draw(size wndsize);    
    void clearscreen(color c);
    void flip();
    
    void drawpixel();
    void drawline();
    void drawrect();
    void drawtext();

    position homepos() const;

private:
    HANDLE m_mainbuf;
    HANDLE m_backbuf;
    size m_wndsize;
};

/*========================= input =========================*/

class input {
public:
    enum button { leftbutton, rightbutton, middlebutton };

    input();
    void update();

    bool keydown(int virtualkey);
    bool keyclick(int virtualkey);
    bool buttondown(button btn);
    bool buttonclick(button btn);
    bool buttondblclick(button btn);

    position mousepos() const;

    HANDLE safehandle() const;

private:
    HANDLE m_inputhandle;
    bool m_keysdown[256];
    bool m_lastkeysdown[256];
    bool m_buttonsdown[3];
    bool m_lastbuttonsdown[3];
    bool m_buttonsdblclick[3];
    position m_mousepos;
};

/*========================= exception =========================*/

inline
invalidhandle::invalidhandle(HANDLE handle)
: m_handle(handle)
{}

/*========================= point =========================*/

inline 
point::point()
: x(0), y(0)
{}

inline 
point::point(short x_, short y_)
: x(x_), y(y_)
{}

inline 
point::point(COORD coord)
: x(coord.X), y(coord.Y)
{}

inline 
point::operator COORD() const
{
    COORD coord = { x, y };
    return coord;
}

/*========================= size ==========================*/

inline 
size::size()
: w(1), h(1)
{}

inline 
size::size(short w_, short h_)
: w(w_), h(h_)
{}

inline 
size::size(COORD coord)
: w(coord.X), h(coord.Y)
{}

inline 
size::operator COORD() const
{
    COORD coord = { w, h };
    return coord;
}

inline 
long size::area() const
{ return w * h; }

/*======================= rectangle =======================*/

inline 
rectangle::rectangle()
: left(0), top(0)
, right(1), bottom(1)
{}

inline 
rectangle::rectangle(short l, short t, short r, short b)
: left(l), top(t)
, right(r), bottom(b)
{}

inline 
rectangle::rectangle(point topleft, point bottomright)
: left(topleft.x), top(topleft.y)
, right(bottomright.x), bottom(bottomright.y)
{}

inline 
rectangle::rectangle(point topleft, size sz)
: left(topleft.x), top(topleft.y)
, right(sz.w), bottom(sz.h)
{}

inline 
rectangle::rectangle(const SMALL_RECT& smallrect)
: left(smallrect.Left), top(smallrect.Top)
, right(smallrect.Right+1), bottom(smallrect.Bottom+1)
{}

inline 
rectangle::operator SMALL_RECT() const
{
    SMALL_RECT smallrect = { left, top, right-1, bottom-1 };
    return smallrect;
}

inline 
point rectangle::topleft() const
{ return point(left, top); }

inline 
point rectangle::bottomright() const
{ return point(right, bottom); }

inline 
point rectangle::topright() const
{ return point(right, top); }

inline 
point rectangle::bottomleft() const
{ return point(left, bottom); }

inline 
short rectangle::width() const
{ return right - left; }

inline 
short rectangle::height() const
{ return bottom - top; }

inline 
long rectangle::area() const
{ return width() * height(); }

inline 
bool rectangle::pointin(point pt) const
{ 
    return left <= pt.x && pt.x < right
        && top <= pt.y && pt.y < bottom;
}

/*========================= draw ==========================*/

inline 
position draw::homepos() const
{ return point(0, 0); }

/*========================= input =========================*/

inline
bool input::keydown(int virtualkey)
{ return m_keysdown[virtualkey]; }

inline
bool input::keyclick(int virtualkey)
{
    return m_keysdown[virtualkey]
        && !m_lastkeysdown[virtualkey];
}

inline
bool input::buttondown(button btn)
{ return m_buttonsdown[btn]; }

inline
bool input::buttonclick(button btn)
{
    return m_buttonsdown[btn] 
        && !m_lastbuttonsdown[btn];
}

inline
bool input::buttondblclick(button btn)
{ return m_buttonsdblclick[btn]; }

inline
position input::mousepos() const
{ return m_mousepos; }

inline
HANDLE input::safehandle() const
{ return m_inputhandle; }

}

