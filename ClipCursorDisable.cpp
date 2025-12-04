#include <Windows.h>
#include <detours.h>
#include <random>
#include <psapi.h>
#include <winnt.h>
#include <dbghelp.h>

// Typedef for ClipCursor and pointer to the original function
typedef BOOL (WINAPI *ClipCursorFunc)(const RECT *);

// Pointer to the original ClipCursor function
ClipCursorFunc pOriginalClipCursor = nullptr;

// Hooked function for ClipCursor
__declspec(dllexport) BOOL WINAPI HookedClipCursor(const RECT *lpRect)
{
    UNREFERENCED_PARAMETER(lpRect);

    // Completely disable clipping. Do not call the original.
    // Pretend it succeeded so the game thinks the call worked.
    return TRUE;
}

/**
 * Initialize the hook by finding the target function and attaching the custom implementation.
 */
void InitHook()
{
    // Get the address of ClipCursor in user32.dll
    auto hModule = GetModuleHandleA("user32.dll");
    if (!hModule)
    {
        MessageBoxA(NULL, "user32.dll not found!", "ClipCursorDisable.dll Error", MB_OK);
        return;
    }

    pOriginalClipCursor = reinterpret_cast<ClipCursorFunc>(GetProcAddress(hModule, "ClipCursor"));
    if (!pOriginalClipCursor)
    {
        MessageBoxA(NULL, "ClipCursor not found!", "ClipCursorDisable.dll Error", MB_OK);
        return;
    }

    // Detour ClipCursor with our custom implementation
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(PVOID &)pOriginalClipCursor, HookedClipCursor);
    DetourTransactionCommit();
}

/**
 * Remove the hook by detaching the custom implementation from the target function.
 */
void RemoveHook()
{
    if (!pOriginalClipCursor)
        return;

    // Detach the hook
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourDetach(&(PVOID &)pOriginalClipCursor, HookedClipCursor);
    DetourTransactionCommit();
}

/**
 * Entry point of the DLL.
 * @param hinst     The handle to the DLL module.
 * @param dwReason  The reason for calling the DLL entry point.
 * @param reserved  Reserved parameter.
 * @return          True if the DLL was initialized successfully, false otherwise.
 */
BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID reserved)
{
    if (DetourIsHelperProcess())
    {
        return TRUE;
    }

    if (dwReason == DLL_PROCESS_ATTACH)
    {
        // Initialize the hook when the DLL is loaded
        InitHook();
    }
    else if (dwReason == DLL_PROCESS_DETACH)
    {
        // Remove the hook when the DLL is unloaded
        RemoveHook();
    }

    return TRUE;
}
