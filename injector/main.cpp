#include "windows.h"
#include <Psapi.h>
#include <TlHelp32.h>
#include <filesystem>
#include <iostream>
#include <string>
#include <thread>

using namespace std::chrono_literals;

std::optional<std::wstring> GetModuleFilename(DWORD processId, HMODULE module = NULL)
{
    std::vector<wchar_t> buffer(MAX_PATH);
    if (auto process = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, processId))
        if (GetModuleFileNameExW(process, module, buffer.data(), MAX_PATH))
            return buffer.data();
    return {};
}

std::optional<std::wstring> GetModuleFilename(HMODULE module = NULL)
{
    std::vector<wchar_t> buffer(MAX_PATH);
    if (GetModuleFileNameW(module, buffer.data(), MAX_PATH))
        return buffer.data();
    return {};
}

void InjectProcess(DWORD processId)
{
    if (processId == GetCurrentProcessId())
        return;

    if (auto process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId))
    {
        static std::wstring location = std::filesystem::path(GetModuleFilename().value()).replace_filename(L"neohook");

        if (LPVOID remoteData = VirtualAllocEx(process, nullptr, (location.size() + 1) * sizeof(wchar_t),
                                               MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE))
        {
            WriteProcessMemory(process, remoteData, location.c_str(), (location.size() + 1) * sizeof(wchar_t), nullptr);
            if (auto thread = CreateRemoteThread(process, nullptr, 0, (LPTHREAD_START_ROUTINE)LoadLibraryW, remoteData,
                                                 0, nullptr))
                WaitForSingleObject(thread, INFINITE);
            VirtualFreeEx(process, remoteData, 0, MEM_RELEASE);
            return;
        }
    }
}

std::vector<std::pair<DWORD, std::optional<std::wstring>>> GetAllProcesses()
{
    std::vector<DWORD> processIds(10000);
    DWORD spaceUsed = 0;
    EnumProcesses(processIds.data(), 10000 * sizeof(DWORD), &spaceUsed);
    std::vector<std::pair<DWORD, std::optional<std::wstring>>> processes;
    for (int i = 0; i < spaceUsed / sizeof(DWORD); ++i)
        processes.push_back({processIds[i], GetModuleFilename(processIds[i])});
    return processes;
}

DWORD GetProcessIdByName(const std::wstring &processName)
{
    DWORD processId = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnapshot != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32 processEntry;
        processEntry.dwSize = sizeof(processEntry);

        if (Process32First(hSnapshot, &processEntry))
        {
            do
            {
                std::wstring currentProcessName(processEntry.szExeFile);
                if (currentProcessName == processName)
                {
                    processId = processEntry.th32ProcessID;
                    break;
                }
            } while (Process32Next(hSnapshot, &processEntry));
        }

        CloseHandle(hSnapshot);
    }

    return processId;
}

int main()
{

    // InjectProcess(GetProcessIdByName(L"musicus_x86.exe"));
    // GetAllProcesses();

    std::this_thread::sleep_for(2s);
    InjectProcess(GetProcessIdByName(L"殻ノ少女 HD.exe"));
    // InjectProcess(GetProcessIdByName(L"Karas.exe"));
}