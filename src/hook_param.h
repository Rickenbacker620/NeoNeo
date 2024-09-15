#pragma once
#include "common.h"
#include <string_view>
#include <regex>

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

// HookAddress structure
struct HookAddress
{
    address_t offset{};
    std::string module{};
    std::string function{};
    address_t GetAddress() const;
};

// TextOffsetHints structure
struct TextOffsetHints
{
    OffsetType data{};
    std::optional<OffsetType> context{};
    std::optional<intptr_t> length{};
};

// HookParam structure that includes the logic for parsing hcode
struct HookParam
{
    // How to parse the extracted text
    uint16_t attribute = 0;
    // Which address to hook
    HookAddress address{};
    // When entering the hook function, where to find the text
    TextOffsetHints text_offset{};

    HookParam() = default;

    // Constructor to parse hcode
    HookParam(std::string_view hcode);

    HookParam(uint16_t attr, HookAddress addr, TextOffsetHints inst)
        : attribute{attr}, address{addr}, text_offset{inst} {};

    static constexpr intptr_t offsets[]{0x0, 0x4, 0x8, 0xC, 0x10, 0x14, 0x18, 0x1C, 0x20};

    template <typename T = int> static std::optional<T> ParseHex(std::string_view hex_raw)
    {
        if (hex_raw.empty())
        {
            return std::nullopt;
        }
        T hex{};
        std::from_chars(hex_raw.data(), hex_raw.data() + hex_raw.size(), hex, 16);
        return hex;
    }

    // Function to parse the attribute
    static uint16_t ParseAttribute(std::string_view attribute);

    // Function to parse the address
    static HookAddress ParseAddress(std::string_view address);

    // Function to parse the instructor for complex case
    static TextOffsetHints ParseInstructor(std::string_view instructor, uint16_t attribute);

    // Function to parse the instructor for predefined simple cases
    static TextOffsetHints ParsePredefinedInstructor(std::string_view instructor, uint16_t attribute);
};


inline HookParam operator"" _hcode(const char *hcode, std::size_t)
{
    // Define the regex pattern for parsing the hcode
    std::regex re(R"(^\\?H([ABWHSQVM]N?)(.*)@(.*)$)");
    std::smatch match;

    std::string hcode_str{hcode};

    // Perform the regex match on the hcode
    if (std::regex_match(hcode_str, match, re)) {
        // Parse the components of the hcode
        auto hook_attr = HookParam::ParseAttribute(match[1].str());
        auto hook_inst = HookParam::ParsePredefinedInstructor(match[2].str(), hook_attr);
        auto hook_addr = HookParam::ParseAddress(match[3].str());

        // Return a HookParam instance with the parsed values
        return HookParam{hook_attr, hook_addr, hook_inst};
    }

    // Handle the case where the input does not match the expected pattern
    throw std::invalid_argument("Invalid hook code format");
}