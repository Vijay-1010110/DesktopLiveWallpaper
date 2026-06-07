#include "Application.h"
#include "WallpaperInjector.h"
#include <stdio.h>

namespace Core {

    Application::Application() : m_hwnd(nullptr), m_renderer(nullptr), m_isRunning(false)
    {
    }

    Application::~Application()
    {
        if (m_renderer) {
            delete m_renderer;
        }
    }

    void Application::Invalidate()
    {
        if (m_hwnd) {
            InvalidateRect(m_hwnd, NULL, FALSE);
        }
    }

    LRESULT CALLBACK Application::WindowProcSetup(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        if (message == WM_NCCREATE)
        {
            CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
            Application* pApp = reinterpret_cast<Application*>(pCreate->lpCreateParams);
            SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pApp));
            SetWindowLongPtrW(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Application::WindowProcThunk));
            return pApp->WindowProc(hWnd, message, wParam, lParam);
        }
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    LRESULT CALLBACK Application::WindowProcThunk(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        Application* pApp = reinterpret_cast<Application*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
        if (pApp) {
            return pApp->WindowProc(hWnd, message, wParam, lParam);
        }
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    LRESULT Application::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
        case WM_TIMER:
            if (wParam == 1)
            {
                // Basic 60FPS tick
                OnUpdate(1.0f / 60.0f);
                Invalidate();
            }
            break;
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            if (m_renderer)
            {
                RECT rc;
                GetClientRect(hWnd, &rc);
                m_renderer->BeginDraw(hdc, rc);
                OnRender(*m_renderer);
                m_renderer->EndDraw();
            }
            EndPaint(hWnd, &ps);
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
            m_isRunning = false;
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        return 0;
    }

    bool Application::InitializeWindow(HINSTANCE hInstance)
    {
        WNDCLASSEXW wcex = { sizeof(WNDCLASSEX) };
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = Application::WindowProcSetup;
        wcex.hInstance = hInstance;
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcex.lpszClassName = L"WeatherLiveWallpaperClass";

        if (!RegisterClassExW(&wcex)) {
            return false;
        }

        // Try to attach to desktop
        HWND g_progman = FindWindowW(L"Progman", NULL);

        // Check if Windows 11 raised desktop (24H2+)
        LONG_PTR progmanExStyle = GetWindowLongPtrW(g_progman, GWL_EXSTYLE);
        bool isRaisedDesktop = (progmanExStyle & WS_EX_NOREDIRECTIONBITMAP) != 0;

        // Spawn the WorkerW layer
        SendMessageTimeoutW(g_progman, 0x052C, 0xD, 0x1, SMTO_NORMAL, 1000, nullptr);

        HWND targetParent = nullptr;
        HWND workerW = nullptr;

        if (isRaisedDesktop) {
            targetParent = g_progman;
            // Find the workerW that Windows created behind the desktop icons
            workerW = FindWindowExW(g_progman, nullptr, L"WorkerW", nullptr);
        }
        else {
            targetParent = Core::WallpaperInjector::GetWallpaperWindow();
        }

        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);

        // Create a standard child window
        m_hwnd = CreateWindowExW(
            WS_EX_LAYERED,
            L"WeatherLiveWallpaperClass", L"WeatherLiveWallpaper",
            WS_CHILD | WS_VISIBLE,
            0, 0, screenWidth, screenHeight,
            targetParent, nullptr, hInstance, this
        );

        if (!m_hwnd) return false;

        SetLayeredWindowAttributes(m_hwnd, 0, 255, LWA_ALPHA);

        if (isRaisedDesktop) {
            HWND defView = FindWindowExW(g_progman, nullptr, L"SHELLDLL_DefView", nullptr);

            // Place our window explicitly behind SHELLDLL_DefView
            if (defView) {
                SetWindowPos(m_hwnd, defView, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
            }

            // Push the empty workerW to the very bottom
            if (workerW) {
                SetWindowPos(workerW, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
            }
        }
        else {
            // Place our window at the bottom of the empty top-level WorkerW
            SetWindowPos(m_hwnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
        }

        return true;
    }

    int Application::Run(HINSTANCE hInstance, int nCmdShow)
    {
        if (!InitializeWindow(hInstance)) {
            return -1;
        }

        m_renderer = new Graphics::Renderer();
        if (!m_renderer->Initialize(m_hwnd)) {
            return -1;
        }

        OnInit();

        // Start 60 FPS timer
        SetTimer(m_hwnd, 1, 16, NULL);

        m_isRunning = true;

        MSG msg;
        while (GetMessage(&msg, nullptr, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        return (int)msg.wParam;
    }

}
