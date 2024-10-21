
#include <MinHook.h>

#include <Windows.h>

import <chrono>;
import <iostream>;
import <thread>;

import server;
import engine_base;
import pcengine;
import dialogue;


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
         auto out = new TempOutput();
//        auto out = new NeoServer();
        Sink::Init(*out, 500);
        MH_Initialize();

        Engine *eg = new PCEngine();
        eg->AttachHooks();
//        MessageBoxA(NULL, "Hello, World!", "Hello, World!", MB_OK);
    }
        break;
    case DLL_PROCESS_DETACH: {
        MH_Uninitialize();
    }
        break;
    }
    return TRUE;
}
