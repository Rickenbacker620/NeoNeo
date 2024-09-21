#include "engine.h"

PCEngine::PCEngine() : Engine{"PC"}
{
    // for (auto &h : kernel32_hooks_)
    // {
    //     hooks_.push_back(h);
    // }

    // for (auto &h : user32_hooks_)
    // {
    //     hooks_.push_back(h);
    // }

    hooks_.push_back(Hook("HW2@0:gdi32.dll:GetGlyphOutlineW"_hcode));
    // for (auto &h : gdi32_hooks_)
    // {
    //     hooks_.push_back(Hook("HW2@0:gdi32.dll:GetGlyphOutlineW"_hcode));
    // }

    // if (GetModuleHandle("OLEAUT32.dll"))
    // {
    //     for (auto &h : oleaut32_hooks_)
    //     {
    //         hooks_.push_back(h);
    //     }
    // }
}

Engine *PCEngine::Match()
{
    if (true)
    {
        return new PCEngine();
    }
}