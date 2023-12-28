#include <ctre.hpp>
#include "engine_base.h"



void Engine::AttachHooks()
{
    std::cout << "Injecting ----" << name_ << std::endl;
    for (auto &hook : hooks_)
    {
        hook.Attach();
    }
}