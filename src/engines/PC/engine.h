#pragma once
#include "../engine_base.h"

static HookParam kernel32_hooks_[] = {
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

static HookParam user32_hooks_[] = {
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
    "HW2*@0:user32.dll:CharPrevExA"_hcode,
    // CharPrevExA(WORD CodePage, LPCTSTR lpStart, LPCTSTR lpCurrentChar, DWORD dwFlags)
    "HS2:3@0:user32.dll:DrawTextA"_hcode,
    // DrawTextA(HDC hdc, LPCSTR lpchText, int cchText, LPRECT lprc, UINT format)
    "HS2:3@0:user32.dll:DrawTextExA"_hcode,
    // DrawTextExA(HDC hdc, LPSTR lpchText, int cchText, LPRECT lprc, UINT format, LPDRAWTEXTPARAMS lpdtp)
    "HS4:5@0:user32.dll:TabbedTextOutA"_hcode,
    // TabbedTextOutA(HDC hdc, int x, int y, LPCSTR lpString, int chCount, int nTabPositions, const LPINT lpnTabStopPositions, int nTabOrigin)
    "HS2:3@0:user32.dll:GetTabbedTextExtentA"_hcode,
    // GetTabbedTextExtentA(HDC hdc, LPCSTR lpString, int chCount, int nTabPositions, const LPINT lpnTabStopPositions)
    "HS2:3@0:user32.dll:DrawTextW"_hcode,
    // DrawTextW(HDC hdc, LPCWSTR lpchText, int cchText, LPRECT lprc, UINT format)
    "HS2:3@0:user32.dll:DrawTextExW"_hcode,
    // DrawTextExW(HDC hdc, LPWSTR lpchText, int cchText, LPRECT lprc, UINT format, LPDRAWTEXTPARAMS lpdtp)
    "HS4:5@0:user32.dll:TabbedTextOutW"_hcode,
    // TabbedTextOutW(HDC hdc, int x, int y, LPCWSTR lpString, int chCount, int nTabPositions, const LPINT lpnTabStopPositions, int nTabOrigin)
    "HS2:3@0:user32.dll:GetTabbedTextExtentW"_hcode,
    // GetTabbedTextExtentW(HDC hdc, LPCWSTR lpString, int chCount, int nTabPositions, const LPINT lpnTabStopPositions)
};

static HookParam oleaut32_hooks_[] = {
    "HQ1@0:OLEAUT32.dll:SysAllocString"_hcode,
    // SysAllocString(const OLECHAR* psz)
    "HQ1:2@0:OLEAUT32.dll:SysAllocStringLen"_hcode
    // SysAllocStringLen(const OLECHAR* pch, unsigned int cch)
};

static HookParam gdi32_hooks_[] = {
    "HS2:3@0:gdi32.dll:GetTextExtentPoint32A"_hcode,
    // GetTextExtentPoint32A(HDC hdc, LPCSTR lpString, int c, LPSIZE psizl)
    "HS2:3@0:gdi32.dll:GetTextExtentPointA"_hcode,
    // GetTextExtentPointA(HDC hdc, LPCSTR lpString, int c, LPSIZE psizl)
    "HS2:3@0:gdi32.dll:GetCharacterPlacementA"_hcode,
    // GetCharacterPlacementA(HDC hdc, LPCSTR lpString, int nCount, int nMaxExtent, GCP_RESULTS* lpResults, DWORD dwFlags)
    "HS2:3@0:gdi32.dll:GetGlyphIndicesA"_hcode,
    // GetGlyphIndicesA(HDC hdc, LPCSTR lpstr, int c, LPWORD pgi, DWORD fl)
    "HA2@0:gdi32.dll:GetGlyphOutlineA"_hcode,
    // GetGlyphOutlineA(HDC hdc, UINT uChar, UINT uFormat, LPGLYPHMETRICS lpgm, DWORD cbBuffer, LPVOID lpvBuffer, const MAT2* lpmat2)
    "HS6:7@0:gdi32.dll:ExtTextOutA"_hcode,
    // ExtTextOutA(HDC hdc, int x, int y, UINT options, const RECT* lprect, LPCSTR lpString, UINT c, const INT* lpDx)
    "HS4:5@0:gdi32.dll:TextOutA"_hcode,
    // TextOutA(HDC hdc, int x, int y, LPCSTR lpString, int c)
    "HA2@0:gdi32.dll:GetCharABCWidthsA"_hcode,
    // GetCharABCWidthsA(HDC hdc, UINT iFirst, UINT iLast, LPABC lpabc)
    "HA2@0:gdi32.dll:GetCharABCWidthsFloatA"_hcode,
    // GetCharABCWidthFloatA(HDC hdc, UINT iFirst, UINT iLast, LPABCFLOAT lpABCF)
    "HA2@0:gdi32.dll:GetCharWidth32A"_hcode,
    // Getcharwidth32A(HDC hdc, UINT iFirst, UINT iLast, LPINT lpBuffer)
    "HA2@0:gdi32.dll:GetCharWidthFloatA"_hcode,
    // GetCharWidthFloatA(HDC hdc, UINT iFirst, UINT iLast, PFLOAT lpBuffer)
    "HS2:3@0:gdi32.dll:GetTextExtentPoint32W"_hcode,
    // GetTextExtentPoint32W(HDC hdc, LPCWSTR lpString, int c, LPSIZE psizl)
    "HS2:3@0:gdi32.dll:GetTextExtentExPointW"_hcode,
    // GetTextExtentExPointW(HDC hdc, LPCWSTR lpszStr, int cchString, int nMaxExtent, LPINT lpnFit, LPINT alpDx, LPSIZE lpSize)
    "HS2:3@0:gdi32.dll:GetCharacterPlacementW"_hcode,
    // GetCharacterPlacementW(HDC hdc, LPCWSTR lpString, int nCount, int nMaxExtent, GCP_RESULTS* lpResults, DWORD dwFlags)
    "HS2:3@0:gdi32.dll:GetGlyphIndicesW"_hcode,
    // GetGlyphIndicesW(HDC hdc, LPCWSTR lpstr, int c, LPWORD pgi, DWORD fl)
    "HW2@0:gdi32.dll:GetGlyphOutlineW"_hcode,
    // GetGlyphOutlineW(HDC hdc, UINT uChar, UINT uFormat, LPGLYPHMETRICS lpgm, DWORD cbBuffer, LPVOID lpvBuffer, const MAT2* lpmat2)
    "HS6:7@0:gdi32.dll:ExtTextOutW"_hcode,
    // ExtTextOutW(HDC hdc, int x, int y, UINT options, const RECT* lprect, LPCWSTR lpString, UINT c, const INT* lpDx)
    "HS4:5@0:gdi32.dll:TextOutW"_hcode,
    // TextOutW(HDC hdc, int x, int y, LPCWSTR lpString, int c)
    "HW2@0:gdi32.dll:GetCharABCWidthsW"_hcode,
    // GetCharABCWidthsW(HDC hdc, UINT iFirst, UINT iLast, LPABC lpabc)
    "HA2@0:gdi32.dll:GetCharABCWidthsFloatW"_hcode,
    // GetCharABCWidthFloatW(HDC hdc, UINT iFirst, UINT iLast, LPABCFLOAT lpABCF)
    "HA2@0:gdi32.dll:GetCharWidth32W"_hcode,
    // Getcharwidth32W(HDC hdc, UINT iFirst, UINT iLast, LPINT lpBuffer)
    "HA2@0:gdi32.dll:GetCharWidthFloatW"_hcode,
    // GetCharWidthFloatW(HDC hdc, UINT iFirst, UINT iLast, PFLOAT lpBuffer)
};
// clang-format on

class PCEngine : public Engine
{
  public:
    PCEngine();
    static Engine *Match();

    REGISTER_ENGINE_MATCH
};