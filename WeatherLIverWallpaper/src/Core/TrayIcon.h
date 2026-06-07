#pragma once
#include <windows.h>
#include <shellapi.h>

namespace Core {
    class TrayIcon {
    public:
        TrayIcon(HINSTANCE hInstance);
        ~TrayIcon();

        bool Initialize();
        void Cleanup();

    private:
        static LRESULT CALLBACK TrayWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
        void ShowContextMenu(HWND hWnd, POINT pt);

        HINSTANCE m_hInstance;
        HWND m_hWnd;
        NOTIFYICONDATA m_nid;

        static const UINT WM_TRAYICON = WM_USER + 1;
    };
}
