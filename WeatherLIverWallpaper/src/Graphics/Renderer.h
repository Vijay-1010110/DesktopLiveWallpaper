#pragma once

#include <d2d1.h>
#pragma comment(lib, "d2d1.lib")

#include <wincodec.h>
#pragma comment(lib, "windowscodecs.lib")

#include <string>

namespace Graphics {
    class Renderer
    {
    public:
        Renderer();
        ~Renderer();

        bool Initialize(HWND hwnd);
        void Cleanup();

        bool BeginDraw(HDC hdc, const RECT& rc);
        void EndDraw();

        // Generic drawing APIs for the Sandbox
        void Clear(float r, float g, float b);
        void DrawLine(float x0, float y0, float x1, float y1, float r, float g, float b, float thickness = 1.0f);
        void DrawBezierCurve(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3, float r, float g, float b, float thickness = 1.0f);
        void FillCircle(float cx, float cy, float radius, float r, float g, float b);
        
        // Texture APIs
        ID2D1Bitmap* LoadTexture(const std::wstring& filePath);
        void DrawTexture(ID2D1Bitmap* pBitmap, float x, float y, float width, float height, float opacity = 1.0f, float rotationAngle = 0.0f, float pivotX = 0.5f, float pivotY = 0.5f);
        
        float GetWidth() const;
        float GetHeight() const;

    private:
        HWND m_hwnd;
        ID2D1Factory* m_pDirect2dFactory;
        ID2D1DCRenderTarget* m_pRenderTarget;
        IWICImagingFactory* m_pWICFactory;

        HRESULT CreateDeviceResources();
        void DiscardDeviceResources();
        
        ID2D1SolidColorBrush* GetColorBrush(float r, float g, float b);
        ID2D1SolidColorBrush* m_pTempBrush;
    };
}
