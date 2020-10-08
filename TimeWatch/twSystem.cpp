// Time Watch
// twSystem.cpp
// System Initialization

#include "_pch.h"
#include "tw.h"
#include "twSystem.h"

namespace timewatch
{

    System::System() :
        m_Instance{ ::GetModuleHandle(nullptr) }
    {
        // Windows

        ::SetErrorMode(SEM_FAILCRITICALERRORS);

        ::QueryPerformanceFrequency(&m_QpcFrequency);
        ::QueryPerformanceCounter(&m_QpcEpoch);

        // Common Controls

        INITCOMMONCONTROLSEX Iccex{};
        Iccex.dwSize = sizeof(Iccex);
        Iccex.dwICC = ICC_STANDARD_CLASSES;

        if (!::InitCommonControlsEx(&Iccex))
        {
            throw std::runtime_error("Could not initialize common controls");
        }

        // COM

        auto CoRet{ ::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE) };

        if (FAILED(CoRet))
        {
            throw std::runtime_error("Could not initialize COM");
        }

        // Direct2D / DirectWrite

        HRESULT Ret{};
        
        Ret = ::D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_D2D);

        if (FAILED(Ret))
        {
            throw std::runtime_error("Could not create Direct2D");
        }

        Ret = ::DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(m_DWrite), reinterpret_cast<IUnknown**>(&m_DWrite));

        if (FAILED(Ret))
        {
            throw std::runtime_error("Could not create DirectWrite");
        }
    }

    System::~System()
    {
        m_DWrite->Release();
        m_D2D->Release();
        ::CoUninitialize();
    }

    ID2D1HwndRenderTarget* System::CreateHwndRenderTarget(HWND Window, unsigned int Width, unsigned int Height) const
    {
        ID2D1HwndRenderTarget* Render;
        
        HRESULT Ret{ m_D2D->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(Window, D2D1::SizeU(Width, Height)),
            &Render
        ) };

        if (SUCCEEDED(Ret))
        {
            return Render;
        }
        else
        {
            throw std::runtime_error("Could not create hardware render target");
        }
    }

    ID2D1PathGeometry* System::CreatePathGeometry() const
    {
        ID2D1PathGeometry* Geometry;

        HRESULT Ret{ m_D2D->CreatePathGeometry(&Geometry) };

        if (SUCCEEDED(Ret))
        {
            return Geometry;
        }
        else
        {
            throw std::runtime_error("Could not create path geometry");
        }
    }

    IDWriteTextFormat* System::CreateTextFormat(LPCTSTR FontName, FLOAT FontSize, DWRITE_FONT_WEIGHT FontWeight) const
    {
        IDWriteTextFormat* Format{};

        HRESULT Ret{ m_DWrite->CreateTextFormat(
            FontName,
            nullptr,
            FontWeight,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            FontSize,
            _T(""),
            &Format
        ) };

        if (SUCCEEDED(Ret))
        {
            return Format;
        }
        else
        {
            throw std::runtime_error("Could not create text format");
        }
    }

    double System::GetTime() const noexcept
    {
        if (m_Paused)
        {
            return double(m_QpcPauseEpoch.QuadPart - m_QpcEpoch.QuadPart) / m_QpcFrequency.QuadPart;
        }
        
        LARGE_INTEGER Qpc{};

        ::QueryPerformanceCounter(&Qpc);
        
        return double(Qpc.QuadPart - m_QpcEpoch.QuadPart) / m_QpcFrequency.QuadPart;
    }

    void System::Pause(bool Paused) noexcept
    {
        if (m_Paused == Paused)
        {
            return;
        }
        
        m_Paused = Paused;

        if (Paused)
        {
            ::QueryPerformanceCounter(&m_QpcPauseEpoch);
        }
        else
        {
            LARGE_INTEGER Now{};

            ::QueryPerformanceCounter(&Now);

            m_QpcEpoch.QuadPart += Now.QuadPart - m_QpcPauseEpoch.QuadPart;
            m_QpcPauseEpoch.QuadPart = 0;
        }
    }

}
