#include "hook.h"
#include "dialogue.h"
#include "server.h"
#include <ctre.hpp>
#include <fmt/core.h>
#include <MinHook.h>
#include <Windows.h>

size_t Hook::GetTextLength(address_t base, address_t text_addr) const
{
    auto &attribute_ = this->param_.attribute;
    auto &text_offset_ = this->param_.text_offset;
    // lambda to get the length of a null-terminated string
    auto GetNullTerminatedLength = [&](address_t addr) -> size_t {
        return attribute_ & USING_UTF16 ? std::wcslen(std::bit_cast<const wchar_t *>(addr)) * 2
                                        : std::strlen(std::bit_cast<const char *>(addr));
    };

    if (attribute_ & USING_STRING)
    {
        // if length offset is specified, use it
        if (text_offset_.length.has_value())
        {
            size_t length = *(static_cast<int *>(base + text_offset_.length.value()));
            //  (base + text_offset_.length.value());
            // in some cases, -1 is used to indicate null-terminated string
            if (length == -1)
            {
                return GetNullTerminatedLength(text_addr);
            }
            else
            {
                // if using utf16, double the length, otherwise use the length as is
                return attribute_ & USING_UTF16 ? length * 2 : length;
            }
        }
        // if length offset is not specified, it is a null-terminated string
        else
        {
            return GetNullTerminatedLength(text_addr);
        }
    }
    else
    {
        if (attribute_ & USING_UTF16)
        {
            return 2;
        }
        else
        {
            if (attribute_ & BIG_ENDIAN)
            {
                text_addr >>= 8;
            }
            // FIXME need to fix hardcoded 932 codepage && logic not correct
            // return !!IsDBCSLeadByteEx(932, text_addr & 0xFF) + 1;
            // return !!IsDBCSLeadByteEx(932, text_addr & 0xFF) + 1;
            return 1;
        }
    }
}

address_t Hook::GetTextAddress(address_t base) const
{
    auto &text_offset_ = this->param_.text_offset;

    address_t address{};

    // for character, data is right on stack, add the 1st offset to get a stack address
    address = base + text_offset_.data.first;

    // if there is a 2nd offset, the value on stack is not the data, it is an address
    // for strings, the 2nd offset must exist and is 0 by default
    if (text_offset_.data.second.has_value())
    {
        address = address.GetAsAddress() + text_offset_.data.second.value();
    }
    return address;
}

address_t Hook::GetTextContext(address_t base) const
{
    auto &text_offset_ = this->param_.text_offset;

    address_t context{};

    context = base.GetAsAddress();
    if (text_offset_.context.has_value())
    {
        context = (context.GetAsAddress() + text_offset_.context.value().first).GetAsAddress();
    }
    return context;
}

std::string Hook::GetName() const
{
    auto &address_ = this->param_.address;
    std::string name = fmt::format("{:p}", (void *)address_.offset);
    if (!address_.module.empty())
    {
        name += fmt::format(":{:s}", address_.module);
    }
    if (!address_.function.empty())
    {
        name += fmt::format("::{:s}", address_.function);
    }
    return name;
}

void Hook::Send(address_t base)
{
    auto getBufferHex = [&](std::vector<char> &buffer) -> std::string {
        std::string strDebug;
        for (size_t i = 0; i < buffer.size(); i++)
        {
            strDebug += fmt::format("{:02x} ", (unsigned char)buffer[i]);
        }
        return strDebug;
    };

    auto &attribute_ = this->param_.attribute;
    auto &text_offset_ = this->param_.text_offset;
    auto &address_ = this->param_.address;

    auto hook_addr = address_.GetAddress();

    auto text_address = GetTextAddress(base);

    // skip if textaddress is invalid
    if (!text_address)
    {
        return;
    }

    auto text_context = GetTextContext(base);

    auto text_length = GetTextLength(base, text_address);

    std::string encoding = "";

    if (attribute_ & USING_UTF8)
    {
        encoding = "UTF-8";
    }
    else if (attribute_ & USING_UTF16)
    {
        encoding = "UTF-16LE";
    }
    else
    {
        encoding = "Shift-JIS";
    }

    auto dialogue_id = fmt::format("{:s}\x02{:d}\x02{:p}\x02{:p}\x02{:p}\x02{:s}",
                                   this->GetName(),               // hookname
                                   1234,                          // process id
                                   (void *)address_.GetAddress(), // hook address
                                   (void *)text_context,          // text context
                                   (void *)0,                     // text context2
                                   encoding                       // encoding
    );

    if (text_length == 2)
    {
        DialoguePool::Push(dialogue_id, encoding, *(static_cast<char *>(text_address)));
        DialoguePool::Push(dialogue_id, encoding, *(static_cast<char *>(text_address + 1)));
    }
    else
    {
        char temp[1000];
        std::memcpy(temp, text_address, text_length);
        for (size_t i = 0; i < text_length; i++)
        {
            DialoguePool::Push(dialogue_id, encoding, temp[i]);
        }
    }

    // if (address_.function == "GetGlyphOutlineW")
    // {
    //     //  && text_context == address_t{0x67EA21F4}
    //     // std::cout << text_context << " " << address_t{0x678021F4} << (text_context == address_t{0x678021F4})
    //     //           << std::endl;
    //     std::cout << "text_context" << text_context << "|" << "text length: " << text_length << "|"
    //               << "buffer_index: " << buffer_index << "|" << "buffer: " << getBufferHex(buffer) << std::endl;
    // }

    // // if buffer is full or null-terminated string is reached
    // if (buffer.size() >= 1000 || buffer.back() == 0)
    // {
    //     // clang-format off
    //     // massage use 0x02 as sep
    //     auto str = fmt::format(
    //         "{:s}\x02{:d}\x02{:p}\x02{:p}\x02{:p}\x02{}\x03",
    //         this->GetName(),                // hookname
    //         1234,                           // process id
    //         (void*)address_.GetAddress(),          // hook address
    //         (void*)text_context,                   // text context
    //         (void*)0,                              // text context2
    //         buffer.data()                          // text content
    //     );
    //     // clang-format on

    //     g_server.Send(str);
    // }

    /*
    Msg format
    {
        hook name
        (custom hook: userhookn)
        (pre-defined system hook: offset:module:function)
        (address only hook: address)
        process id
        hook address
        text context
        text context2(optional)
        text content
    }
    use 0x02 as seperator
    */

    // // clang-format off
    // // massage use 0x02 as sep
    // auto str = fmt::format(
    //     "{:s}\x02{:d}\x02{:p}\x02{:p}\x02{:p}\x02{}\x03",
    //     this->GetName(),                // hookname
    //     1234,                           // process id
    //     (void*)address_.GetAddress(),          // hook address
    //     (void*)text_context,                   // text context
    //     (void*)0,                              // text context2
    //     buffer                          // text content
    // );
    // // clang-format on

    // // print buffer as hex for debug
    // std::string strDebug;
    // for (int i = 0; i < text_length; i++)
    // {
    //     strDebug += fmt::format("{:02x} ", (unsigned char)buffer[i]);
    // }

    // if (address_.function == "GetGlyphOutlineW")
    // {
    //     //  && text_context == address_t{0x67EA21F4}
    //     // std::cout << text_context << " " << address_t{0x678021F4} << (text_context == address_t{0x678021F4})
    //     //           << std::endl;
    //     std::cout << strDebug;
    // }

    // g_server.Send(str);
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
    auto &address_ = this->param_.address;

    VirtualProtect((LPVOID)(0x75BE0410), 10, PAGE_EXECUTE_READWRITE, DUMMY);
    VirtualProtect((LPVOID)(&trampoline), 40, PAGE_EXECUTE_READWRITE, DUMMY);

    void *original;
    MH_STATUS error;
    auto address = address_.GetAddress();

    while ((error = MH_CreateHook(address, trampoline, &original)) != MH_OK)
        if (error == MH_ERROR_ALREADY_CREATED)
        {
            MH_DisableHook(address);
            MH_RemoveHook(address);
            std::cout << "hook conflict, removing existing hook" << std::endl;
        }
        else
            std::cout << MH_StatusToString(error) << address << std::endl << this->trampoline << std::endl;

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
    auto& address_ = this->param_.address;
    auto address = address_.GetAddress();
    MH_DisableHook(address);
    MH_RemoveHook(address);
}