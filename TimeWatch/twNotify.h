// Time Watch
// twNotify.h
// Notifications and the Notification Area

#pragma once

namespace timewatch
{

    class Notify
    {
    public:

        Notify(HINSTANCE Instance, HWND Window);

        ~Notify();

        void ShowBallonTip(LPCTSTR Title, LPCTSTR Description);

        void UpdateTip(LPCTSTR Tip);

    private:

        HINSTANCE m_Instance{};
        HWND m_Window{};
        NOTIFYICONDATA m_Data{};
    };

}
