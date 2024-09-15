#include "hook_param.h"
#include "common.h"
#include <regex>

HookParam::HookParam(std::string_view hcode)
{
    // Regular expression for HookParam format
    std::regex re(R"(^\\?H([ABWHSQVM]N?)(.*)@(.*)$)");
    std::smatch match;
    std::string hcode_str{hcode};

    if (std::regex_match(hcode_str, match, re)) {
        attribute = ParseAttribute(match[1].str());
        text_offset = ParseInstructor(match[2].str(), attribute);
        address = ParseAddress(match[3].str());
        auto c = 3;
    }
}

address_t HookAddress::GetAddress() const
{
    auto handle = GetModuleHandle(std::string{module}.c_str());
    address_t result{};
    if (!function.empty())
    {
        result = GetProcAddress(handle, std::string{function}.c_str());
    }
    else
    {
        result = handle;
    }
    return result + offset;
}

uint16_t HookParam::ParseAttribute(std::string_view attribute)
{
    uint16_t hattr{};
    switch (attribute[0])
    {
    case L'A':
        hattr |= BIG_ENDIAN;
        break;
    case L'B':
        break;
    case L'W':
        hattr |= USING_UTF16;
        break;
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

HookAddress HookParam::ParseAddress(std::string_view address)
{
    // Regular expression for address parsing
    std::regex re(R"(^([[:xdigit:]]+)(?::(\w+\.(?:exe|dll)))?(?::(\w+))?$)");
    std::smatch match;
    std::string address_str{address};

    if (std::regex_match(address_str, match, re)) {
        HookAddress result {
            ParseHex<uintptr_t>(match[1].str()).value(),
            match[2].str(),
            match[3].str()
        };
        return result;
    }
    return HookAddress{};
}

TextOffsetHints HookParam::ParseInstructor(std::string_view instructor, uint16_t attribute)
{
    // Regular expression for instructor parsing
    std::regex re(R"(^(-?[[:xdigit:]]+)(?:\*(-?[[:xdigit:]]+))?(?::(-?[[:xdigit:]]+))?(?:\*(-?[[:xdigit:]]+))?$)");
    std::smatch match;
    std::string instructor_str{instructor};

    if (std::regex_match(instructor_str, match, re)) {
        OffsetType data{
            ParseHex(match[1].str()).value(),
            ParseHex(match[2].str())
        };

        if (attribute & USING_STRING && !data.second.has_value())
        {
            data.second.emplace(0);
        }

        std::optional<OffsetType> context{};
        if (!match[3].str().empty())
        {
            context = {
                ParseHex(match[3].str()).value(),
                ParseHex(match[4].str())
            };
        }
        else
        {
            context = std::nullopt;
        }

        return TextOffsetHints{data, context};
    }
    return TextOffsetHints{};
}

TextOffsetHints HookParam::ParsePredefinedInstructor(std::string_view instructor, uint16_t attribute)
{
    // Regular expression for predefined instructor parsing
    std::regex re(R"(^(\d)(\*)?(?::(\d))?$)");
    std::smatch match;
    std::string instructor_str{instructor};

    if (std::regex_match(instructor_str, match, re)) {
        auto argn = ParseHex(match[1].str()).value();
        OffsetType data{offsets[argn], std::nullopt};

        if (attribute & USING_STRING && !data.second.has_value())
        {
            data.second.emplace(0);
        }

        std::optional<OffsetType> context{};
        if (!match[2].str().empty())
        {
            context = {0, std::nullopt};
        }
        else
        {
            context = std::nullopt;
        }

        auto len_offset_index = ParseHex(match[3].str());
        std::optional<intptr_t> len_offset{};
        if (len_offset_index.has_value())
        {
            len_offset = offsets[len_offset_index.value()];
        }
        else
        {
            len_offset = std::nullopt;
        }

        return TextOffsetHints{.data = data, .context = context, .length = len_offset};
    }
    return TextOffsetHints{};
}
