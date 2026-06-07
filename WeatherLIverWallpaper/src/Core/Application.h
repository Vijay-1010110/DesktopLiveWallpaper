#pragma once

#include <windows.h>
#include "../Graphics/Renderer.h"

namespace Core {

    class Application
    {
    public:
        Application();
        virtual ~Application();

        // Starts the application loop
        int Run(HINSTANCE hInstance, int nCmdShow);

    protected:
        // Lifecycle methods for the Sandbox to override
        virtual void OnInit() {}
        virtual void OnUpdate(float deltaTime) {}
        virtual void OnRender(Graphics::Renderer& renderer) {}

        // Request a redraw
        void Invalidate();

    private:
        HWND m_hwnd;
        Graphics::Renderer* m_renderer;
        bool m_isRunning;
        
        // Static window procedure to route messages to class instance
        static LRESULT CALLBACK WindowProcSetup(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
        static LRESULT CALLBACK WindowProcThunk(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
        LRESULT WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

        bool InitializeWindow(HINSTANCE hInstance);
    };

}
