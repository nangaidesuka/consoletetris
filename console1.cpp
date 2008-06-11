#include "console1.h"
#include <assert.h>

#define dim(array) (sizeof(array) / sizeof(*array))
#define zeroarray(array) memset(array, 0, sizeof(array));

namespace console {

/*====================== exception =======================*/

const char* invalidhandle::what() const
{
    static char buf[256];
    sprintf_s(buf, dim(buf),
        "Invalid handle: 0x%0x\n"
        "Error code: %d", 
        (long)m_handle, GetLastError());
    return buf;
}


const char* unsupportsize::what() const
{
    static char buf[256];
    sprintf_s(buf, dim(buf),
        "Unsupport size: %d x %d",
        m_size->w, m_size->h);
    return buf;
}

/*======================= rectangle =======================*/

bool rectangle::intersect(const rectangle& rhs, rectangle& res) const
{
    if (rhs.right <= left || right <= rhs.left || 
        rhs.bottom <= top || bottom <= rhs.top)
        return false;

    res.left = max(left, rhs.left);
    res.top = max(top, rhs.top);
    res.right = min(right, rhs.right);
    res.bottom = min(bottom, rhs.bottom);
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
    
    m_wndrect = rectangle(homepos(), wndsize);

    SMALL_RECT rect = m_wndrect;
    /* 注意必须先修改窗口大小, 否则会出现缓冲区小于窗口的错误 */
    SetConsoleWindowInfo(m_mainbuf, true, &rect);
    SetConsoleWindowInfo(m_backbuf, true, &rect);
    SetConsoleScreenBufferSize(m_mainbuf, wndsize);    
    SetConsoleScreenBufferSize(m_backbuf, wndsize);

    showcursor(false);
}

draw::~draw()
{
    CloseHandle(m_backbuf);
}

void draw::clear(color c)
{
    if (c == transparent)
        return;

    FillConsoleOutputAttribute(m_backbuf, makecolor(white, c), m_wndrect.area(), homepos(), NULL);
    FillConsoleOutputCharacterA(m_backbuf, ' ', m_wndrect.area(), homepos(), NULL);
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
        FillConsoleOutputCharacterA(m_backbuf, ' ', 1, pos, NULL);
    }
}

void draw::drawlineh(position pos, short len, color c)
{    
    drawrect(rectangle(pos, size(len, 1)), c);
}

void draw::drawlinev(position pos, short len, color c)
{
    drawrect(rectangle(pos, size(1, len)), c);
}

void draw::drawrect(const rectangle& rect, color c) 
{
    if (c == transparent)
        return;

    rectangle intersect;
    if (m_wndrect.intersect(rect, intersect)) {
        for (int y=intersect.top; y<intersect.bottom; y++) {
            COORD coord = { intersect.left, y };
            FillConsoleOutputAttribute(m_backbuf, makecolor(white, c), intersect.width(), coord, NULL);
            FillConsoleOutputCharacterA(m_backbuf, ' ', intersect.width(), coord, NULL);            
        }
    }
}


void draw::drawtext(const std::string& s, position pos, color fgcolor, color bgcolor)
{
    assert (fgcolor != transparent);

    /*
    ** 逐个字符绘制而不是整块绘制
    ** 因为很难确定块的起点和终点(比如汉字可能中间被打断)
    */

    const char* cs = s.c_str();         
    DWORD read;    
    int i = 0;

    char buf[2];
    while ((buf[0] = cs[i++]) != '\0') {
        /* 一旦某个字符超出又边界立刻停止 */
        if (pos.x >= m_wndrect.right)
            break;

        if (checkascii(buf[0])) {
            if (m_wndrect.pointin(pos)) {                
                WORD attr = makecolor(fgcolor, bgcolor);
                if (bgcolor == transparent) {
                    ReadConsoleOutputAttribute(m_backbuf, &attr, 1, pos, &read);
                    chfgcolor(attr, fgcolor);
                }
                FillConsoleOutputAttribute(m_backbuf, attr, 1, pos, NULL);     
                FillConsoleOutputCharacterA(m_backbuf, buf[0], 1, pos, NULL);
            }
            pos.x++;
        }
        else {
            if ((buf[1] = cs[i++]) == '\0')
                break;
            /* 两个字符都在窗口内才被绘制 */            
            if (m_wndrect.pointin(pos)) {
                position pos2(pos.x+1, pos.y);
                if (m_wndrect.pointin(pos2)) {                    
                    WORD attr[2];
                    attr[0] = attr[1] = makecolor(fgcolor, bgcolor);
                    if (bgcolor == transparent) {
                        ReadConsoleOutputAttribute(m_backbuf, attr, 2, pos, &read);
                        chfgcolor(attr[0], fgcolor);
                        chfgcolor(attr[1], fgcolor);
                    }
                    WriteConsoleOutputAttribute(m_backbuf, attr, 2, pos, NULL);
                    WriteConsoleOutputCharacterA(m_backbuf, buf, 2, pos, NULL);
                }                
            }
            pos.x += 2;
        }
    }  
}

void draw::drawtext(const std::wstring& s, position pos, color fgcolor, color bgcolor)
{
    assert (fgcolor != transparent);

    /*
    ** 逐个字符绘制而不是整块绘制
    ** 因为很难确定块的起点和终点(比如汉字可能中间被打断)
    */

    const wchar_t* cs = s.c_str();
    DWORD read;
    int i = 0;

    wchar_t wc;
    while ((wc = cs[i++]) != L'\0') {
        /* 一旦某个字符超出又边界立刻停止 */
        if (pos.x >= m_wndrect.right)
            break;

        if (checkascii(wc)) {
            char c = (char)wc;
            if (m_wndrect.pointin(pos)) {
                WORD attr = makecolor(fgcolor, bgcolor);
                if (bgcolor == transparent) {
                    ReadConsoleOutputAttribute(m_backbuf, &attr, 1, pos, &read);
                    chfgcolor(attr, fgcolor);
                }
                FillConsoleOutputAttribute(m_backbuf, attr, 1, pos, NULL);
                FillConsoleOutputCharacterA(m_backbuf, c, 1, pos, NULL);
            }
            pos.x++;
        }
        else {
            /* 整个宽字符都在窗口内才被绘制 */            
            if (m_wndrect.pointin(pos)) {
                position pos2(pos.x+1, pos.y);
                if (m_wndrect.pointin(pos2)) {
                    WORD attr[2];
                    attr[0] = attr[1] = makecolor(fgcolor, bgcolor);
                    if (bgcolor == transparent) {
                        ReadConsoleOutputAttribute(m_backbuf, attr, 2, pos, &read);
                        chfgcolor(attr[0], fgcolor);
                        chfgcolor(attr[1], fgcolor);
                    }
                    WriteConsoleOutputAttribute(m_backbuf, attr, 2, pos, NULL);
                    WriteConsoleOutputCharacterW(m_backbuf, &wc, 1, pos, NULL);
                }                
            }
            pos.x += 2;
        }
    }
}

void draw::showcursor(bool show)
{
    CONSOLE_CURSOR_INFO cci;
    cci.dwSize = 1;
    cci.bVisible = show ? TRUE : FALSE;
    SetConsoleCursorInfo(m_mainbuf, &cci);
    SetConsoleCursorInfo(m_backbuf, &cci);    
}

/*========================= input =========================*/

input::input()
{
    m_inputhandle = GetStdHandle(STD_INPUT_HANDLE);
    if (m_inputhandle == INVALID_HANDLE_VALUE)
        throw invalidhandle(m_inputhandle);

    SetConsoleMode(m_inputhandle, ENABLE_MOUSE_INPUT);    

    zeroarray(m_keysdown);
    zeroarray(m_lastkeysdown);
    zeroarray(m_buttonsdown);
    zeroarray(m_lastbuttonsdown);    
}

void input::update()
{
    memcpy(m_lastkeysdown, m_keysdown, sizeof(m_keysdown));
    memcpy(m_lastbuttonsdown, m_buttonsdown, sizeof(m_buttonsdown));        
    zeroarray(m_buttonsdblclick);
    
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