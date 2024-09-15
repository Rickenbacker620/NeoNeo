// #include "engines/engine_base.h"
#include <Windows.h>
#include <MinHook.h>
#include <chrono>
#include <iostream>
#include <thread>

using namespace std::chrono_literals;

// BOOL WINAPI DllMain(HINSTANCE, DWORD fdwReason, LPVOID)
// {
//     AllocConsole();
//     SetConsoleOutputCP(CP_UTF8);
//     FILE *filePtr;
//     freopen_s(&filePtr, "CONOUT$", "w", stdout);

//     switch (fdwReason)
//     {
//     case DLL_PROCESS_ATTACH: {

//         // REVIEW main loop

//         MH_Initialize();
//         // while(true) {
//         //     std::this_thread::sleep_for(1s);
//         //     std::cout << "Hello, World!" << std::endl;
//         // }

//         // Engine *eg = Engine::DeduceEngineType();
//         // eg->AttachHooks();
//     }
//     break;
//     case DLL_PROCESS_DETACH: {
//         MH_Uninitialize();
//     }
//     break;
//     }
//     return TRUE;
// }
BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        /*我们的代码，注入DLL后会执行*/
        MH_Initialize();
        MessageBox(0, "Wow", "Wow123", NULL);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
