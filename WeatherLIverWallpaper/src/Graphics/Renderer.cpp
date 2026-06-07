#include "Renderer.h"

namespace Graphics {
    Renderer::Renderer() :
        m_hwnd(nullptr),
        m_pDirect2dFactory(nullptr),
        m_pRenderTarget(nullptr),
        m_pWICFactory(nullptr),
        m_pTempBrush(nullptr)
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
        if (FAILED(hr)) return false;

        hr = CoInitialize(NULL);
        if (SUCCEEDED(hr) || hr == RPC_E_CHANGED_MODE) {
            hr = CoCreateInstance(
                CLSID_WICImagingFactory,
                NULL,
                CLSCTX_INPROC_SERVER,
                IID_PPV_ARGS(&m_pWICFactory)
            );
        }

        return SUCCEEDED(hr);
    }

    HRESULT Renderer::CreateDeviceResources()
    {
        HRESULT hr = S_OK;
        if (!m_pRenderTarget)
        {
            D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
                D2D1_RENDER_TARGET_TYPE_DEFAULT,
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
                0, 0, D2D1_RENDER_TARGET_USAGE_NONE, D2D1_FEATURE_LEVEL_DEFAULT
            );
            hr = m_pDirect2dFactory->CreateDCRenderTarget(&props, &m_pRenderTarget);
            
            if (SUCCEEDED(hr)) {
                hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_pTempBrush);
            }
        }
        return hr;
    }

    void Renderer::DiscardDeviceResources()
    {
        if (m_pTempBrush) { m_pTempBrush->Release(); m_pTempBrush = nullptr; }
        if (m_pRenderTarget) { m_pRenderTarget->Release(); m_pRenderTarget = nullptr; }
    }

    ID2D1SolidColorBrush* Renderer::GetColorBrush(float r, float g, float b)
    {
        if (m_pTempBrush) {
            m_pTempBrush->SetColor(D2D1::ColorF(r, g, b));
        }
        return m_pTempBrush;
    }

    bool Renderer::BeginDraw(HDC hdc, const RECT& rc)
    {
        HRESULT hr = CreateDeviceResources();
        if (SUCCEEDED(hr))
        {
            hr = m_pRenderTarget->BindDC(hdc, &rc);
            if (FAILED(hr)) return false;

            m_pRenderTarget->BeginDraw();
            m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
            return true;
        }
        return false;
    }

    void Renderer::EndDraw()
    {
        if (m_pRenderTarget) {
            HRESULT hr = m_pRenderTarget->EndDraw();
            if (hr == D2DERR_RECREATE_TARGET) {
                DiscardDeviceResources();
            }
        }
    }

    void Renderer::Clear(float r, float g, float b)
    {
        if (m_pRenderTarget) {
            m_pRenderTarget->Clear(D2D1::ColorF(r, g, b));
        }
    }

    void Renderer::DrawLine(float x0, float y0, float x1, float y1, float r, float g, float b, float thickness)
    {
        if (m_pRenderTarget) {
            m_pRenderTarget->DrawLine(D2D1::Point2F(x0, y0), D2D1::Point2F(x1, y1), GetColorBrush(r, g, b), thickness);
        }
    }

    void Renderer::DrawBezierCurve(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3, float r, float g, float b, float thickness)
    {
        if (m_pRenderTarget && m_pDirect2dFactory) {
            ID2D1PathGeometry* pathGeometry = nullptr;
            if (SUCCEEDED(m_pDirect2dFactory->CreatePathGeometry(&pathGeometry))) {
                ID2D1GeometrySink* sink = nullptr;
                if (SUCCEEDED(pathGeometry->Open(&sink))) {
                    sink->BeginFigure(D2D1::Point2F(x0, y0), D2D1_FIGURE_BEGIN_HOLLOW);
                    D2D1_BEZIER_SEGMENT bezier = {
                        D2D1::Point2F(x1, y1),
                        D2D1::Point2F(x2, y2),
                        D2D1::Point2F(x3, y3)
                    };
                    sink->AddBezier(&bezier);
                    sink->EndFigure(D2D1_FIGURE_END_OPEN);
                    sink->Close();
                    
                    m_pRenderTarget->DrawGeometry(pathGeometry, GetColorBrush(r, g, b), thickness);
                    sink->Release();
                }
                pathGeometry->Release();
            }
        }
    }

    void Renderer::FillCircle(float cx, float cy, float radius, float r, float g, float b)
    {
        if (m_pRenderTarget) {
            D2D1_ELLIPSE ellipse = D2D1::Ellipse(D2D1::Point2F(cx, cy), radius, radius);
            m_pRenderTarget->FillEllipse(ellipse, GetColorBrush(r, g, b));
        }
    }

    ID2D1Bitmap* Renderer::LoadTexture(const std::wstring& filePath)
    {
        if (!m_pWICFactory || !m_pRenderTarget) return nullptr;

        IWICBitmapDecoder* pDecoder = nullptr;
        HRESULT hr = m_pWICFactory->CreateDecoderFromFilename(
            filePath.c_str(), NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &pDecoder);
        if (FAILED(hr)) return nullptr;

        IWICBitmapFrameDecode* pSource = nullptr;
        hr = pDecoder->GetFrame(0, &pSource);

        IWICFormatConverter* pConverter = nullptr;
        if (SUCCEEDED(hr)) {
            hr = m_pWICFactory->CreateFormatConverter(&pConverter);
        }

        if (SUCCEEDED(hr)) {
            hr = pConverter->Initialize(
                pSource, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone,
                NULL, 0.f, WICBitmapPaletteTypeMedianCut);
        }

        ID2D1Bitmap* pBitmap = nullptr;
        if (SUCCEEDED(hr)) {
            hr = m_pRenderTarget->CreateBitmapFromWicBitmap(pConverter, NULL, &pBitmap);
        }

        if (pDecoder) pDecoder->Release();
        if (pSource) pSource->Release();
        if (pConverter) pConverter->Release();

        return pBitmap;
    }

    void Renderer::DrawTexture(ID2D1Bitmap* pBitmap, float x, float y, float width, float height, float opacity, float rotationAngle, float pivotX, float pivotY)
    {
        if (m_pRenderTarget && pBitmap) {
            D2D1_RECT_F destRect = D2D1::RectF(x, y, x + width, y + height);
            
            if (rotationAngle != 0.0f) {
                D2D1_POINT_2F pivot = D2D1::Point2F(x + width * pivotX, y + height * pivotY);
                m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Rotation(rotationAngle, pivot));
            }

            m_pRenderTarget->DrawBitmap(pBitmap, &destRect, opacity, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, nullptr);

            if (rotationAngle != 0.0f) {
                m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
            }
        }
    }

    float Renderer::GetWidth() const
    {
        return m_pRenderTarget ? m_pRenderTarget->GetSize().width : 0.0f;
    }

    float Renderer::GetHeight() const
    {
        return m_pRenderTarget ? m_pRenderTarget->GetSize().height : 0.0f;
    }

    void Renderer::Cleanup()
    {
        DiscardDeviceResources();
        if (m_pWICFactory) { m_pWICFactory->Release(); m_pWICFactory = nullptr; }
        if (m_pDirect2dFactory) { m_pDirect2dFactory->Release(); m_pDirect2dFactory = nullptr; }
        CoUninitialize();
    }
}
