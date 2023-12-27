#include <ctre.hpp>
#include "engine_base.h"

// ANCHOR stack snapshot at hook
/*
|  -20     |     edi      |
|  -1C     |     esi      |
|  -18     |     ebp      |
|  -14     |     esp      |
|  -10     |     ebx      |
|  -C      |     edx      |
|  -8      |     ecx      |
|  -4      |     eax      |
|dwDatabase|   ret addr   |
|   4      |     arg1     |
|   8      |     arg2     |
|   C      |     arg3     |
|   10     |     arg4     |
*/


void Engine::AttachHooks()
{
    std::cout << "Injecting ----" << name_ << std::endl;
    for (auto &hook : hooks_)
    {
        hook.Attach();
    }
}