#pragma once

// texthook/const.h
// 8/23/2013 jichi
// Branch: ITH/common.h, rev 128

enum
{
    STRING = 12,
    MESSAGE_SIZE = 500,
    PIPE_BUFFER_SIZE = 50000,
    SHIFT_JIS = 932,
    MAX_MODULE_SIZE = 120,
    PATTERN_SIZE = 30,
    HOOK_NAME_SIZE = 60,
    FIXED_SPLIT_VALUE = 0x10001
};
enum WildcardByte
{
    XX = 0x11
};

enum HostCommandType
{
    HOST_COMMAND_NEW_HOOK,
    HOST_COMMAND_REMOVE_HOOK,
    HOST_COMMAND_FIND_HOOK,
    HOST_COMMAND_MODIFY_HOOK,
    HOST_COMMAND_HIJACK_PROCESS,
    HOST_COMMAND_DETACH
};

enum HostNotificationType
{
    HOST_NOTIFICATION_TEXT,
    HOST_NOTIFICATION_NEWHOOK,
    HOST_NOTIFICATION_FOUND_HOOK,
    HOST_NOTIFICATION_RMVHOOK
};

enum HookParamType : unsigned int
{
    // Data types
    USING_STRING = 1 << 0,  // Indicates the use of string (char* or wchar_t*) with length
    USING_UNICODE = 1 << 1, // Indicates the use of Unicode (wchar_t or wchar_t*)

    // Data configurations
    BIG_ENDIAN = 1 << 2,     // Indicates the data type is in big-endian format
    DATA_INDIRECT = 1 << 3,  // Indicates indirect data usage
    USING_SPLIT = 1 << 4,    // Use ctx2 or not for split
    SPLIT_INDIRECT = 1 << 5, // Indirect split usage
    FULL_STRING = 1 << 13,   // Entire string data is considered
    USING_UTF8 = 1 << 8,     // Indicates the use of UTF-8 encoding

    // Address-related flags
    MODULE_OFFSET = 1 << 6,   // Address is relative to module
    FUNCTION_OFFSET = 1 << 7, // Address is relative to function

    // Miscellaneous options
    NO_CONTEXT = 1 << 9,       // No context is required
    HOOK_EMPTY = 1 << 10,      // Empty hook
    FIXING_SPLIT = 1 << 11,    // Fixing split
    DIRECT_READ = 1 << 12,     // /R read code instead of classic /H hook code
    HEX_DUMP = 1 << 14,        // Perform hex dump
    HOOK_ENGINE = 1 << 15,     // Use hook engine
    HOOK_ADDITIONAL = 1 << 16, // Additional hook
    KNOWN_UNSTABLE = 1 << 17,  // Indicates known instability
};
