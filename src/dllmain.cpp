#include <includes.h>
#include "Hooks.hpp"
#include "Menu.hpp"

// The entry fn
DWORD WINAPI my_thread(void* hModule) {
#ifdef GDL_INDEV
    AllocConsole();
    freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);

    setlocale(LC_ALL, "ru-RU");
    system("chcp 65001");
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
#endif // GDL_INDEV

    if (MH_Initialize() != MH_OK) {
        log << "Failed to initialize MinHook!";
        return 0;
    }

    Hooks::main();
    Menu::main();

    MH_EnableHook(MH_ALL_HOOKS);

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        CreateThread(0, 0x1000, my_thread, hModule, 0, 0);
        break;
    }
    return TRUE;
}