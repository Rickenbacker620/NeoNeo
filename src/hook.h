#pragma once
#include <ctre.hpp>

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

using OffsetType = std::pair<int, std::optional<int>>;

struct HookInstructor
{
    OffsetType data{};
    std::optional<OffsetType> context{};
};

struct HookAddress
{
    uintptr_t offset{};
    std::string_view module{};
    std::string_view function{};
    void *GetAddress() const;
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

constexpr HookInstructor ParseInstructor(std::string_view instructor)
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
    return HookInstructor{data, context};
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
    uint16_t attr_{};
    HookAddress addr_{};

    HookInstructor inst_{};

    BYTE trampoline[40]{};
    void *GetHookAddress()
    {
        return addr_.GetAddress();
    };

  public:
    constexpr Hook(HookAddress addr, HookInstructor inst) : addr_{addr}, inst_{inst} {};
    constexpr Hook(std::string_view hcode)
    {
        auto result = ctre::match<R"(^\\?H([ABWHSQVM]N?)(.*)@(.*)$)">(hcode);
        attr_ = hookcode::ParseAttribute(result.get<1>());
        inst_ = hookcode::ParseInstructor(result.get<2>());
        addr_ = hookcode::ParseAddress(result.get<3>());
    };
    void Send(uintptr_t dwDatabase);
    bool Attach();
};

consteval Hook operator"" _hcode(const char *hcode, std::size_t)
{
    return Hook{hcode};
}