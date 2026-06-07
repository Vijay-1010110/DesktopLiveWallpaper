#pragma once

#include <d2d1.h>
#pragma comment(lib, "d2d1.lib")

namespace Graphics {
    class Renderer
    {
    public:
        Renderer();
        ~Renderer();

        bool Initialize(HWND hwnd);
        void Render(HDC hdc, const RECT& rc);
        void Cleanup();

    private:
        HWND m_hwnd;
        ID2D1Factory* m_pDirect2dFactory;
        ID2D1DCRenderTarget* m_pRenderTarget;
        ID2D1SolidColorBrush* m_pLightSlateGrayBrush;
        ID2D1SolidColorBrush* m_pCornflowerBlueBrush;
        
        float m_animationTime;

        HRESULT CreateDeviceResources();
        void DiscardDeviceResources();
    };
}
