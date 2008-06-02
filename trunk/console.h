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

class point {
public:
    point() : x(0), y(0) {}
    point(short x_, short y_) : x(x_), y(y_) {}
    COORD tocoord_copy() const { COORD res = {x, y}; return res; }
    short x;
    short y;
};

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

enum consolecolor {
    CC_BLUE         = 0x0001,
    CC_GREEN        = 0x0002,
    CC_RED          = 0x0004, 
    CC_INTENSITY    = 0x0008,
};

class console {
public:
    console();
    ~console();
    
    void movewnd(const rect& rc);
    void putc(tchar c, point pt, int forecolor=0x0F, int bkcolor=0);
    void textout(const stdstr& s, point pt, int forecolor=0x0F, int bkcolor=0);
    void drawrect(const rect& rc, int color);    

private:   
    HANDLE m_hout;    
    rect m_wndrect;
};

inline
console::console()
{
    m_hout = GetStdHandle(STD_OUTPUT_HANDLE);  

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(m_hout, &csbi); 
    
    m_wndrect.l = m_wndrect.t = 0;
    m_wndrect.r = csbi.srWindow.Right - csbi.srWindow.Left + m_wndrect.l;
    m_wndrect.b = csbi.srWindow.Bottom - csbi.srWindow.Top + m_wndrect.t;
}

inline
console::~console()
{
    CloseHandle(m_hout);
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
void console::putc(tchar c, point pt, int forecolor, int bkcolor)
{
    if (m_wndrect.pointin(pt)) {   
        FillConsoleOutputAttribute(m_hout, forecolor|(bkcolor<<4), 1, pt.tocoord_copy() , NULL);
        FillConsoleOutputCharacter(m_hout, c, 1, pt.tocoord_copy() , NULL);
    }
}

inline
void console::textout(const stdstr& s, point pt, int forecolor, int bkcolor)
{
    short len = s.length();
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

    FillConsoleOutputAttribute(m_hout, forecolor|(bkcolor<<4), len, lt.tocoord_copy() , NULL);
    WriteConsoleOutputCharacter(m_hout, subs.c_str(), len, lt.tocoord_copy(), NULL);
}

inline
void console::drawrect(const rect& rc, int color)
{
    rect intersect;
    if (m_wndrect.intersect(intersect, rc)) {
        for (int y=intersect.t; y<=intersect.b; y++) {
            // erase characters in rect
            FillConsoleOutputCharacter(m_hout, TEXT(' '), intersect.w(), point(intersect.l, y).tocoord_copy() , NULL);
            // draw background color     
            FillConsoleOutputAttribute(m_hout, color<<4, intersect.w(), point(intersect.l, y).tocoord_copy() , NULL);
        }
    }
}