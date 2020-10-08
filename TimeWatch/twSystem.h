// Time Watch
// twSystem.h
// System Initialization

#pragma once

namespace timewatch
{

    class System
    {
    public:

        System();

        ~System();

        ID2D1HwndRenderTarget* CreateHwndRenderTarget(HWND Window, unsigned int Width, unsigned int Height) const;

        ID2D1PathGeometry* CreatePathGeometry() const;

        IDWriteTextFormat* CreateTextFormat(LPCTSTR FontName, FLOAT FontSize, DWRITE_FONT_WEIGHT FontWeight = DWRITE_FONT_WEIGHT_NORMAL) const;

        double GetTime() const noexcept;

        void Pause(bool Paused) noexcept;

        inline auto Instance() const { return m_Instance; }

    private:

        HINSTANCE m_Instance{};
        ID2D1Factory* m_D2D{};
        IDWriteFactory* m_DWrite{};
        LARGE_INTEGER m_QpcFrequency{};
        LARGE_INTEGER m_QpcEpoch{};
        LARGE_INTEGER m_QpcPauseEpoch{};
        bool m_Paused{};
    };

}
