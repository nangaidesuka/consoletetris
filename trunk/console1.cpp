#include "console1.h"

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
    SetConsoleScreenBufferSize(m_backbuf, size(80, 300));
    GetConsoleScreenBufferInfo(m_backbuf, &csbi);
    

#if 0
    m_csbi.dwSize;//80,300
    m_csbi.dwCursorPosition;//0,0
    m_csbi.wAttributes;//ºÚµ×°××Ö
    m_csbi.srWindow;//0,0,79,24
    m_csbi.dwMaximumWindowSize;//80,44
#endif
}

void draw::drawpixel() {}
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