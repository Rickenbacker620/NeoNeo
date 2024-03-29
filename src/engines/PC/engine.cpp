#include "engine.h"

PCEngine::PCEngine() : Engine{"PC"}
{
    for (auto &h : kernel32_hooks_)
    {
        hooks_.push_back(h);
    }

    for (auto &h : user32_hooks_)
    {
        hooks_.push_back(h);
    }

    for (auto &h : gdi32_hooks_)
    {
        hooks_.push_back(h);
    }

    if (GetModuleHandle("OLEAUT32.dll"))
    {
        for (auto &h : oleaut32_hooks_)
        {
            hooks_.push_back(h);
        }
    }
}

Engine *PCEngine::Match()
{
    if (true)
    {
        return new PCEngine();
    }
}