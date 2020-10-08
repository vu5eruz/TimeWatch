// Time Watch
// twNotify.cpp
// Notifications and the Notification Area

#include "_pch.h"
#include "tw.h"
#include "twNotify.h"

namespace timewatch
{

    Notify::Notify(HINSTANCE Instance, HWND Window) :
        m_Instance{ Instance },
        m_Window{ Window }
    {
        m_Data.cbSize = sizeof(m_Data);
        m_Data.hWnd = m_Window;
        m_Data.uID = 1;
        m_Data.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_SHOWTIP;
        m_Data.uCallbackMessage = TWM_NOTIFY;
        m_Data.hIcon = ::LoadIcon(Instance, MAKEINTRESOURCE(TWRID_ICON_APPLICATION));
        m_Data.hBalloonIcon = m_Data.hIcon;
        _tcscpy_s(m_Data.szTip, _T("Time Watch"));
        m_Data.uVersion = NOTIFYICON_VERSION_4;

        if (!::Shell_NotifyIcon(NIM_ADD, &m_Data))
        {
            throw std::runtime_error("Could not create Notification Area icon");
        }

        ::Shell_NotifyIcon(NIM_SETVERSION, &m_Data);
    }

    Notify::~Notify()
    {
        ::Shell_NotifyIcon(NIM_DELETE, &m_Data);
    }

    void Notify::ShowBallonTip(LPCTSTR Title, LPCTSTR Description)
    {
        m_Data.uFlags |= NIF_INFO;
        _tcscpy_s(m_Data.szInfo, Description);
        _tcscpy_s(m_Data.szInfoTitle, Title);
        m_Data.dwInfoFlags = NIIF_LARGE_ICON | NIIF_USER;

        ::Shell_NotifyIcon(NIM_MODIFY, &m_Data);
    }

}
