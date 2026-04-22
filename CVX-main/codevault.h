#ifndef codevault_PROTECT
#define codevault_PROTECT

#include <windows.h>
#include <string>
#include <vector>
#include <memory>
#pragma comment(lib, "ntdll.lib")
#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "wintrust.lib")

#define codevault_Export extern __declspec(dllexport)

#define Hide_Function() try { int* __INVALID_ALLOC__ = new int[(std::size_t)std::numeric_limits<std::size_t>::max()]; } catch (const std::bad_alloc& except) {
#define Hide_Function_End() }

typedef void(*codevaultFunction)();

extern "C" {
    codevault_Export void codevault(std::string key, std::string webhook, bool imgui_support, HWND imgui_window_handle);
    codevault_Export void unban();
    codevault_Export void check();

    codevault_Export void Print(const std::string text);
    codevault_Export void Sleeptime(int milliseconds);
    codevault_Export void SafeThreads(codevaultFunction func);
    codevault_Export void AllThreads();
    codevault_Export void Exit();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved);
#endif
