#include "engine.h"
#include "../../hook.h"

PCEngine::PCEngine() : Engine{"PC"}
{
    constexpr auto a =  "HS4@0:kernel32.dll:lstrlenA"_hcode;
    hooks_.push_back(a);
    // for (auto& k32hook : kernel32_hooks_) {
    //     hooks_.push_back(k32hook);
    // }
}

PCEngine::~PCEngine()
{
}

Engine *PCEngine::Match()
{
    if (true)
    {
        return new PCEngine();
    }
}