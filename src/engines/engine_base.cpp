#include <ctre.hpp>
#include "engine_base.h"



void Engine::AttachHooks()
{
    std::cout << "Injecting ----" << name_ << std::endl;
    for (auto &hook : hooks_)
    {
        hook.Attach();
        std::cout << hook.GetName() << "attached" << std::endl;
    }
}
Engine::~Engine()
{
    std::cout << "Detaching ----" << name_ << std::endl;
    for (auto &hook : hooks_)
    {
        hook.Detach();
    }
}
