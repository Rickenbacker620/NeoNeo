#pragma once
#include <ctre.hpp>

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

static constexpr intptr_t offsets[] {
    0x0,
    0x4,
    0x8,
    0xC,
    0x10,
    0x14,
    0x18,
    0x1C,
    0x20
};

enum HookAttribute : uint16_t
{
    // Data types
    USING_STRING = 1 << 0, // Indicates the use of string (char* or wchar_t*) with length

    // Data configurations
    BIG_ENDIAN = 1 << 1,  // Indicates the data type is in big-endian format
    USING_SPLIT = 1 << 2, // Use ctx2 or not for split
    FULL_STRING = 1 << 3, // Entire string data is considered

    USING_UTF8 = 1 << 4,  // Indicates the use of UTF-8 encoding
    USING_UTF16 = 1 << 5, // Indicates the use of Unicode (wchar_t or wchar_t*)

    NO_CONTEXT = 1 << 6, // Indicates the use of Unicode (wchar_t or wchar_t*)
};

using OffsetType = std::pair<intptr_t, std::optional<intptr_t>>;

// Offset hints about where to find the text
struct HookOffsetHints
{
    OffsetType data{};
    std::optional<OffsetType> context{};
    std::optional<intptr_t> length;
};

// Address where to attach the hook
struct HookAddress
{
    address_t offset{};
    std::string_view module{};
    std::string_view function{};
    address_t GetAddress() const;
};

namespace hookcode
{

template <typename T = int> constexpr std::optional<T> ParseHex(std::string_view hex_raw)
{
    if (!hex_raw.data())
    {
        return std::nullopt;
    }
    T hex{};
    std::from_chars(hex_raw.data(), hex_raw.data() + hex_raw.size(), hex, 16);
    return hex;
}

constexpr uint16_t ParseAttribute(std::string_view attribute)
{
    uint16_t hattr{};
    switch (attribute[0])
    {
    // chars
    case L'A':
        hattr |= BIG_ENDIAN;
        break;
    case L'B':
        break;
    case L'W':
        hattr |= USING_UTF16;
        break;
    // strings
    case L'S':
        hattr |= USING_STRING;
        break;
    case L'Q':
        hattr |= USING_STRING | USING_UTF16;
        break;
    case L'V':
        hattr |= USING_STRING | USING_UTF8;
        break;
    default:
        return 0;
    }
    if (attribute.length() == 2 && attribute[1] == 'N')
    {
        hattr |= NO_CONTEXT;
    }
    return hattr;
}

constexpr HookOffsetHints ParseInstructor(std::string_view instructor)
{
    auto result =
        ctre::match<R"(^(-?[[:xdigit:]]+)(?:\*(-?[[:xdigit:]]+))?(?::(-?[[:xdigit:]]+))?(?:\*(-?[[:xdigit:]]+))?$)">(
            instructor);
    OffsetType data{ParseHex(result.get<1>()).value(), ParseHex(result.get<2>())};
    std::optional<OffsetType> context{};
    if (result.get<3>().data())
    {
        context = {ParseHex(result.get<3>()).value(), ParseHex(result.get<4>())};
    }
    else
    {
        context = {std::nullopt};
    }

    return HookOffsetHints{data, context};
}

constexpr HookOffsetHints ParsePredefinedInstructor(std::string_view instructor)
{
    auto result = ctre::match<R"(^(\d)(?::(\d))?$)">(instructor);
    auto argn = ParseHex(result.get<1>()).value();
    OffsetType data{offsets[argn], std::nullopt};

    auto len_offset_index = ParseHex(result.get<2>());
    std::optional<intptr_t> len_offset{};
    if (len_offset_index.has_value()) {
        len_offset = offsets[len_offset_index.value()];
    }else {
        len_offset = std::nullopt;
    }

    return HookOffsetHints{.data=data, .context=std::nullopt, .length=len_offset};
}

constexpr HookAddress ParseAddress(std::string_view address)
{
    auto result = ctre::match<R"(^([[:xdigit:]]+)(?::(\w+\.(?:exe|dll)))?(?::(\w+))?$)">(address);
    return HookAddress{ParseHex<uintptr_t>(result.get<1>()).value(), result.get<2>(), result.get<3>()};
}
} // namespace hookcode

class Hook
{
  private:
    uint16_t hook_attr_{};
    HookAddress hook_addr_{};
    HookOffsetHints hook_off_{};
    BYTE trampoline[40]{};

    size_t GetTextLength(address_t text_addr);
    address_t GetTextAddress(address_t base);
    address_t GetTextContext(address_t base);

  public:
    constexpr Hook(uint16_t attr, HookAddress addr, HookOffsetHints inst)
        : hook_attr_{attr}, hook_addr_{addr}, hook_off_{inst} {};
    constexpr Hook(std::string_view hcode)
    {
        auto result = ctre::match<R"(^\\?H([ABWHSQVM]N?)(.*)@(.*)$)">(hcode);
        hook_attr_ = hookcode::ParseAttribute(result.get<1>());
        hook_off_ = hookcode::ParseInstructor(result.get<2>());
        hook_addr_ = hookcode::ParseAddress(result.get<3>());
    };
    consteval Hook(std::string_view hcode, uint32_t len){

    };
    void Send(address_t dwDatabase);
    bool Attach();
    void Detach();
};

consteval Hook operator"" _hcode(const char *hcode, std::size_t)
{
    auto result = ctre::match<R"(^\\?H([ABWHSQVM]N?)(.*)@(.*)$)">(hcode);
    auto hook_attr = hookcode::ParseAttribute(result.get<1>());
    auto hook_inst = hookcode::ParsePredefinedInstructor(result.get<2>());
    auto hook_addr = hookcode::ParseAddress(result.get<3>());
    return Hook{hook_attr, hook_addr, hook_inst};
}