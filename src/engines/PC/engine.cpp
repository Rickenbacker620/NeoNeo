#include "engine.h"

PCEngine::PCEngine() : Engine{"PC"}
{
    hooks_.push_back(Hook("kernel32.dll", "lstrlenA"));
    hooks_.push_back(Hook("kernel32.dll", "lstrlenA"));
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