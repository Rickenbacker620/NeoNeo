#include "hook.h"
#include <ctre.hpp>
#include <format>

address_t HookAddress::GetAddress() const
{
    auto handle = GetModuleHandle(std::string{module}.c_str());
    address_t result{};
    if (function.data())
    {
        result = GetProcAddress(handle, std::string{function}.c_str());
    }
    else
    {
        result = handle;
    }
    return result + offset;
}

void Hook::Send(address_t dwDatabase)
{
    auto addr = GetHookAddress();
    address_t data_addr{};
    address_t context{};
    data_addr = *(dwDatabase + inst_.data.first);

    if (inst_.data.second.has_value()) {
        data_addr = data_addr + inst_.data.second.value();
    }
    std::cout << std::format("Data at{} || Context is", uintptr_t(addr)) << *data_addr << std::endl;
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
    auto address = addr_.GetAddress();

    while ((error = MH_CreateHook(address, trampoline, &original)) != MH_OK)
        if (error == MH_ERROR_ALREADY_CREATED)
        {
            MH_DisableHook(address);
            MH_RemoveHook(address);
            std::cout << "hook conflict, removing existing hook" << std::endl;
        }
        else
            std::cout << MH_StatusToString(error) << address << std::endl << this->trampoline << std::endl;
    // return ConsoleOutput(MH_StatusToString(error)), false;

    using HookThisType = Hook *;
    using HookSendType = void (Hook::*)(address_t);
    *reinterpret_cast<HookThisType *>(common_hook + this_offset) = this;
    *reinterpret_cast<HookSendType *>(common_hook + send_offset) = &Hook::Send;

    *(void **)(common_hook + original_offset) = original;
    memcpy(trampoline, common_hook, sizeof(common_hook));
    return MH_EnableHook(address) == MH_OK;
}

void Hook::Detach()
{
    auto address = GetHookAddress();
    MH_DisableHook(address);
    MH_RemoveHook(address);
}