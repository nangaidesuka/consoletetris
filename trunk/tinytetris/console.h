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

class size;
class unsupportsize : public std::exception {
public:
    unsupportsize(size sz, size szmax);
    ~unsupportsize();
    const char* what() const;

private:
    size* m_size;
    size* m_sizemax;
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
    bool intersect(const rectangle& rhs, rectangle& res) const;

    short left;
    short top;
    short right;
    short bottom;
};

/*========================= draw ==========================*/

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

/*
** 具有双缓冲的绘图窗口
*/
class draw {
public:
    /* 目前还没有办法判断最小窗口 */
    draw(size wndsize); 
    ~draw();
    void clear(color c=black);
    void flip();
    
    void drawpixel(position pos, color c);
    void drawlineh(position pos, short len, color c);
    void drawlinev(position pos, short len, color c);
    void drawrect(const rectangle& rect, color c);        
    /* 占两个字符宽的比如汉字在边界处可能绘制不出来 */        
    void drawtext(const std::string& s, position pos, color fgcolor=white, color bgcolor=transparent);
    void drawtext(const std::wstring& s, position pos, color fgcolor=white, color bgcolor=transparent);

    void drawcodepage();
    void drawcolortable();

    static position homepos();

    /* 只在一帧内有效 */
    HANDLE rawhandle() const;

private:    
    void showcursor(bool show=true);
    static WORD makecolor(color fgcolor, color bgcolor);
    static void chfgcolor(WORD& attr, color fgcolor);
    static bool checkascii(char c);
    static bool checkascii(wchar_t c);

    HANDLE m_mainbuf;
    HANDLE m_backbuf;
    rectangle m_wndrect;
};

/*========================= input =========================*/

enum button { leftbutton, rightbutton, middlebutton };

class input {
public:   

    input();
    void update();

    bool keydown(int vk);
    bool keyclick(int vk);
    bool buttondown(button btn);
    bool buttonclick(button btn);
    bool buttondblclick(button btn);

    position mousepos() const;

    HANDLE rawhandle() const;

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

inline
unsupportsize::unsupportsize(size sz, size szmax)
{ 
    m_size = new size(sz); 
    m_sizemax = new size(szmax);
}

inline
unsupportsize::~unsupportsize()
{ 
    delete m_size; 
    delete m_sizemax; 
}

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
, right(topleft.x+sz.w), bottom(topleft.y+sz.h)
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
position draw::homepos()
{ return point(0, 0); }

inline
HANDLE draw::rawhandle() const
{ return m_backbuf; }

inline 
WORD draw::makecolor(color fgcolor, color bgcolor)
{ return (bgcolor<<4) | fgcolor; }

inline 
void draw::chfgcolor(WORD& attr, color fgcolor)
{ attr = (attr&0xF0) | fgcolor; }

inline 
bool draw::checkascii(char c) 
{ return 0 <= c && c <= 127; }

inline 
bool draw::checkascii(wchar_t c)
{ 
    char buf[2];
    memcpy(buf, &c, 2);
    int n = 1;
    if (*(char*)&n) /* 小尾 */
        std::swap(buf[0], buf[1]);
    return (buf[0] == 0) && checkascii(buf[1]);
}

/*========================= input =========================*/

inline
bool input::keydown(int vk)
{ return m_keysdown[toupper(vk)]; }

inline
bool input::keyclick(int vk)
{
    return m_keysdown[toupper(vk)]
        && !m_lastkeysdown[toupper(vk)];
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
HANDLE input::rawhandle() const
{ return m_inputhandle; }

}

