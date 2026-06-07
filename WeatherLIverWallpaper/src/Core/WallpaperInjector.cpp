#include "WallpaperInjector.h"

namespace Core {
    HWND WallpaperInjector::s_workerw = nullptr;

    BOOL CALLBACK WallpaperInjector::EnumWindowsProc(HWND tophandle, LPARAM topparamhandle)
    {
        HWND p = FindWindowExW(tophandle, 0, L"SHELLDLL_DefView", nullptr);
        if (p != nullptr)
        {
            // Gets the WorkerW Window after the current one.
            s_workerw = FindWindowExW(0, tophandle, L"WorkerW", 0);
        }
        return TRUE;
    }

    HWND WallpaperInjector::GetWallpaperWindow()
    {
        HWND progman = FindWindowW(L"Progman", nullptr);

        // Send message to Progman to spawn a WorkerW
        SendMessageTimeoutW(progman, 0x052C, 0, 0, SMTO_NORMAL, 1000, nullptr);

        s_workerw = nullptr;
        EnumWindows(EnumWindowsProc, 0);

        if (s_workerw == nullptr)
        {
            // Alternative approach if the first method fails
            EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
                wchar_t className[256];
                GetClassNameW(hwnd, className, 256);
                if (wcscmp(className, L"WorkerW") == 0) {
                    HWND defView = FindWindowExW(hwnd, 0, L"SHELLDLL_DefView", nullptr);
                    if (!defView && IsWindowVisible(hwnd)) {
                        s_workerw = hwnd;
                        return FALSE; // Stop enumerating
                    }
                }
                return TRUE;
            }, 0);
        }

        if (s_workerw != nullptr) {
            return s_workerw;
        }

        return progman; // Absolute last resort
    }
}
