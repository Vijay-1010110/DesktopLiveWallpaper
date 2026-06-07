#include <windows.h>
#include <stdio.h>
#include "Core/WallpaperInjector.h"
#include "Core/TrayIcon.h"
#include "Graphics/Renderer.h"

Graphics::Renderer* g_pRenderer = nullptr;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // WARNING: This log might be too spammy if the app enters the message loop, 
    // but right now it crashes inside CreateWindowExW.
    // We will log WM_CREATE and WM_NCCREATE specially.
    static FILE* pLog = nullptr;
    if (!pLog) fopen_s(&pLog, "C:\\Users\\Vijay\\Documents\\wallpaper_WndProc_log.txt", "w");
    
    if (message == WM_NCCREATE && pLog) { fprintf(pLog, "WM_NCCREATE\n"); fflush(pLog); }
    if (message == WM_CREATE && pLog) { fprintf(pLog, "WM_CREATE\n"); fflush(pLog); }
    if (message == WM_PAINT && pLog) { fprintf(pLog, "WM_PAINT Begin\n"); fflush(pLog); }
    if (message == WM_CLOSE && pLog) { fprintf(pLog, "WM_CLOSE\n"); fflush(pLog); }
    if (message == WM_DESTROY && pLog) { fprintf(pLog, "WM_DESTROY\n"); fflush(pLog); }

    switch (message)
    {
    case WM_TIMER:
        if (wParam == 1 && g_pRenderer)
        {
            InvalidateRect(hWnd, NULL, FALSE);
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            if (g_pRenderer)
            {
                RECT rc;
                GetClientRect(hWnd, &rc);
                if (pLog) { fprintf(pLog, "WM_PAINT Renderer->Render begin\n"); fflush(pLog); }
                g_pRenderer->Render(hdc, rc);
                if (pLog) { fprintf(pLog, "WM_PAINT Renderer->Render end\n"); fflush(pLog); }
            }
            EndPaint(hWnd, &ps);
            if (pLog) { fprintf(pLog, "WM_PAINT End\n"); fflush(pLog); }
        }
        break;
    case WM_ERASEBKGND:
        return 1;
    // Forward ALL shell messages back to SHELLDLL_DefView so desktop icons still function
    case WM_NOTIFY:
    case WM_COMMAND:
    case WM_CONTEXTMENU:
    case WM_DRAWITEM:
    case WM_MEASUREITEM:
    case WM_INITMENUPOPUP:
        return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    FILE* fp = nullptr;
    fopen_s(&fp, "C:\\Users\\Vijay\\Documents\\wallpaper_log.txt", "w");
    if (fp) { fprintf(fp, "Starting app\n"); fflush(fp); }

    // Core::TrayIcon trayIcon(hInstance);
    // trayIcon.Initialize();

    if (fp) { fprintf(fp, "TrayIcon skipped\n"); fflush(fp); }

    WNDCLASSEXW wcex = { sizeof(WNDCLASSEX) };
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.hInstance      = hInstance;
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.lpszClassName  = L"WeatherLiveWallpaperClass";

    if (!RegisterClassExW(&wcex)) {
        if (fp) { fprintf(fp, "RegisterClassExW failed\n"); fclose(fp); }
        return 1;
    }

    if (fp) { fprintf(fp, "Class Registered\n"); fflush(fp); }

    // Try to attach to desktop
    HWND g_progman = FindWindowW(L"Progman", NULL);
    
    // Check if Windows 11 raised desktop (24H2+)
    LONG_PTR progmanExStyle = GetWindowLongPtrW(g_progman, GWL_EXSTYLE);
    bool isRaisedDesktop = (progmanExStyle & WS_EX_NOREDIRECTIONBITMAP) != 0;

    if (fp) { fprintf(fp, "Is raised desktop: %d\n", isRaisedDesktop); fflush(fp); }

    // Spawn the WorkerW layer (This is critical to make SHELLDLL_DefView transparent!)
    SendMessageTimeoutW(g_progman, 0x052C, 0xD, 0x1, SMTO_NORMAL, 1000, nullptr);

    HWND targetParent = nullptr;
    HWND workerW = nullptr;

    if (isRaisedDesktop) {
        targetParent = g_progman;
        
        // Find the workerW that Windows created behind the desktop icons
        workerW = FindWindowExW(g_progman, nullptr, L"WorkerW", nullptr);
    } else {
        targetParent = Core::WallpaperInjector::GetWallpaperWindow();
    }

    if (fp) { fprintf(fp, "Target parent is %p\n", targetParent); fflush(fp); }

    // Get screen dimensions
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Create a standard child window
    HWND hWnd = CreateWindowExW(
        WS_EX_LAYERED, 
        L"WeatherLiveWallpaperClass", L"WeatherLiveWallpaper", 
        WS_CHILD | WS_VISIBLE,
        0, 0, screenWidth, screenHeight, 
        targetParent, nullptr, hInstance, nullptr
    );

    if (!hWnd)
    {
        if (fp) { fprintf(fp, "CreateWindowExW failed, error: %d\n", GetLastError()); fclose(fp); }
        return 1;
    }

    if (fp) { fprintf(fp, "Window created.\n"); fflush(fp); }

    SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);

    if (isRaisedDesktop) {
        HWND defView = FindWindowExW(g_progman, nullptr, L"SHELLDLL_DefView", nullptr);
        
        // Place our window explicitly behind SHELLDLL_DefView
        if (defView) {
            SetWindowPos(hWnd, defView, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
        }
        
        // Push the empty workerW to the very bottom
        if (workerW) {
            SetWindowPos(workerW, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        }
    } else {
        // Place our window at the bottom of the empty top-level WorkerW
        SetWindowPos(hWnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
    }

    if (fp) { fprintf(fp, "Z-ordered correctly\n"); fflush(fp); }

    Graphics::Renderer renderer;
    g_pRenderer = &renderer;
    if (!renderer.Initialize(hWnd)) {
        if (fp) { fprintf(fp, "Renderer Init failed\n"); fclose(fp); }
        return -1;
    }

    if (fp) { fprintf(fp, "Renderer Initialized. Entering message loop.\n"); fflush(fp); }

    SetTimer(hWnd, 1, 33, nullptr);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (fp) { fprintf(fp, "Message loop ended with wParam: %llu\n", msg.wParam); fclose(fp); }

    KillTimer(hWnd, 1);
    g_pRenderer = nullptr;
    return (int) msg.wParam;
}
