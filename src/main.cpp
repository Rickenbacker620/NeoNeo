// #include "engines/engine_base.h"
#include <Windows.h>
#include <MinHook.h>
#include <chrono>
#include <iostream>
#include <thread>
#include "engines/engine_base.h"
#include "server.h"

using namespace std::chrono_literals;

BOOL WINAPI DllMain(HINSTANCE, DWORD fdwReason, LPVOID)
{
    AllocConsole();
    SetConsoleOutputCP(CP_UTF8);
    FILE *filePtr;
    freopen_s(&filePtr, "CONOUT$", "w", stdout);

    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH: {
        // REVIEW main loop
        // auto out = TempOutput();
        auto out = new NeoServer();
        Sink::Init(*out, 500);
        MH_Initialize();

        Engine *eg = Engine::DeduceEngineType();
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