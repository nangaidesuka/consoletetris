#include "console.h"

#define dim(array) (sizeof(array)/sizeof(*array))

/*=========================== rect ===========================*/

bool rect::intersect(rect& dest, const rect& src) const
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

console::console()
{
    m_hout = GetStdHandle(STD_OUTPUT_HANDLE);  
    m_fullscreen = false;

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(m_hout, &csbi); 
    
    m_wndrect.l = m_wndrect.t = 0;
    m_wndrect.r = csbi.srWindow.Right - csbi.srWindow.Left + m_wndrect.l;
    m_wndrect.b = csbi.srWindow.Bottom - csbi.srWindow.Top + m_wndrect.t;

    SetConsoleScreenBufferSize(m_hout, point(m_wndrect.w(), m_wndrect.h()).tocoord_copy());

    m_cci.bVisible = TRUE;
    m_cci.dwSize = 1;  

    m_hout2 = CreateConsoleScreenBuffer( 
       GENERIC_READ |           // read/write access 
       GENERIC_WRITE, 
       0,                       // not shared 
       NULL,                    // default security attributes 
       CONSOLE_TEXTMODE_BUFFER, // must be TEXTMODE 
       NULL);                   // reserved; must be NULL     
}

console::~console()
{
    CloseHandle(m_hout);
}

void console::fullscreen(bool set)
{    
    if (m_fullscreen ^ set) {
    /*    keybd_event(VK_MENU, 0x38, 0, 0);
        keybd_event(VK_RETURN, 0x1c, 0, 0);
        keybd_event(VK_RETURN, 0x1c, KEYEVENTF_KEYUP, 0);
        keybd_event(VK_MENU, 0x38, KEYEVENTF_KEYUP, 0);*/
        COORD newdim;
        SetConsoleDisplayMode(m_hout, CONSOLE_FULLSCREEN_MODE, &newdim);
        
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(m_hout, &csbi);         
        
        m_wndrect.l = m_wndrect.t = 0;
        m_wndrect.r = csbi.srWindow.Right - csbi.srWindow.Left + m_wndrect.l;
        m_wndrect.b = csbi.srWindow.Bottom - csbi.srWindow.Top + m_wndrect.t;

        m_fullscreen = !m_fullscreen;
    }
}

bool console::fullscreen() const
{   
    return m_fullscreen;
}

void console::title(const stdstr& s)
{
    SetConsoleTitle(s.c_str());
}

const tchar* console::title() const
{
    static tchar tit[MAX_PATH] = {0};
    GetConsoleTitle(tit, MAX_PATH);
    return tit;
}

void console::movewnd(const rect& rc)
{    
    SetConsoleWindowInfo(m_hout, true, &rc.tosmallrect_copy());        
    SetConsoleScreenBufferSize(m_hout, point(rc.w(), rc.h()).tocoord_copy()); 
    SetConsoleWindowInfo(m_hout2, true, &rc.tosmallrect_copy());        
    SetConsoleScreenBufferSize(m_hout2, point(rc.w(), rc.h()).tocoord_copy());
    
    m_wndrect.l = m_wndrect.t = 0;
    m_wndrect.r = rc.r - rc.l + m_wndrect.l;
    m_wndrect.b = rc.b - rc.t + m_wndrect.t;    
}

const rect& console::wndrect() const 
{ 
    return m_wndrect; 
}

point console::wndsizemax() const
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(m_hout, &csbi); 
    return point(csbi.dwMaximumWindowSize.X, csbi.dwMaximumWindowSize.Y);
}

void console::clearscreen(color c)
{
    assert (c != transparent);

    COORD coord = {0, 0};    
    DWORD scrsize = (DWORD)m_wndrect.w()*(DWORD)m_wndrect.h();
    FillConsoleOutputAttribute(m_hout, c<<4, scrsize, coord, NULL);
    FillConsoleOutputCharacter(m_hout, TEXT(' '), scrsize, coord, NULL);
}

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

void console::setpixel(point pos, color c)
{
    assert (c != transparent);

    if (m_wndrect.pointin(pos)) {
        // erase characters in rect
        FillConsoleOutputCharacter(m_hout, TEXT(' '), 1, pos.tocoord_copy() , NULL);
        // draw background color     
        FillConsoleOutputAttribute(m_hout, c<<4, 1, pos.tocoord_copy() , NULL);
    }
}

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

void console::cursorvisible(bool visible)
{    
    m_cci.bVisible = visible ? TRUE : FALSE;    
    SetConsoleCursorInfo(m_hout, &m_cci);
}

bool console::cursorvisible() const
{
    return m_cci.bVisible ? true : false;
}

void console::cursorsize(int size)
{
    assert(1 <= size && size <= 100);
    m_cci.dwSize = (DWORD)size;
    SetConsoleCursorInfo(m_hout, &m_cci);
}

int console::cursorsize() const
{
    return m_cci.dwSize;
}

void console::cursorpos(point pos)
{    
    assert(m_wndrect.pointin(pos) && "Can't set cursor out of window");
    SetConsoleCursorPosition(m_hout, pos.tocoord_copy());
}

point console::cursorpos() const
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(m_hout, &csbi);
    return point(csbi.dwCursorPosition.X, csbi.dwCursorPosition.Y);
}

void console::codepage(int cp)
{
    SetConsoleOutputCP(UINT(cp));
}

int console::codepage() const
{
    return (int)GetConsoleOutputCP();
}

#ifdef _DEBUG
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

#ifdef _DEBUG
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

HANDLE console::safehandle() const 
{
    return m_hout; 
}

/*=========================== intput ===========================*/

input::input()
{
    m_hin = GetStdHandle(STD_INPUT_HANDLE);    
    SetConsoleMode(m_hin, ENABLE_MOUSE_INPUT);
    //SetConsoleMode(m_hin, ENABLE_WINDOW_INPUT);

    memset(m_keys, 0, dim(m_keys));
    memset(m_lastkeys, 0, dim(m_lastkeys));
    memset(m_btns, 0, dim(m_btns));
    memset(m_lastbtns, 0, dim(m_lastbtns));
}

bool input::keydown(int vk) const
{
    return m_keys[vk];
}

bool input::keyclick(int vk) const
{
    return m_keys[vk] && !m_lastkeys[vk];
}

bool input::btndown(button btn) const
{
    return m_btns[btn];
}

bool input::btnclick(button btn) const
{
    return m_btns[btn] && !m_lastbtns[btn];
}

bool input::btndblclick(button btn) const
{
    return m_btndblclk[btn];
}

point input::mousepos() const
{
    return m_mousepos;
}

void input::update()
{    
    for (int i=0; i< dim(m_keys); ++i) 
        m_lastkeys[i] = m_keys[i];

    for (int i=0; i< dim(m_btns); ++i) 
        m_lastbtns[i] = m_btns[i];
    
    memset(m_btndblclk, 0, dim(m_btndblclk));
    
    while (1) {
        DWORD eventcnt; 
        GetNumberOfConsoleInputEvents(m_hin, &eventcnt);
        if (eventcnt == 0)
            break;
        
        INPUT_RECORD rec;
        DWORD read = 0;
        ReadConsoleInput(m_hin, &rec, 1, &read);

        switch (rec.EventType) {
        case KEY_EVENT:            
            m_keys[rec.Event.KeyEvent.wVirtualKeyCode] = rec.Event.KeyEvent.bKeyDown ? true : false;
            break;

        case MOUSE_EVENT:
            if (rec.Event.MouseEvent.dwEventFlags == 0) {                 
                m_btns[0] = (rec.Event.MouseEvent.dwButtonState & 0x01) ? true : false;                
                m_btns[1] = (rec.Event.MouseEvent.dwButtonState & 0x02) ? true : false;                
                m_btns[2] = (rec.Event.MouseEvent.dwButtonState & 0x04) ? true : false;
            }
            else if (rec.Event.MouseEvent.dwEventFlags == MOUSE_MOVED) {
                m_mousepos.x = rec.Event.MouseEvent.dwMousePosition.X;
                m_mousepos.y = rec.Event.MouseEvent.dwMousePosition.Y;
            }
            else if (rec.Event.MouseEvent.dwEventFlags == DOUBLE_CLICK) {
                if (rec.Event.MouseEvent.dwButtonState & 0x01)
                    m_btndblclk[0] = true;
                if (rec.Event.MouseEvent.dwButtonState & 0x02)
                    m_btndblclk[1] = true;
                if (rec.Event.MouseEvent.dwButtonState & 0x04)
                    m_btndblclk[2] = true;                
            }
            else /*(rec.Event.MouseEvent.dwEventFlags == MOUSE_WHEELED)*/ {
                // DO NOTHING
            }
        case WINDOW_BUFFER_SIZE_EVENT:
            int i = 0; ++i;
            break;
        } // end switch 
    } // end while
}