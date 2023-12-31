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

size_t Hook::GetTextLength(address_t text_addr)
{
    size_t len{};
    if (hook_attr_ & USING_STRING) {
        return std::strlen(std::bit_cast<const char*>(text_addr));
    } else {
        return 1;
    }
    return len;
}

address_t Hook::GetTextAddress(address_t base)
{
    address_t address{};
    // get data address
    address = *(base + hook_off_.data.first);
    if (hook_off_.data.second.has_value())
    {
        address = *(address + hook_off_.data.second.value());
    }
    return address;
}

address_t Hook::GetTextContext(address_t base)
{
    address_t context{};

    context = *(base);
    if (hook_off_.context.has_value())
    {
        context = *(context + hook_off_.context.value().first);
    }
    return context;
}

void Hook::Send(address_t base)
{
    auto hook_addr = hook_addr_.GetAddress();
    auto text_address = GetTextAddress(base);
    auto text_context = GetTextContext(base);

    char buffer[1000];
    auto text_length = GetTextLength(text_address);
    std::memcpy(buffer, text_address, text_length);
}

bool Hook::Attach()
{
    BYTE common_hook[] = {
        0x9c, // pushfd
        0x60, // pushad
        0x9c, // pushfd ; Artikash 11/4/2018: not sure why pushfd happens twice. Anyway, after this a total of 0x28
              // bytes are pushed
        0x8d, 0x44, 0x24, 0x28, // lea eax,[esp+0x28]
        0x50,                   // push eax ; base
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
    auto address = hook_addr_.GetAddress();

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
    auto address = hook_addr_.GetAddress();
    MH_DisableHook(address);
    MH_RemoveHook(address);
}