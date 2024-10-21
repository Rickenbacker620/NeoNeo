#include <ctre.hpp>
#include "engine_base.h"
#include <vector>

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

void Engine::StartListenController()
{
    controller_->Start();
}