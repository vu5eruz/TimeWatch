// Time Watch
// twApp.h
// Application entry point

#include "_pch.h"
#include "tw.h"
#include "twApp.h"
#include "twNotify.h"
#include "twSystem.h"
#include "twWindow.h"

namespace timewatch
{
    
    App::App() :
        m_System{},
        m_Window{ m_System },
        m_Notify{ m_System.Instance(), m_Window.Handle() }
    {
        m_Window.SetNotifyPtr(&m_Notify);
    }

    App::~App()
    {

    }

    int App::Run()
    {
        MSG Message{};

        m_Window.AddStage(_T("Borealis — Work"), 0x00A3C2, 25.0 * 60.0);
        m_Window.AddStage(_T("Borealis — Break"), 0x8093AB, 5.0 * 60.0);
        m_Window.AddStage(_T("Australis — Work"), 0xBA8384, 25.0 * 60.0);
        m_Window.AddStage(_T("Australis — Break"), 0xC38F60, 5.0 * 60.0);
        m_Window.FreezeStages();

        while (::GetMessage(&Message, nullptr, 0, 0) > 0)
        {
            ::TranslateMessage(&Message);
            ::DispatchMessage(&Message);
        }
        
        return int(Message.wParam);
    }

}
