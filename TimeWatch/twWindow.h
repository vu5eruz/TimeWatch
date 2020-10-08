// Time Watch
// twWindow.h
// Application Main Window Frame

#pragma once

namespace timewatch
{

    class Notify;
    class System;
    class Window;

    struct Stage
    {
        LPCTSTR Name;
        DWORD Color;
        double Duration;
    };

    class Window
    {
        friend class WindowDraw;

    public:

        Window(System& SystemRef);

        ~Window();

        inline void SetNotifyPtr(Notify* Ptr) { m_NotifyPtr = Ptr; }

        inline auto Handle() const { return m_Window; }

        inline auto AddStage(LPCTSTR Name, DWORD Color, double Duration)
        {
            m_Stages.emplace_back(Stage{ Name, Color, Duration });
            m_StagesRuntime += Duration;
        }

        void FreezeStages();

    private:

        void LoadDraw();

        void UnloadDraw();

        void Draw();

        void UpdateTime(bool CalcClock = true);

        LRESULT WindowProc(UINT Message, WPARAM Wparam, LPARAM Lparam);

        static LRESULT CALLBACK StaticWindowProc(HWND Window, UINT Message, WPARAM Wparam, LPARAM Lparam);

        System& m_System;
        HWND m_Window{};
        Notify* m_NotifyPtr{};
        std::basic_string<TCHAR> m_DisplayText{};
        std::basic_string_view<TCHAR> m_DisplayDesc{};

        bool m_Paused{};
        double m_CurTime{};
        double m_BiasTime{};
        unsigned int m_SelectedStage{};
        std::vector<Stage> m_Stages{};
        double m_StagesRuntime{};

        ID2D1HwndRenderTarget* m_Render{};
        ID2D1PathGeometry* m_Needle{};
        ID2D1SolidColorBrush* m_Brush{};
        ID2D1SolidColorBrush* m_PausedBrush{};
        ID2D1SolidColorBrush* m_ArcBrush{};
        IDWriteTextFormat* m_MajorFont{};
        IDWriteTextFormat* m_MinorFont{};

        std::vector<std::pair<ID2D1PathGeometry*, DWORD>> m_Geometries{};
    };

}
