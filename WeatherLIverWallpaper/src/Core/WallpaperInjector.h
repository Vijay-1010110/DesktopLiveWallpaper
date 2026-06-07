#pragma once
#include <windows.h>

namespace Core {
    class WallpaperInjector {
    public:
        // Attempts to spawn and return the WorkerW window handle
        // that sits behind the desktop icons.
        static HWND GetWallpaperWindow();
    private:
        static BOOL CALLBACK EnumWindowsProc(HWND tophandle, LPARAM topparamhandle);
        static HWND s_workerw;
    };
}
