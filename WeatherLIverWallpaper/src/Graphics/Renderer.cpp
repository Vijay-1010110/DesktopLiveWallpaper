#include "Renderer.h"
#include <math.h>

namespace Graphics {
    Renderer::Renderer() :
        m_hwnd(nullptr),
        m_pDirect2dFactory(nullptr),
        m_pRenderTarget(nullptr),
        m_pLightSlateGrayBrush(nullptr),
        m_pCornflowerBlueBrush(nullptr),
        m_animationTime(0.0f)
    {
    }

    Renderer::~Renderer()
    {
        Cleanup();
    }

    bool Renderer::Initialize(HWND hwnd)
    {
        m_hwnd = hwnd;

        HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);
        if (FAILED(hr))
        {
            return false;
        }

        return true;
    }

    HRESULT Renderer::CreateDeviceResources()
    {
        HRESULT hr = S_OK;

        if (!m_pRenderTarget)
        {
            D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
                D2D1_RENDER_TARGET_TYPE_DEFAULT,
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
                0,
                0,
                D2D1_RENDER_TARGET_USAGE_NONE,
                D2D1_FEATURE_LEVEL_DEFAULT
            );

            hr = m_pDirect2dFactory->CreateDCRenderTarget(
                &props,
                &m_pRenderTarget
            );

            if (SUCCEEDED(hr))
            {
                hr = m_pRenderTarget->CreateSolidColorBrush(
                    D2D1::ColorF(D2D1::ColorF::LightSlateGray),
                    &m_pLightSlateGrayBrush
                );
            }

            if (SUCCEEDED(hr))
            {
                hr = m_pRenderTarget->CreateSolidColorBrush(
                    D2D1::ColorF(D2D1::ColorF::CornflowerBlue),
                    &m_pCornflowerBlueBrush
                );
            }
        }

        return hr;
    }

    void Renderer::DiscardDeviceResources()
    {
        if (m_pRenderTarget) m_pRenderTarget->Release(); m_pRenderTarget = nullptr;
        if (m_pLightSlateGrayBrush) m_pLightSlateGrayBrush->Release(); m_pLightSlateGrayBrush = nullptr;
        if (m_pCornflowerBlueBrush) m_pCornflowerBlueBrush->Release(); m_pCornflowerBlueBrush = nullptr;
    }

    void Renderer::Render(HDC hdc, const RECT& rc)
    {
        HRESULT hr = CreateDeviceResources();

        if (SUCCEEDED(hr))
        {
            hr = m_pRenderTarget->BindDC(hdc, &rc);
            if (FAILED(hr)) return;

            m_pRenderTarget->BeginDraw();

            m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

            // Clear background with a dynamic color
            float r = (sin(m_animationTime) + 1.0f) * 0.5f;
            float g = (cos(m_animationTime * 0.8f) + 1.0f) * 0.5f;
            float b = (sin(m_animationTime * 1.2f) + 1.0f) * 0.5f;

            m_pRenderTarget->Clear(D2D1::ColorF(r * 0.2f, g * 0.2f + 0.3f, b * 0.5f + 0.5f));

            D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize();

            // Draw a grid
            int width = static_cast<int>(rtSize.width);
            int height = static_cast<int>(rtSize.height);

            for (int x = 0; x < width; x += 50)
            {
                m_pRenderTarget->DrawLine(
                    D2D1::Point2F(static_cast<FLOAT>(x), 0.0f),
                    D2D1::Point2F(static_cast<FLOAT>(x), rtSize.height),
                    m_pLightSlateGrayBrush,
                    0.5f
                );
            }

            for (int y = 0; y < height; y += 50)
            {
                m_pRenderTarget->DrawLine(
                    D2D1::Point2F(0.0f, static_cast<FLOAT>(y)),
                    D2D1::Point2F(rtSize.width, static_cast<FLOAT>(y)),
                    m_pLightSlateGrayBrush,
                    0.5f
                );
            }

            // Draw an animated circle
            float cx = rtSize.width / 2.0f + sin(m_animationTime * 2.0f) * 200.0f;
            float cy = rtSize.height / 2.0f + cos(m_animationTime * 2.0f) * 200.0f;

            D2D1_ELLIPSE ellipse = D2D1::Ellipse(D2D1::Point2F(cx, cy), 100.0f, 100.0f);
            m_pRenderTarget->FillEllipse(ellipse, m_pCornflowerBlueBrush);

            hr = m_pRenderTarget->EndDraw();

            if (hr == D2DERR_RECREATE_TARGET)
            {
                hr = S_OK;
                DiscardDeviceResources();
            }
            
            m_animationTime += 0.033f; // ~30 fps step
        }
    }

    void Renderer::Cleanup()
    {
        DiscardDeviceResources();
        if (m_pDirect2dFactory) m_pDirect2dFactory->Release(); m_pDirect2dFactory = nullptr;
    }
}
