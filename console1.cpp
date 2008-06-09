#include "console1.h"
#include <assert.h>

#define DIM(array) (sizeof(array) / sizeof(*array))
#define ZEROARRAY(array) memset(array, 0, sizeof(array));

namespace console {

/*====================== exception =======================*/

const char* invalidhandle::what() const
{
    static char buf[256];
    sprintf_s(buf, DIM(buf),
        "Invalid handle: 0x%0x\n"
        "Error code: %d", 
        (long)m_handle, GetLastError());
    return buf;
}


const char* unsupportsize::what() const
{
    static char buf[256];
    sprintf_s(buf, DIM(buf),
        "Unsupport size: %d x %d",
        m_size->w, m_size->h);
    return buf;
}

/*======================= rectangle =======================*/

bool rectangle::intersect(const rectangle& src, rectangle& dest) const
{
    if (src.right <= left || right <= src.left || 
        src.bottom <= top || bottom <= src.top)
        return false;

    dest.left = max(left, src.left);
    dest.top = max(top, src.top);
    dest.right = min(right, src.right);
    dest.bottom = min(bottom, src.bottom);
    return true;
}

/*========================= draw ==========================*/

draw::draw(size wndsize)
{
    m_mainbuf = GetStdHandle(STD_OUTPUT_HANDLE);
    if (m_mainbuf == INVALID_HANDLE_VALUE)
        throw invalidhandle(m_mainbuf);

    m_backbuf = CreateConsoleScreenBuffer( 
       GENERIC_READ | GENERIC_WRITE, 0, NULL, 
       CONSOLE_TEXTMODE_BUFFER, NULL);    
    if (m_backbuf == INVALID_HANDLE_VALUE)
        throw invalidhandle(m_backbuf);

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(m_backbuf, &csbi);
    
    if (wndsize.w > csbi.dwMaximumWindowSize.X ||
        wndsize.h > csbi.dwMaximumWindowSize.Y)
    {
        CloseHandle(m_backbuf);
        throw unsupportsize(wndsize);
    }
    
    m_wndrect = rectangle(point(0,0), wndsize);

    SMALL_RECT rect = m_wndrect;
    /* 注意必须先修改窗口大小, 否则会出现缓冲区小于窗口的错误 */
    SetConsoleWindowInfo(m_mainbuf, true, &rect);
    SetConsoleWindowInfo(m_backbuf, true, &rect);
    SetConsoleScreenBufferSize(m_mainbuf, wndsize);    
    SetConsoleScreenBufferSize(m_backbuf, wndsize);
}

draw::~draw()
{
    CloseHandle(m_backbuf);
}

void draw::clear(color c)
{
    if (c == transparent)
        return;

    COORD coord = { 0, 0 };
    FillConsoleOutputAttribute(m_backbuf, makecolor(white, c), m_wndrect.area(), coord, NULL);
    FillConsoleOutputCharacterA(m_backbuf, ' ', m_wndrect.area(), coord, NULL);
}

void draw::flip()
{
    SetConsoleActiveScreenBuffer(m_backbuf);
    std::swap(m_mainbuf, m_backbuf);
}

void draw::drawpixel(position pos, color c)
{
    if (c == transparent)
        return;

    if (m_wndrect.pointin(pos)) {  
        FillConsoleOutputAttribute(m_backbuf, makecolor(white, c), 1, pos, NULL);
        FillConsoleOutputCharacter(m_backbuf, ' ', 1, pos, NULL);
    }
}

void draw::drawline() {}
void draw::drawrect() {}
void draw::drawtext() {}

/*========================= input =========================*/

input::input()
{
    m_inputhandle = GetStdHandle(STD_INPUT_HANDLE);
    if (m_inputhandle == INVALID_HANDLE_VALUE)
        throw invalidhandle(m_inputhandle);

    SetConsoleMode(m_inputhandle, ENABLE_MOUSE_INPUT);    

    ZEROARRAY(m_keysdown);
    ZEROARRAY(m_lastkeysdown);
    ZEROARRAY(m_buttonsdown);
    ZEROARRAY(m_lastbuttonsdown);    
}

void input::update()
{
    memcpy(m_lastkeysdown, m_keysdown, sizeof(m_keysdown));
    memcpy(m_lastbuttonsdown, m_buttonsdown, sizeof(m_buttonsdown));        
    ZEROARRAY(m_buttonsdblclick);
    
    while (1) {
        DWORD eventcnt; 
        GetNumberOfConsoleInputEvents(m_inputhandle, &eventcnt);
        if (eventcnt == 0)
            break;
        
        INPUT_RECORD rec;
        DWORD read = 0;
        ReadConsoleInput(m_inputhandle, &rec, 1, &read);

        switch (rec.EventType) {
        case KEY_EVENT:            
            m_keysdown[rec.Event.KeyEvent.wVirtualKeyCode] = rec.Event.KeyEvent.bKeyDown ? true : false;
            break;

        case MOUSE_EVENT:
            if (rec.Event.MouseEvent.dwEventFlags == 0) {                 
                m_buttonsdown[0] = (rec.Event.MouseEvent.dwButtonState & 0x01) ? true : false;                
                m_buttonsdown[1] = (rec.Event.MouseEvent.dwButtonState & 0x02) ? true : false;                
                m_buttonsdown[2] = (rec.Event.MouseEvent.dwButtonState & 0x04) ? true : false;
            }
            else if (rec.Event.MouseEvent.dwEventFlags == MOUSE_MOVED) {
                m_mousepos.x = rec.Event.MouseEvent.dwMousePosition.X;
                m_mousepos.y = rec.Event.MouseEvent.dwMousePosition.Y;
            }
            else if (rec.Event.MouseEvent.dwEventFlags == DOUBLE_CLICK) {
                if (rec.Event.MouseEvent.dwButtonState & 0x01)
                    m_buttonsdblclick[0] = true;
                if (rec.Event.MouseEvent.dwButtonState & 0x02)
                    m_buttonsdblclick[1] = true;
                if (rec.Event.MouseEvent.dwButtonState & 0x04)
                    m_buttonsdblclick[2] = true;                
            }
            else /*(rec.Event.MouseEvent.dwEventFlags == MOUSE_WHEELED)*/ {
                // DO NOTHING
            }
        } // end switch 
    } // end while
}

}