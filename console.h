#pragma once

#include <windows.h>
#include <string>
#include <cassert>

#ifndef UNICODE
typedef char tchar;
typedef std::string stdstr;
#else
typedef wchar_t tchar;
typedef std::wstring stdstr;
#endif

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
    
    point wndsizemax() const;

    /* do drawing */

    void clearscreen(color c=black);

    void textout(tchar c, point pt, color forecolor=white, color bkcolor=transparent);
    void textout(const stdstr& s, point pt, color forecolor=white, color bkcolor=transparent);   

    void setpixel(point pos, color c);
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

    /* other */

#ifndef NDEBUG
    void drawcolortab();
#endif
    
    // return the output handle
    HANDLE safehandle() const;
    
private:   
    HANDLE m_hout; 
    bool m_fullscreen;
    rect m_wndrect;
    CONSOLE_CURSOR_INFO m_cci;
    point m_cursorpos;
};

/*=========================== intput ===========================*/

class input {
public:
    enum button {
        lbtn = 0,        
        rbtn = 1,
        mbtn = 2, 
    };

    input();
    void update();

    bool keydown(int vk) const;
    bool keyclick(int vk) const;
    bool btndown(button btn) const;
    bool btnclick(button btn) const; 
    bool btndblclick(button btn) const; 
    point mousepos() const;

private:
    HANDLE m_hin;    
    bool m_keys[256];       /*the down keys*/
    bool m_lastkeys[256];   /*the down keys*/
    bool m_btns[3];         /* mouse button click */
    bool m_lastbtns[3];     /* mouse button click */
    bool m_btndblclk[3];    /* mouse button double click */
    point m_mousepos;
};