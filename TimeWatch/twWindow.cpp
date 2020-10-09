// Time Watch
// twWindow.cpp
// Application Main Window Frame

#include "_pch.h"
#include "tw.h"
#include "twNotify.h"
#include "twSystem.h"
#include "twWindow.h"

namespace
{

    constexpr auto c_WindowClass{ _T("TW_CLASS") };
    constexpr auto c_WindowTitle{ _T("Time Watch") };

    constexpr auto c_WindowStyle{ WS_POPUP | WS_BORDER };
    constexpr auto c_WindowStyleEx{ 0 };

    constexpr auto c_EllipseRadius{ 160.0f };
    constexpr auto c_TimerGranularity{ 6 };

}

namespace timewatch
{
    
    Window::Window(System& SystemRef) :
        m_System{ SystemRef }
    {
        // Register Window Class

        WNDCLASSEX Wcex{};
        Wcex.cbSize = sizeof(Wcex);
        Wcex.style = CS_OWNDC;
        Wcex.lpfnWndProc = Window::StaticWindowProc;
        Wcex.cbClsExtra = 0;
        Wcex.cbWndExtra = 0;
        Wcex.hInstance = m_System.Instance();
        Wcex.hIcon = ::LoadIcon(m_System.Instance(), MAKEINTRESOURCE(TWRID_ICON_APPLICATION));
        Wcex.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
        Wcex.hbrBackground = nullptr;
        Wcex.lpszMenuName = nullptr;
        Wcex.lpszClassName = c_WindowClass;
        Wcex.hIconSm = Wcex.hIcon;

        if (!::RegisterClassEx(&Wcex))
        {
            throw std::runtime_error("Could not register window class");
        }

        // Compute window size and position

        auto ScreenWidth{ ::GetSystemMetrics(SM_CXSCREEN) };
        auto ScreenHeight{ ::GetSystemMetrics(SM_CYSCREEN) };

        RECT ClientArea{};
        ClientArea.right = 256 + 128;
        ClientArea.bottom = 256 + 128;

        ::AdjustWindowRectEx(&ClientArea, c_WindowStyle, FALSE, c_WindowStyleEx);

        auto ClientWidth{ ClientArea.right - ClientArea.left };
        auto ClientHeight{ ClientArea.bottom - ClientArea.top };
        auto ClientX{ ScreenWidth / 2 - ClientWidth / 2 };
        auto ClientY{ ScreenHeight / 2 - ClientHeight / 2 };

        // Create Window

        m_Window = ::CreateWindowEx(
            c_WindowStyleEx,
            c_WindowClass,
            c_WindowTitle,
            c_WindowStyle,
            ClientX,
            ClientY,
            ClientWidth,
            ClientHeight,
            nullptr,
            nullptr,
            m_System.Instance(),
            this
        );

        if (m_Window == nullptr)
        {
            throw std::runtime_error("Could not create window");
        }

        ::ShowWindow(m_Window, SW_SHOWDEFAULT);

        ::SetTimer(m_Window, TWTID_TIMER, 1000 / c_TimerGranularity, nullptr);

        LoadDraw();

        m_Needle = m_System.CreatePathGeometry();

        if (ID2D1GeometrySink* Sink{}; SUCCEEDED(m_Needle->Open(&Sink)))
        {
            Sink->BeginFigure(D2D1::Point2F(0.0f, 0.0f), D2D1_FIGURE_BEGIN_FILLED);

            D2D1_POINT_2F Points[] = {
                D2D1::Point2F(+5.0f, 10.0f),
                D2D1::Point2F(-5.0f, 10.0f)
            };

            Sink->AddLines(Points, _countof(Points));

            Sink->EndFigure(D2D1_FIGURE_END_CLOSED);

            Sink->Close();
            Sink->Release();
        }


        UpdateTime(false);
    }

    Window::~Window()
    {                        
        for (const auto& [Geometry, Color] : m_Geometries)
        {
            Geometry->Release();
        }

        m_Needle->Release();
        
        UnloadDraw();

        ::KillTimer(m_Window, TWTID_TIMER);
        ::DestroyWindow(m_Window);
        ::UnregisterClass(c_WindowClass, m_System.Instance());
    }

    void Window::FreezeStages()
    {
        RECT ClientRect{};
        ::GetClientRect(m_Window, &ClientRect);

        auto ClientCenterX{ float(ClientRect.right - ClientRect.left) / 2.0f };
        auto ClientCenterY{ float(ClientRect.bottom - ClientRect.top) / 2.0f };

        auto GetPointFromAngle = [&](float Angle)
        {
            Angle = (Angle - 90.0f) * 3.141592f / 180.0f;
            
            return D2D1::Point2F(
                ClientCenterX + c_EllipseRadius * std::cosf(Angle),
                ClientCenterY + c_EllipseRadius * std::sinf(Angle)
            );
        };
        
        double FirstAngle{};

        for (const auto& S : m_Stages)
        {
            double SecondAngle{ FirstAngle + S.Duration * 360.0f / m_StagesRuntime };

            ID2D1PathGeometry* ArcGeometry{ m_System.CreatePathGeometry() };

            if (ID2D1GeometrySink* Sink{}; SUCCEEDED(ArcGeometry->Open(&Sink)))
            {
                auto FirstPoint{ GetPointFromAngle(float(FirstAngle)) };
                auto SecondPoint{ GetPointFromAngle(float(SecondAngle)) };

                Sink->BeginFigure(FirstPoint, D2D1_FIGURE_BEGIN_HOLLOW);

                Sink->AddArc(D2D1::ArcSegment(
                    SecondPoint,
                    D2D1::SizeF(c_EllipseRadius, c_EllipseRadius),
                    0.0f,
                    D2D1_SWEEP_DIRECTION_CLOCKWISE,
                    ((SecondAngle - FirstAngle) >= 180.0f) ? D2D1_ARC_SIZE_LARGE : D2D1_ARC_SIZE_SMALL
                ));

                Sink->EndFigure(D2D1_FIGURE_END_OPEN);
                
                Sink->Close();
                Sink->Release();
                
                m_Geometries.emplace_back(ArcGeometry, S.Color);
            }
            else
            {
                ArcGeometry->Release();
            }

            FirstAngle = SecondAngle;
        }
    }

    void Window::LoadDraw()
    {
        RECT ClientRect{};
        ::GetClientRect(m_Window, &ClientRect);

        m_Render = m_System.CreateHwndRenderTarget(
            m_Window,
            ClientRect.right - ClientRect.left,
            ClientRect.bottom - ClientRect.top
        );

        m_Render->CreateSolidColorBrush(D2D1::ColorF(0x000000), &m_Brush);

        m_Render->CreateSolidColorBrush(D2D1::ColorF(0xE4E4E4), &m_PausedBrush);

        m_Render->CreateSolidColorBrush(D2D1::ColorF(0x000000), &m_ArcBrush);

        m_MajorFont = m_System.CreateTextFormat(_T("Consolas"), 32, DWRITE_FONT_WEIGHT_BOLD);
        m_MajorFont->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        m_MajorFont->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

        m_MinorFont = m_System.CreateTextFormat(_T("Arial"), 18);
        m_MinorFont->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        m_MinorFont->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    }

    void Window::UnloadDraw()
    {
        m_MinorFont->Release();
        m_MajorFont->Release();
        m_ArcBrush->Release();
        m_PausedBrush->Release();
        m_Brush->Release();
        m_Render->Release();
    }

    void Window::Draw()
    {
        RECT ClientRect{};
        ::GetClientRect(m_Window, &ClientRect);
        
        auto RenderSize{ m_Render->GetSize() };
        
        m_Render->BeginDraw();
        m_Render->Clear(D2D1::ColorF(0xF9F9F9));

        m_Render->SetTransform(D2D1::Matrix3x2F::Identity());

        m_Render->DrawText(
            m_DisplayText.data(), m_DisplayText.size(),
            m_MajorFont,
            D2D1::RectF(0, 0, RenderSize.width, RenderSize.height),
            m_Paused ? m_PausedBrush : m_Brush
        );

        m_Render->DrawText(
            m_DisplayDesc.data(), m_DisplayDesc.size(),
            m_MinorFont,
            D2D1::RectF(0, 64, RenderSize.width, RenderSize.height),
            m_Paused ? m_PausedBrush : m_Brush
        );

        for (const auto& [Geometry, Color] : m_Geometries)
        {
            m_ArcBrush->SetColor(D2D1::ColorF(Color));

            m_Render->DrawGeometry(
                Geometry,
                m_Paused ? m_PausedBrush : m_ArcBrush,
                16.0f
            );
        }

        auto ClientCenterX{ float(ClientRect.right - ClientRect.left) / 2.0f };
        auto ClientCenterY{ float(ClientRect.bottom - ClientRect.top) / 2.0f };

        auto GetPointFromAngle = [&](float Angle)
        {
            Angle = Angle - (90.0f * 3.141592f / 180.0f);

            return D2D1::Point2F(
                ClientCenterX + (c_EllipseRadius - 24.0f) * std::cosf(Angle),
                ClientCenterY + (c_EllipseRadius - 24.0f) * std::sinf(Angle)
            );
        };

        float NeedleAngle{ float(m_CurTime * 6.28318f / m_StagesRuntime) };
        auto NeedlePosition{ GetPointFromAngle(NeedleAngle) };

        float NaSin{ std::sinf(NeedleAngle) };
        float NaCos{ std::cosf(NeedleAngle) };

        m_Render->SetTransform(D2D1::Matrix3x2F(
            NaCos, NaSin,
            -NaSin, NaCos,
            NeedlePosition.x, NeedlePosition.y
        ));

        m_Render->FillGeometry(m_Needle, m_Paused ? m_PausedBrush : m_Brush);

        if (m_Render->EndDraw() == D2DERR_RECREATE_TARGET)
        {
            UnloadDraw();
            ::Sleep(100);
            LoadDraw();
        }
    }

    void Window::UpdateTime(bool CalcClock)
    {
        m_CurTime = m_System.GetTime();
        
        // Format time

        auto IntTime{ static_cast<unsigned int>(m_CurTime) };

        auto Seconds{ IntTime % 60 };
        auto Minutes{ (IntTime /= 60) % 60 };
        auto Hours{ IntTime /= 60 };

        m_DisplayText.clear();
        m_DisplayText.insert(0, 64, TCHAR{});

        ::StringCchPrintf(
            m_DisplayText.data(),
            m_DisplayText.size(),
            _T("%02u:%02u:%02u"),
            Hours, Minutes, Seconds
        );

        m_DisplayText.erase(m_DisplayText.find(TCHAR{}));

        if (!CalcClock)
        {
            m_DisplayDesc = _T("...");
            m_Brush->SetColor(D2D1::ColorF(0x000000));
            return;
        }

        // Select stage

        auto PrevSelectedStage{ m_SelectedStage };
        auto StageIndex{ 0u };
        double CurAngle{};
        double CtxTime{ m_CurTime - m_BiasTime };

        for (const auto& S : m_Stages)
        {
            if (CtxTime <= S.Duration)
            {
                m_SelectedStage = StageIndex;
                m_DisplayDesc = S.Name;
                m_Brush->SetColor(D2D1::ColorF(S.Color));

                break;
            }

            CtxTime -= S.Duration;
            StageIndex++;
        }

        if (m_SelectedStage != PrevSelectedStage)
        {
            std::basic_string<TCHAR> Desc = _T("Time is ");
            Desc += m_DisplayText;

            m_NotifyPtr->ShowBallonTip(m_DisplayDesc.data(), Desc.c_str());
        }
    }

    LRESULT Window::WindowProc(UINT Message, WPARAM Wparam, LPARAM Lparam)
    {
        switch (Message)
        {            
            case WM_CLOSE:
                ::PostQuitMessage(0);
                return 0;
            
            case WM_PAINT:
            {
                Draw();
                ::ValidateRect(m_Window, nullptr);
                return 0;
            }

            case WM_ERASEBKGND:
                return TRUE;

            case WM_KEYDOWN:
            {
                switch (LOWORD(Wparam))
                {
                    case VK_ESCAPE:
                        ::ShowWindow(m_Window, SW_HIDE);
                        return 0;

                    case VK_SPACE:
                        m_System.Pause(m_Paused = !m_Paused);
                        return 0;
                    
                    default:
                        return ::DefWindowProc(m_Window, Message, Wparam, Lparam);
                }
            }

            case WM_TIMER:
                if (auto StageTime{ m_CurTime - m_BiasTime }; StageTime >= (m_StagesRuntime - 1.0 / c_TimerGranularity))
                {
                    m_BiasTime += StageTime;
                }
                
                if (Wparam == TWTID_TIMER)
                {
                    UpdateTime();

                    ::InvalidateRect(m_Window, nullptr, FALSE);
                    
                    return 0;
                }
                else
                {
                    return ::DefWindowProc(m_Window, Message, Wparam, Lparam);
                }

            case TWM_NOTIFY:
                if ((LOWORD(Lparam) == NIN_SELECT) || (LOWORD(Lparam) == NIN_KEYSELECT))
                {
                    ::ShowWindow(m_Window, SW_SHOWNORMAL);
                    ::SetFocus(m_Window);
                }

                return 0;
            
            default:
                return ::DefWindowProc(m_Window, Message, Wparam, Lparam);
        }
    }

    LRESULT Window::StaticWindowProc(HWND Wnd, UINT Message, WPARAM Wparam, LPARAM Lparam)
    {
        Window* ThisPtr{};

        if (Message == WM_NCCREATE)
        {
            ThisPtr = reinterpret_cast<Window*>(LPCREATESTRUCT(Lparam)->lpCreateParams);
            ThisPtr->m_Window = Wnd;

            ::SetWindowLongPtr(Wnd, GWLP_USERDATA, LONG_PTR(ThisPtr));
        }
        else
        {
            ThisPtr = reinterpret_cast<Window*>(::GetWindowLongPtr(Wnd, GWLP_USERDATA));
        }

        if (ThisPtr)
        {
            return ThisPtr->WindowProc(Message, Wparam, Lparam);
        }
        else
        {
            return DefWindowProc(Wnd, Message, Wparam, Lparam);
        }
    }

}
