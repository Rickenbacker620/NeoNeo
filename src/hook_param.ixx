module;
#include <Windows.h>
export module hook_param;

import <string_view>;
import <regex>;
import <optional>;
import <charconv>;

// Import local modules
import common;

export enum HookAttribute : uint16_t {
    USING_STRING = 1 << 0,
    BIG_ENDIAN = 1 << 1,
    USING_SPLIT = 1 << 2,
    FULL_STRING = 1 << 3,
    USING_UTF8 = 1 << 4,
    USING_UTF16 = 1 << 5,
    NO_CONTEXT = 1 << 6,
};

export using OffsetType = std::pair<intptr_t, std::optional<intptr_t>>;

export struct HookAddress {
    address_t offset{};
    std::string module{};
    std::string function{};

    address_t GetAddress() const {
        auto handle = GetModuleHandle(module.c_str());
        address_t result{};
        if (!function.empty()) {
            result = GetProcAddress(handle, function.c_str());
        } else {
            result = handle;
        }
        return result + offset;
    }
};

export struct TextOffsetHints {
    OffsetType data{};
    std::optional<OffsetType> context{};
    std::optional<intptr_t> length{};
};

export struct HookParam {
    uint16_t attribute = 0;
    HookAddress address{};
    TextOffsetHints text_offset{};

    static constexpr intptr_t offsets[]{0x0, 0x4, 0x8, 0xC, 0x10, 0x14, 0x18, 0x1C, 0x20};

    HookParam() = default;
    HookParam(std::string_view hcode);
    HookParam(uint16_t attr, HookAddress addr, TextOffsetHints inst)
        : attribute{attr}, address{addr}, text_offset{inst} {}

    template <typename T = int>
    static std::optional<T> ParseHex(std::string_view hex_raw) {
        if (hex_raw.empty()) {
            return std::nullopt;
        }
        T hex{};
        std::from_chars(hex_raw.data(), hex_raw.data() + hex_raw.size(), hex, 16);
        return hex;
    }

    static uint16_t ParseAttribute(std::string_view attribute) {
        uint16_t hattr{};
        switch (attribute[0]) {
        case L'A': hattr |= BIG_ENDIAN; break;
        case L'B': break;
        case L'W': hattr |= USING_UTF16; break;
        case L'S': hattr |= USING_STRING; break;
        case L'Q': hattr |= USING_STRING | USING_UTF16; break;
        case L'V': hattr |= USING_STRING | USING_UTF8; break;
        default: return 0;
        }
        if (attribute.length() == 2 && attribute[1] == 'N') {
            hattr |= NO_CONTEXT;
        }
        return hattr;
    }

    static HookAddress ParseAddress(std::string_view address) {
        std::regex re(R"(^([[:xdigit:]]+)(?::(\w+\.(?:exe|dll)))?(?::(\w+))?$)");
        std::smatch match;
        std::string address_str{address};

        if (std::regex_match(address_str, match, re)) {
            return HookAddress{
                ParseHex<uintptr_t>(match[1].str()).value(),
                match[2].str(),
                match[3].str()
            };
        }
        return HookAddress{};
    }

    static TextOffsetHints ParseInstructor(std::string_view instructor, uint16_t attribute) {
        std::regex re(R"(^(-?[[:xdigit:]]+)(?:\*(-?[[:xdigit:]]+))?(?::(-?[[:xdigit:]]+))?(?:\*(-?[[:xdigit:]]+))?$)");
        std::smatch match;
        std::string instructor_str{instructor};

        if (std::regex_match(instructor_str, match, re)) {
            OffsetType data{
                ParseHex(match[1].str()).value(),
                ParseHex(match[2].str())
            };

            if (attribute & USING_STRING && !data.second.has_value()) {
                data.second.emplace(0);
            }

            std::optional<OffsetType> context{};
            if (!match[3].str().empty()) {
                context = OffsetType{
                    ParseHex(match[3].str()).value(),
                    ParseHex(match[4].str())
                };
            }

            return TextOffsetHints{data, context};
        }
        return TextOffsetHints{};
    }

    static TextOffsetHints ParsePredefinedInstructor(std::string_view instructor, uint16_t attribute) {
        std::regex re(R"(^(\d)(\*)?(?::(\d))?$)");
        std::smatch match;
        std::string instructor_str{instructor};

        if (std::regex_match(instructor_str, match, re)) {
            auto argn = ParseHex(match[1].str()).value();
            OffsetType data{offsets[argn], std::nullopt};

            if (attribute & USING_STRING && !data.second.has_value()) {
                data.second.emplace(0);
            }

            std::optional<OffsetType> context{};
            if (!match[2].str().empty()) {
                context = OffsetType{0, std::nullopt};
            }

            std::optional<intptr_t> len_offset{};
            auto len_offset_index = ParseHex(match[3].str());
            if (len_offset_index.has_value()) {
                len_offset = offsets[len_offset_index.value()];
            }

            return TextOffsetHints{data, context, len_offset};
        }
        return TextOffsetHints{};
    }
};

HookParam::HookParam(std::string_view hcode) {
    std::regex re(R"(^\\?H([ABWHSQVM]N?)(.*)@(.*)$)");
    std::smatch match;
    std::string hcode_str{hcode};

    if (std::regex_match(hcode_str, match, re)) {
        attribute = ParseAttribute(match[1].str());
        text_offset = ParseInstructor(match[2].str(), attribute);
        address = ParseAddress(match[3].str());
    }
}

export inline HookParam operator"" _hcode(const char* hcode, std::size_t) {
    std::regex re(R"(^\\?H([ABWHSQVM]N?)(.*)@(.*)$)");
    std::smatch match;
    std::string hcode_str{hcode};

    if (std::regex_match(hcode_str, match, re)) {
        auto hook_attr = HookParam::ParseAttribute(match[1].str());
        auto hook_inst = HookParam::ParsePredefinedInstructor(match[2].str(), hook_attr);
        auto hook_addr = HookParam::ParseAddress(match[3].str());

        return HookParam{hook_attr, hook_addr, hook_inst};
    }

    throw std::invalid_argument("Invalid hook code format");
}
