#pragma once

#include <windows.h>

class fpscontrol {
public:
    fpscontrol(int fps=30);
    void wait();
    int fps() const;
    int mspf() const;

private:
    int m_fps;
    int m_mspf;
    int m_lasttick;
};

inline
fpscontrol::fpscontrol(int fps)
{
    m_fps = fps;
    m_mspf = 1000 / m_fps;
    m_lasttick = (int)GetTickCount();
}

inline
void fpscontrol::wait()
{
    int pastticks = (int)GetTickCount() - m_lasttick;
    if (pastticks < m_mspf) 
        Sleep(m_mspf - pastticks);
    m_lasttick = GetTickCount();
}

inline 
int fpscontrol::fps() const
{
    return m_fps;
}

inline 
int fpscontrol::mspf() const
{
    return m_mspf;
}