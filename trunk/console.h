#pragma once

#include <windows.h>

#include <string>
#ifndef UNICODE
typedef char tchar;
typedef std::string stdstr;
#else
typedef wchar_t tchar;
typedef std::wstring stdstr;
#endif

#include <cassert>

/*=========================== point ===========================*/

class point {
public:
    point() : x(0), y(0) {}
    point(short x_, short y_) : x(x_), y(y_) {}
    COORD tocoord_copy() const { COORD res = {x, y}; return res; }
    short x;
    short y;
};

/*=========================== rect ===========================*/

class rect {
public:
    rect() { l = t = r = b = 0; }
    rect(short l_, short t_, short w_, short h_) : l(l_), t(t_), r(l_+w_-1), b(t_+h_-1) {}    
    short w() const { return r-l+1; }
    short h() const { return b-t+1; }
    point lt() const { return point(l, t); }
    point lb() const { return point(l, b); }
    point rt() const { return point(r, t); }
    point rb() const { return point(r, b); }   
    SMALL_RECT tosmallrect_copy() const { SMALL_RECT res = {l, t, r, b}; return res; }
    bool pointin(point pt) const { return l <= pt.x && pt.x <= r && t <= pt.y && pt.y <= b; }
    bool intersect(rect& dest, const rect& src) const;
    short l;
    short t;
    short r;
    short b;
};

inline bool rect::intersect(rect& dest, const rect& src) const
{
    if (src.r < l || src.l > r || src.b < t || src.t > b)
        return false;
    dest.l = max(l, src.l);
    dest.t = max(t, src.t);
    dest.r = min(r, src.r);
    dest.b = min(b, src.b);
    return true;    
}

/*=========================== console ===========================*/

class console {

    /* color definitioin */

private:
    enum {intensity = 0x0008};
public:
    enum color {
        black       = 0x00,
        red         = 0x04,
        green       = 0x02,
        blue        = 0x01,
        yellow      = red | green,
        cyan        = green | blue,
        magenta     = blue | red,
        white       = red | green | blue,
        black_i     = black | intensity,
        red_i       = red | intensity,
        green_i     = green | intensity,
        blue_i      = blue | intensity,
        yellow_i    = yellow | intensity,
        cyan_i      = cyan | intensity,
        magenta_i   = magenta | intensity,
        white_i     = white | intensity,
        transparent = 0x10
    };

public:
    console();
    ~console();
    
    /* control window */ 

    void fullscreen(bool set);
    bool fullscreen() const;

    void title(const stdstr& s);
    const tchar* title() const;
    
    void movewnd(const rect& rc);    
    const rect& wndrect() const;
    
    /* do drawing */

    void clearscreen(color c=black);

    void textout(tchar c, point pt, color forecolor=white, color bkcolor=transparent);
    void textout(const stdstr& s, point pt, color forecolor=white, color bkcolor=transparent);

    void setpixcel(point pos, color c);
    void drawrect(const rect& rc, color c);

    /* control cursor */
    
    void cursorvisible(bool visible);
    bool cursorvisible() const;    
    void cursorsize(int size); // 1-100
    int cursorsize() const;
    void cursorpos(point pos); // can't set cursor out of window
    point cursorpos() const;

    /* control codepage */    

    // NOTE: the code page impact all the character even that has been drawed
    // different code page may have different character font
    void codepage(int cp);
    int codepage() const;  

#ifndef NDEBUG    
    void drawcodepage();
#endif

    /* control keyboard */

    bool keydown(int vk);
    bool togglekeydown(int vk);

    /* other */

#ifndef NDEBUG
    void drawcolortab();
#endif

    HANDLE safehandle() const;
    
private:   
    HANDLE m_hout; 
    bool m_fullscreen;
    rect m_wndrect;
    CONSOLE_CURSOR_INFO m_cci;
    point m_cursorpos;
};

inline
console::console()
{
    m_hout = GetStdHandle(STD_OUTPUT_HANDLE);  
    m_fullscreen = false;

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(m_hout, &csbi); 
    
    m_wndrect.l = m_wndrect.t = 0;
    m_wndrect.r = csbi.srWindow.Right - csbi.srWindow.Left + m_wndrect.l;
    m_wndrect.b = csbi.srWindow.Bottom - csbi.srWindow.Top + m_wndrect.t;

    m_cci.bVisible = TRUE;
    m_cci.dwSize = 1;  
}

inline
console::~console()
{
    CloseHandle(m_hout);
}

void console::fullscreen(bool set)
{    
    if (m_fullscreen ^ set) {
        keybd_event(VK_MENU, 0x38, 0, 0);
        keybd_event(VK_RETURN, 0x1c, 0, 0);
        keybd_event(VK_RETURN, 0x1c, KEYEVENTF_KEYUP, 0);
        keybd_event(VK_MENU, 0x38, KEYEVENTF_KEYUP, 0);
    }
}

bool console::fullscreen() const
{
    return m_fullscreen;
}

inline
void console::title(const stdstr& s)
{
    SetConsoleTitle(s.c_str());
}

inline
const tchar* console::title() const
{
    static tchar tit[MAX_PATH] = {0};
    GetConsoleTitle(tit, MAX_PATH);
    return tit;
}

inline
void console::movewnd(const rect& rc)
{    
    SetConsoleWindowInfo(m_hout, true, &rc.tosmallrect_copy());        
    SetConsoleScreenBufferSize(m_hout, point(rc.w(), rc.h()).tocoord_copy()); 
    
    m_wndrect.l = m_wndrect.t = 0;
    m_wndrect.r = rc.r - rc.l + m_wndrect.l;
    m_wndrect.b = rc.b - rc.t + m_wndrect.t;    
}

inline 
const rect& console::wndrect() const 
{ 
    return m_wndrect; 
}

inline
void console::clearscreen(color c)
{
    assert (c != transparent);

    COORD coord = {0, 0};    
    DWORD scrsize = (DWORD)m_wndrect.w()*(DWORD)m_wndrect.h();
    FillConsoleOutputAttribute(m_hout, c<<4, scrsize, coord, NULL);
    FillConsoleOutputCharacter(m_hout, TEXT(' '), scrsize, coord, NULL);
}

inline
void console::textout(tchar c, point pt, color forecolor, color bkcolor)
{
    assert (forecolor != transparent);

    if (m_wndrect.pointin(pt)) {
        WORD attr; 
        DWORD read;
        if (bkcolor == transparent) {
            ReadConsoleOutputAttribute(m_hout, &attr, 1, pt.tocoord_copy(), &read);
            // keep background color and replace foreground color
            attr = (attr&0xF0) | forecolor;
        }
        else
            attr = (WORD)(forecolor|(bkcolor<<4));

        FillConsoleOutputAttribute(m_hout, attr, 1, pt.tocoord_copy(), NULL);
        FillConsoleOutputCharacter(m_hout, c, 1, pt.tocoord_copy(), NULL);
    }
}

inline
void console::textout(const stdstr& s, point pt, color forecolor, color bkcolor)
{
    assert (forecolor != transparent);

    short len = (short)s.length();
    short r = pt.x + len - 1;
    if (r < m_wndrect.l || pt.x > m_wndrect.r || 
        pt.y < m_wndrect.t || pt.y > m_wndrect.b)
        return;
    
    point lt;
    lt.x = max(pt.x, m_wndrect.l);
    lt.y = pt.y;        

    r = min(r, m_wndrect.r);
    len = r - lt.x + 1;

    stdstr subs = s.substr(lt.x - pt.x, len);

    if (bkcolor == transparent) {
        for (short i=0; i<len; ++i)
            textout(subs.c_str()[i], point(lt.x+i, lt.y), forecolor, bkcolor);
    }
    else {
        FillConsoleOutputAttribute(m_hout, forecolor|(bkcolor<<4), len, lt.tocoord_copy() , NULL);
        WriteConsoleOutputCharacter(m_hout, subs.c_str(), len, lt.tocoord_copy(), NULL);
    }
}

inline
void console::setpixcel(point pos, color c)
{
    assert (c != transparent);

    if (m_wndrect.pointin(pos)) {
        // erase characters in rect
        FillConsoleOutputCharacter(m_hout, TEXT(' '), 1, pos.tocoord_copy() , NULL);
        // draw background color     
        FillConsoleOutputAttribute(m_hout, c<<4, 1, pos.tocoord_copy() , NULL);
    }
}

inline
void console::drawrect(const rect& rc, color c)
{
    assert (c != transparent);

    rect intersect;
    if (m_wndrect.intersect(intersect, rc)) {
        for (int y=intersect.t; y<=intersect.b; y++) {
            // erase characters in rect
            FillConsoleOutputCharacter(m_hout, TEXT(' '), intersect.w(), point(intersect.l, y).tocoord_copy() , NULL);
            // draw background color     
            FillConsoleOutputAttribute(m_hout, c<<4, intersect.w(), point(intersect.l, y).tocoord_copy() , NULL);
        }
    }
}

inline
void console::cursorvisible(bool visible)
{    
    m_cci.bVisible = visible ? TRUE : FALSE;    
    SetConsoleCursorInfo(m_hout, &m_cci);
}

inline
bool console::cursorvisible() const
{
    return m_cci.bVisible ? true : false;
}

inline
void console::cursorsize(int size)
{
    assert(1 <= size && size <= 100);
    m_cci.dwSize = (DWORD)size;
    SetConsoleCursorInfo(m_hout, &m_cci);
}

inline
int console::cursorsize() const
{
    return m_cci.dwSize;
}

inline
void console::cursorpos(point pos)
{    
    assert(m_wndrect.pointin(pos) && "Can't set cursor out of window");
    SetConsoleCursorPosition(m_hout, pos.tocoord_copy());
}

inline
point console::cursorpos() const
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(m_hout, &csbi);
    return point(csbi.dwCursorPosition.X, csbi.dwCursorPosition.Y);
}

inline
void console::codepage(int cp)
{
    SetConsoleOutputCP(UINT(cp));
}

inline
int console::codepage() const
{
    return (int)GetConsoleOutputCP();
}

#ifndef NDEBUG
inline
void console::drawcodepage()
{   
    const point pt(0, 0);
    const int cellspanx = 1;
    const int cellspany = 0;

    int i;
    const tchar hex[16] = 
    {TEXT('0'),TEXT('1'),TEXT('2'),TEXT('3'),TEXT('4'),TEXT('5'),TEXT('6'),TEXT('7'),TEXT('8'),TEXT('9'),
     TEXT('A'),TEXT('B'),TEXT('C'),TEXT('D'),TEXT('E'),TEXT('F')};    

    for (i=0; i<16; ++i)
        textout(hex[i], point((cellspanx+1)*(i+1)+pt.x, 0+pt.y), green);

    for (i=0; i<256; ++i) {
        int row = i/16;
        int col = i%16;
        if (i%16 == 0)
            textout(hex[row], point(0+pt.x, (cellspany+1)*(row+1)+pt.y), green);
        textout(i, point((cellspanx+1)*(col+1)+pt.x, (cellspany+1)*(row+1)+pt.y));
    }
}
#endif

inline
bool console::keydown(int vk)
{
    return (GetAsyncKeyState(vk) & 0x8000) ? true : false;
}

inline
bool console::togglekeydown(int vk)
{
    return false;
}

#ifndef NDEBUG
inline 
void console::drawcolortab()
{    
    textout(TEXT("red      "), point(0,0), black, red);
    textout(TEXT("green    "), point(0,1), black, green);
    textout(TEXT("blue     "), point(0,2), black, blue);
    textout(TEXT("yellow   "), point(0,3), black, yellow);
    textout(TEXT("cyan     "), point(0,4), black, cyan);
    textout(TEXT("magenta  "), point(0,5), black, magenta);
    textout(TEXT("white    "), point(0,6), black, white);

    textout(TEXT("red_i    "), point(9,0), black, red_i);
    textout(TEXT("green_i  "), point(9,1), black, green_i);
    textout(TEXT("blue_i   "), point(9,2), black, blue_i);
    textout(TEXT("yellow_i "), point(9,3), black, yellow_i);
    textout(TEXT("cyan_i   "), point(9,4), black, cyan_i);
    textout(TEXT("magenta_i"), point(9,5), black, magenta_i);
    textout(TEXT("white_i  "), point(9,6), black, white_i);
}
#endif

inline
HANDLE console::safehandle() const 
{
    return m_hout; 
}