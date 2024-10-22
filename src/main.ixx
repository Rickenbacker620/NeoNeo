#include <MinHook.h>
#include <Windows.h>

import <chrono>;
import <iostream>;

import engine_base;
import pcengine;
import dialogue;

BOOL WINAPI DllMain(HINSTANCE, DWORD fdwReason, LPVOID)
{
    AllocConsole();
    SetConsoleOutputCP(CP_UTF8);
    FILE *filePtr;
    freopen_s(&filePtr, "CONOUT$", "w", stdout);

    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH: {
        auto out = new TempOutput();
        Sink::Init(*out, 500);

        MH_Initialize();
        Engine *eg = new PCEngine();

        eg->AttachHooks();
    }
    break;
    case DLL_PROCESS_DETACH: {
        MH_Uninitialize();
    }
    break;
    }
    return TRUE;
}
