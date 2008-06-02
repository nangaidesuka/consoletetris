#pragma once

#include <windows.h>

class fpser {
public:
    fpser(int fps=30);
    void update();
    int fps() const;
    int mspf() const;

private:
    int m_fps;
    int m_mspf;
    int m_lasttick;
};

inline
fpser::fpser(int fps)
{
    m_fps = fps;
    m_mspf = 1000 / m_fps;
    m_lasttick = (int)GetTickCount();
}

inline
void fpser::update()
{
    int pastticks = (int)GetTickCount() - m_lasttick;
    if (pastticks < m_mspf) 
        Sleep(m_mspf - pastticks);
    m_lasttick = GetTickCount();
}

inline 
int fpser::fps() const
{
    return m_fps;
}

inline 
int fpser::mspf() const
{
    return m_mspf;
}