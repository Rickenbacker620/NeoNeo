#pragma once
#include "../engine_base.h"

class PCEngine : public Engine
{
  private:
    constexpr static Hook kernel32_hooks_[]{
        // clang-format off
        "HS1@0:kernel32.dll:lstrlenA"_hcode,
        // lstrlenA(LPCSTR lpString)
        "HS2@0:kernel32.dll:lstrcpyA"_hcode,
        // lstrcpyA(LPSTR lpString1, LPCSTR lpString2)
        "HS2@0:kernel32.dll:lstrcpynA"_hcode,
        // lstrcpynA(LPSTR lpString1, LPCSTR lpString2, int iMaxLength)
        "HQ1@0:kernel32.dll:lstrlenW"_hcode,
        // lstrlenW(LPCWSTR lpString)
        "HQ2@0:kernel32.dll:lstrcpyW"_hcode,
        // lstrcpyW(LPWSTR lpString1, LPCWSTR lpString2)
        "HQ2@0:kernel32.dll:lstrcpynW"_hcode,
        // lstrcpynW(LPWSTR lpString1, LPCWSTR lpString2, int iMaxLength)
        "HS3:4@0:kernel32.dll:MultiByteToWideChar"_hcode,
        // MultiByteToWideChar(UINT CodePage, DWORD dwFlags, LPCSTR lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar)
        "HQ3:4@0:kernel32.dll:WideCharToMultiByte"_hcode,
        // WideCharToMultiByte(UINT CodePage, DWORD dwFlags, LPCWSTR lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte, LPCSTR lpDefaultChar, LPBOOL lpUsedDefaultChar)
        "HS3:4@0:kernel32.dll:GetStringTypeA"_hcode,
        // GetStringTypeA(LCID Locale, DWORD dwInfoType, LPCSTR lpSrcStr, int cchSrc, LPWORD lpCharType)
        "HS3:4@0:kernel32.dll:GetStringTypeExA"_hcode,
        // GetStringTypeExA(LCID Locale, DWORD dwInfoType, LPCSTR lpSrcStr, int cchSrc, LPWORD lpCharType)
        "HS2:3@0:kernel32.dll:FoldStringA"_hcode,
        // FoldStringA(DWORD dwMapFlags, LPCSTR lpSrcStr, int cchSrc, LPSTR lpDestStr, int cchDest)
        "HQ2:3@0:kernel32.dll:GetStringTypeW"_hcode,
        // GetStringTypeW(DWORD dwInfoType, LPCWSTR lpSrcStr, int cchSrc, LPWORD lpCharType)
        "HQ3:4@0:kernel32.dll:GetStringTypeExW"_hcode,
        // GetStringTypeExW(LPCWSTR lpLocaleName, DWORD dwInfoType, LPCWSTR lpSrcStr, int cchSrc, LPWORD lpCharType)
        "HQ2:3@0:kernel32.dll:FoldStringW"_hcode,
        // FoldStringW(DWORD dwMapFlags, LPCWSTR lpSrcStr, int cchSrc, LPWSTR lpDestStr, int cchDest)
    };

    constexpr static Hook user32_hooks_[] = {
        "HA1*@0:user32.dll:CharNextA"_hcode,
        // CharNextA(LPCTSTR lpsz)
        "HW1*@0:user32.dll:CharNextW"_hcode,
        // CharNextW(LPCWSTR lpsz)
        "HA1*@0:user32.dll:CharPrevA"_hcode,
        // CharPrevA(LPCTSTR lpszStart, LPCTSTR lpszCurrent)
        "HW1*@0:user32.dll:CharPrevW"_hcode,
        // CharPrevW(LPCWSTR lpszStart, LPCWSTR lpszCurrent)
        "HA2*@0:user32.dll:CharNextExA"_hcode,
        // CharNextExA(WORD CodePage, LPCTSTR lpCurrentChar, DWORD dwFlags)
        "HW2*@0:user32.dll:CharPrevExA"_hcode
        // CharPrevExA(WORD CodePage, LPCTSTR lpStart, LPCTSTR lpCurrentChar, DWORD dwFlags)
    };

    constexpr static Hook OLEAUT32_hooks_[] = {
        "HQ1@0:OLEAUT32.dll:SysAllocString"_hcode,
        // SysAllocString(const OLECHAR* psz)
        "HQ1@0:OLEAUT32.dll:SysAllocStringLen"_hcode
        // SysAllocStringLen(const OLECHAR* pch, unsigned int cch)
    };
    // clang-format on

  public:
    PCEngine();
    ~PCEngine();
    static Engine *Match();

    REGIST_ENGINE_MATCH
};