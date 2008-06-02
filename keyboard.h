#pragma once

#include <conio.h>
#include <windows.h>

class keyboard {
public:
    keyboard();
    ~keyboard();
    void update();
    int key();

private:
    static DWORD WINAPI threadfunc(LPVOID param);
    int m_key;
};

inline
keyboard::keyboard()
{
    HANDLE hthread = CreateThread(NULL, 0, threadfunc, this, 0, NULL);
    CloseHandle(hthread);    
}

inline
keyboard::~keyboard()
{
    // NOTHING
}

inline 
void keyboard::update()
{
    m_key = -1;
}

inline 
int keyboard::key()
{
    return m_key;
}

inline
DWORD WINAPI keyboard::threadfunc(LPVOID param)
{
    keyboard& refthis = *(keyboard*)param;
    while (1)
        refthis.m_key = _getch();
    return 0;
}