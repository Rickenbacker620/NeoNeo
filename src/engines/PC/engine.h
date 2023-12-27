#pragma once
#include "../engine_base.h"

// constexpr std::array kernel32_hooks_{"HS4@0:kernel32.dll:lstrlenA"_hcode,
//                                      "HS8@0:kernel32.dll:lstrcpyA"_hcode,
//                                      "HS8@0:kernel32.dll:lstrcpynA"_hcode,
//                                      "HQ4@0:kernel32.dll:lstrlenW"_hcode,
//                                      "HQ8@0:kernel32.dll:lstrcpyW"_hcode,
//                                      "HQ8@0:kernel32.dll:lstrcpynW"_hcode,
//                                      "HSC@0:kernel32.dll:MultiByteToWideChar"_hcode,
//                                      "HQC@0:kernel32.dll:WideCharToMultiByte"_hcode,
//                                      "HSC@0:kernel32.dll:GetStringTypeA"_hcode,
//                                      "HSC@0:kernel32.dll:GetStringTypeExA"_hcode,
//                                      "HS8@0:kernel32.dll:FoldStringA"_hcode,
//                                      "HQ8@0:kernel32.dll:GetStringTypeW"_hcode,
//                                      "HQC@0:kernel32.dll:GetStringTypeExW"_hcode,
//                                      "HQ8@0:kernel32.dll:FoldStringW"_hcode};

class PCEngine : public Engine
{
  public:
    PCEngine();
    ~PCEngine();
    static Engine *Match();

    REGIST_ENGINE_MATCH
};