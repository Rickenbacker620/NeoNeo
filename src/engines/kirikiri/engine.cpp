#include "engine.h"

KiriKiriEngine::KiriKiriEngine() : Engine{"KiriKiri"}
{
}

bool KiriKiriEngine::findKiriKiriHook(DWORD fun, DWORD size, DWORD pt, DWORD flag)
{
    // enum : DWORD
    // {
    //     // jichi 10/20/2014: mov ebp,esp, sub esp,*
    //     kirikiri1_sig = 0xec8b55,

    //     // jichi 10/20/2014:
    //     // 00e01542   53               push ebx
    //     // 00e01543   56               push esi
    //     // 00e01544   57               push edi
    //     kirikiri2_sig = 0x575653
    // };
    // enum : DWORD
    // {
    //     StartAddress = 0x1000
    // };
    // enum : DWORD
    // {
    //     StartRange = 0x6000,
    //     StopRange = 0x8000
    // }; // jichi 10/20/2014: ITH original pattern range

    // // jichi 10/20/2014: The KiriKiri patterns exist in multiple places of the game.
    // // enum : DWORD { StartRange = 0x8000, StopRange = 0x9000 }; // jichi 10/20/2014: change to a different range

    // // WCHAR str[0x40];
    // DWORD sig = flag ? kirikiri2_sig : kirikiri1_sig;
    // DWORD t = 0;
    // for (DWORD i = StartAddress; i < size - 4; i++)
    //     if (*(WORD *)(pt + i) == 0x15ff)
    //     { // jichi 10/20/2014: call dword ptr ds
    //         DWORD addr = *(DWORD *)(pt + i + 2);

    //         // jichi 10/20/2014: There are multiple function calls. The flag+1 one is selected.
    //         // i.e. KiriKiri1: The first call to GetGlyphOutlineW is selected
    //         //      KiriKiri2: The second call to GetTextExtentPoint32W is selected
    //         if (addr >= pt && addr <= pt + size - 4 && *(DWORD *)addr == fun)
    //             t++;
    //         if (t == flag + 1) // We find call to GetGlyphOutlineW or GetTextExtentPoint32W.
    //             // swprintf(str, L"CALL addr:0x%.8X",i+pt);
    //             // ConsoleOutput(str);
    //             for (DWORD j = i; j > i - StartAddress; j--)
    //                 if (((*(DWORD *)(pt + j)) & 0xffffff) == sig)
    //                 {
    //                     if (flag)
    //                     {          // We find the function entry. flag indicate 2 hooks.
    //                         t = 0; // KiriKiri2, we need to find call to this function.
    //                         for (DWORD k = j + StartRange; k < j + StopRange; k++) // Empirical range.
    //                             if (*(BYTE *)(pt + k) == 0xe8)
    //                             {
    //                                 if (k + 5 + *(DWORD *)(pt + k + 1) == j)
    //                                     t++;
    //                                 if (t == 2)
    //                                 {
    //                                     // for (k+=pt+0x14; *(WORD*)(k)!=0xC483;k++);
    //                                     // swprintf(str, L"Hook addr: 0x%.8X",pt+k);
    //                                     // ConsoleOutput(str);
    //                                     HookParam hp = {};
    //                                     hp.address = pt + k + 0x14;
    //                                     hp.offset = -0x14;
    //                                     hp.index = -0x2;
    //                                     hp.split = -0xc;
    //                                     hp.length_offset = 1;
    //                                     hp.type = USING_UNICODE | NO_CONTEXT | USING_SPLIT | DATA_INDIRECT;
    //                                     ConsoleOutput("vnreng: INSERT KiriKiri2");
    //                                     NewHook(hp, "KiriKiri2");
    //                                     return true;
    //                                 }
    //                             }
    //                     }
    //                     else
    //                     {
    //                         // swprintf(str, L"Hook addr: 0x%.8X",pt+j);
    //                         // ConsoleOutput(str);
    //                         HookParam hp = {};
    //                         hp.address = (DWORD)pt + j;
    //                         hp.offset = -0x8;
    //                         hp.index = 0x14;
    //                         hp.split = -0x8;
    //                         hp.length_offset = 1;
    //                         hp.type = USING_UNICODE | DATA_INDIRECT | USING_SPLIT | SPLIT_INDIRECT;
    //                         ConsoleOutput("vnreng: INSERT KiriKiri1");
    //                         NewHook(hp, "KiriKiri1");
    //                         return true;
    //                     }
    //                     return false;
    //                 }
    //         // ConsoleOutput("vnreng:KiriKiri: FAILED to find function entry");
    //     }
    // if (flag)
    //     ConsoleOutput("vnreng:KiriKiri2: failed");
    // else
    //     ConsoleOutput("vnreng:KiriKiri1: failed");
    return false;
}
