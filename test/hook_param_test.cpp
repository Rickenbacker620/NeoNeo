#include "hook_param.h"
#include <gtest/gtest.h>

TEST(HookParam, HCodeParse)
{
    auto hook = "HW2@0:gdi32.dll:GetGlyphOutlineW"_hcode;
    EXPECT_EQ(hook.attribute, USING_UTF16);
    EXPECT_EQ(hook.address.function, "GetGlyphOutlineW");
    EXPECT_EQ(hook.text_offset.data.first, 8);
}

TEST(HookParam, GetHookAddress)
{
    HookParam hook;
    hook.address = {uintptr_t(0xFF), "kernel32.dll", ""};
    auto addr = hook.address.GetAddress();
    // if 64bit
    #ifdef _WIN64
    {
        EXPECT_EQ(addr, 0x00007FFF754300FFULL);
    }
    #endif
    // {
    //     EXPECT_EQ(addr, 0x00007FFF754300FFULL);
    // }
}