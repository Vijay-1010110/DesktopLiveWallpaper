#include "TrayIcon.h"
#include "../../Resource.h"

namespace Core {
    TrayIcon* g_pTrayIcon = nullptr;

    TrayIcon::TrayIcon(HINSTANCE hInstance) :
        m_hInstance(hInstance),
        m_hWnd(nullptr)
    {
        g_pTrayIcon = this;
        ZeroMemory(&m_nid, sizeof(m_nid));
    }

    TrayIcon::~TrayIcon()
    {
        Cleanup();
        g_pTrayIcon = nullptr;
    }

    bool TrayIcon::Initialize()
    {
        // Register a message-only window class
        WNDCLASSEXW wcex = { sizeof(WNDCLASSEX) };
        wcex.lpfnWndProc = TrayWndProc;
        wcex.hInstance = m_hInstance;
        wcex.lpszClassName = L"WeatherLiveWallpaperTrayClass";
        RegisterClassExW(&wcex);

        // Create message-only window
        m_hWnd = CreateWindowExW(0, L"WeatherLiveWallpaperTrayClass", L"", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, m_hInstance, nullptr);
        if (!m_hWnd) return false;

        // Setup notify icon data
        m_nid.cbSize = sizeof(NOTIFYICONDATA);
        m_nid.hWnd = m_hWnd;
        m_nid.uID = 1;
        m_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
        m_nid.uCallbackMessage = WM_TRAYICON;
        m_nid.hIcon = LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_WEATHERLIVERWALLPAPER));
        wcscpy_s(m_nid.szTip, L"Weather Live Wallpaper");

        Shell_NotifyIcon(NIM_ADD, &m_nid);

        return true;
    }

    void TrayIcon::Cleanup()
    {
        if (m_hWnd)
        {
            Shell_NotifyIcon(NIM_DELETE, &m_nid);
            DestroyWindow(m_hWnd);
            m_hWnd = nullptr;
        }
    }

    void TrayIcon::ShowContextMenu(HWND hWnd, POINT pt)
    {
        HMENU hMenu = CreatePopupMenu();
        if (hMenu)
        {
            InsertMenu(hMenu, -1, MF_BYPOSITION | MF_STRING, 1, L"Restart Wallpaper");
            InsertMenu(hMenu, -1, MF_BYPOSITION | MF_STRING, 2, L"Exit");

            // Needed to avoid menu lingering when clicking outside
            SetForegroundWindow(hWnd);

            int cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_NONOTIFY, pt.x, pt.y, 0, hWnd, nullptr);

            if (cmd == 1)
            {
                // Optionally handle restart logic here, but for now just post a quit message or custom message
                // For a proper restart, we might need a custom window message sent to the main loop
            }
            else if (cmd == 2)
            {
                PostQuitMessage(0);
            }

            DestroyMenu(hMenu);
        }
    }

    LRESULT CALLBACK TrayIcon::TrayWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        if (message == WM_TRAYICON)
        {
            if (lParam == WM_RBUTTONUP)
            {
                POINT pt;
                GetCursorPos(&pt);
                if (g_pTrayIcon)
                {
                    g_pTrayIcon->ShowContextMenu(hWnd, pt);
                }
            }
        }
        else if (message == WM_DESTROY)
        {
            PostQuitMessage(0);
        }

        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}
