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

Hook::Hook(const char *module, const char *function, const uintptr_t offset)
    : module_{module}, function_{function}, offset_{offset}, name_{GetHookName()}, address_{GetHookAddress()}
{
}

void Hook::Send(uintptr_t dwDatabase)
{
    std::cout << name_ << std::hex << dwDatabase << "Got Message" << std::endl;
}

void *Hook::GetHookAddress()
{
    auto handle = GetModuleHandle(module_);
    uintptr_t result = 0;
    if (function_)
    {
        result = reinterpret_cast<uintptr_t>(GetProcAddress(handle, function_));
    }
    else
    {
        result = reinterpret_cast<uintptr_t>(handle);
    }
    return reinterpret_cast<void *>(result + offset_);
}

const std::string Hook::GetHookName()
{
    std::string name;
    name.append(function_).append(" at ").append(module_);
    return name;
}

bool Hook::Attach()
{
    BYTE common_hook[] = {
        0x9c, // pushfd
        0x60, // pushad
        0x9c, // pushfd ; Artikash 11/4/2018: not sure why pushfd happens twice. Anyway, after this a total of 0x28
              // bytes are pushed
        0x8d, 0x44, 0x24, 0x28, // lea eax,[esp+0x28]
        0x50,                   // push eax ; dwDatabase
        0xb9, 0, 0, 0, 0,       // mov ecx,@this
        0xbb, 0, 0, 0, 0,       // mov ebx,@TextHook::Send
        0xff, 0xd3,             // call ebx
        0x9d,                   // popfd
        0x61,                   // popad
        0x9d,                   // popfd
        0x68, 0, 0, 0, 0,       // push @original
        0xc3                    // ret ; basically absolute jmp to @original
    };
    int this_offset = 9, send_offset = 14, original_offset = 24;

    VirtualProtect((LPVOID)(0x75BE0410), 10, PAGE_EXECUTE_READWRITE, DUMMY);
    VirtualProtect((LPVOID)(&trampoline), 40, PAGE_EXECUTE_READWRITE, DUMMY);

    void *original;
    MH_STATUS error;
    while ((error = MH_CreateHook(address_, trampoline, &original)) != MH_OK)
        if (error == MH_ERROR_ALREADY_CREATED)
            // RemoveHook((uintptr_t)address_);
            std::cout << "error1" << std::endl;
        else
            std::cout << MH_StatusToString(error) << address_ << std::endl << this->trampoline << std::endl;
    // return ConsoleOutput(MH_StatusToString(error)), false;

    using HookThisType = Hook *;
    using HookSendType = void (Hook::*)(uintptr_t);
    *reinterpret_cast<HookThisType *>(common_hook + this_offset) = this;
    *reinterpret_cast<HookSendType *>(common_hook + send_offset) = &Hook::Send;

    *(void **)(common_hook + original_offset) = original;
    memcpy(trampoline, common_hook, sizeof(common_hook));
    return MH_EnableHook(address_) == MH_OK;
}

void Engine::AttachHooks()
{
    std::cout << "Injecting ----" << name_ << std::endl;
    for (auto &hook : hooks_)
    {
        hook.Attach();
    }
}