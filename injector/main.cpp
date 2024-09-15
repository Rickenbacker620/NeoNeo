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

// void InjectProcess(DWORD processId)
// {
//     if (processId == GetCurrentProcessId())
//         return;

//     if (auto process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId))
//     {
//         static std::wstring location =
//         std::filesystem::path(GetModuleFilename().value()).replace_filename(L"neohook");

//         if (LPVOID remoteData = VirtualAllocEx(process, nullptr, (location.size() + 1) * sizeof(wchar_t),
//                                                MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE))
//         {
//             WriteProcessMemory(process, remoteData, location.c_str(), (location.size() + 1) * sizeof(wchar_t),
//             nullptr); if (auto thread = CreateRemoteThread(process, nullptr, 0, (LPTHREAD_START_ROUTINE)LoadLibraryW,
//             remoteData,
//                                                  0, nullptr))
//                 WaitForSingleObject(thread, INFINITE);
//             VirtualFreeEx(process, remoteData, 0, MEM_RELEASE);
//             return;
//         }
//     }
// }
void InjectProcess(DWORD processId)
{
    std::cout << "Injecting into process " << processId << std::endl;
    if (processId == GetCurrentProcessId())
        return;

    HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
    if (!process)
    {
        std::cerr << "Failed to open process. Error: " << GetLastError() << std::endl;
        return;
    }

    // std::wstring location = std::filesystem::path(GetModuleFileNameW(nullptr, nullptr,
    // 0)).replace_filename(L"neohook");

    static std::wstring location = std::filesystem::path(GetModuleFilename().value()).replace_filename(L"neohook");

    std::wcout << "Location: " << location << std::endl;

    LPVOID remoteData = VirtualAllocEx(process, nullptr, (location.size() + 1) * sizeof(wchar_t),
                                       MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (!remoteData)
    {
        std::cerr << "Failed to allocate memory in the target process. Error: " << GetLastError() << std::endl;
        CloseHandle(process);
        return;
    }

    if (!WriteProcessMemory(process, remoteData, location.c_str(), (location.size() + 1) * sizeof(wchar_t), nullptr))
    {
        std::cerr << "Failed to write to process memory. Error: " << GetLastError() << std::endl;
        VirtualFreeEx(process, remoteData, 0, MEM_RELEASE);
        CloseHandle(process);
        return;
    }

    HANDLE thread =
        CreateRemoteThread(process, nullptr, 0, (LPTHREAD_START_ROUTINE)LoadLibraryW, remoteData, 0, nullptr);
    if (!thread)
    {
        std::cerr << "Failed to create remote thread. Error: " << GetLastError() << std::endl;
        VirtualFreeEx(process, remoteData, 0, MEM_RELEASE);
        CloseHandle(process);
        return;
    }

    WaitForSingleObject(thread, INFINITE);
    // Check if the DLL was loaded successfully
    // Check if the DLL was loaded successfully
    DWORD exitCode;
    if (GetExitCodeThread(thread, &exitCode))
    {
        if (exitCode == NULL)
        {
            // Get the last error from the target process' thread
            DWORD lastError = GetLastError();
            std::cerr << "Failed to load DLL. LoadLibraryW returned NULL. Error code: " << lastError << std::endl;

            // Optionally, print a more human-readable message for common error codes
            if (lastError == ERROR_MOD_NOT_FOUND)
            {
                std::cerr << "The specified module (DLL) was not found." << std::endl;
            }
            else if (lastError == ERROR_BAD_EXE_FORMAT)
            {
                std::cerr << "The DLL is not a valid executable or is for a different architecture." << std::endl;
            }
            else
            {
                std::cerr << "Unknown error occurred. Error code: " << lastError << std::endl;
            }
        }
        else
        {
            std::cout << "DLL loaded successfully." << std::endl;
        }
    }
    else
    {
        std::cerr << "Failed to get the exit code of the remote thread." << std::endl;
    }

    // Clean up
    VirtualFreeEx(process, remoteData, 0, MEM_RELEASE);
    CloseHandle(thread);
    CloseHandle(process);
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

// DWORD GetProcessIdByName(const std::wstring &processName)
// {
//     DWORD processId = 0;
//     HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

//     if (hSnapshot != INVALID_HANDLE_VALUE)
//     {
//         PROCESSENTRY32 processEntry;
//         processEntry.dwSize = sizeof(processEntry);

//         if (Process32First(hSnapshot, &processEntry))
//         {
//             do
//             {
//                 std::wstring currentProcessName(processEntry.szExeFile);
//                 if (currentProcessName == processName)
//                 {
//                     processId = processEntry.th32ProcessID;
//                     break;
//                 }
//             } while (Process32Next(hSnapshot, &processEntry));
//         }

//         CloseHandle(hSnapshot);
//     }

//     return processId;
// }
DWORD GetProcessIdByName(const std::wstring &processName)
{
    DWORD processId = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnapshot == INVALID_HANDLE_VALUE)
    {
        std::cerr << "Failed to create snapshot. Error: " << GetLastError() << std::endl;
        return 0;
    }

    PROCESSENTRY32 processEntry;
    processEntry.dwSize = sizeof(processEntry);

    // Get the first process
    if (!Process32First(hSnapshot, &processEntry))
    {
        std::cerr << "Failed to retrieve the first process. Error: " << GetLastError() << std::endl;
        CloseHandle(hSnapshot);
        return 0;
    }

    // Iterate through the processes
    do
    {
        std::wstring currentProcessName(processEntry.szExeFile);

        if (currentProcessName == processName)
        {
            processId = processEntry.th32ProcessID;
            break;
        }

    } while (Process32Next(hSnapshot, &processEntry));

    if (processId == 0)
    {
        std::cout << "Process not found" << std::endl;
    }

    CloseHandle(hSnapshot);
    return processId;
}

int main()
{

    // InjectProcess(GetProcessIdByName(L"musicus_x86.exe"));
    // GetAllProcesses();

    std::this_thread::sleep_for(3s);
    auto processId = GetProcessIdByName(L"殻ノ少女 HD.exe");
    std::cout << "Process ID: " << processId << std::endl;
    InjectProcess(processId);
    std::cout << "Injected" << std::endl;
    // InjectProcess(GetProcessIdByName(L"Karas.exe"));
}