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

    void App::ParseCommandLine(const std::vector<LPWSTR>& Args)
    {
        if (Args.empty())
        {
        default_settings:

            m_Window.AddStage(_T("Borealis — Work"), 0x00A3C2, 25.0 * 60.0);
            m_Window.AddStage(_T("Borealis — Break"), 0x8093AB, 5.0 * 60.0);
            m_Window.AddStage(_T("Australis — Work"), 0xBA8384, 25.0 * 60.0);
            m_Window.AddStage(_T("Australis — Break"), 0xC38F60, 5.0 * 60.0);

            return;
        }
        
        LPCTSTR Name{};
        DWORD Color{};
        double Duration{};
        unsigned int Stages{};

        for (const auto Arg : Args)
        {
            if ((Arg[0] != _T('/')) && (Arg[0] != _T('-')))
            {
                // Options starts with either '/' or '-'
                continue;
            }

            LPWSTR Value{};

            // Make option lowercase and find value

            for (std::size_t I{ 1 }; Arg[I]; ++I)
            {
                if ((Arg[I] >= _T('A')) && (Arg[I] <= _T('Z')))
                {
                    Arg[I] |= 32;
                }
                else if ((Arg[I] == _T(':')) || (Arg[I] == _T('=')))
                {
                    Value = Arg + I + 1;
                    break;
                }
            }

            // Decode option

            std::size_t ArgSvLength{ Value ? Value - Arg - 2 : _tcslen(Arg) - 1 };
            std::basic_string_view<TCHAR> ArgSv(Arg + 1, ArgSvLength);

            if (ArgSv == _T("stage"))
            {
                m_Window.AddStage(
                    Name ? Name : _T("<STAGE>"),
                    Color,
                    Duration > 0.0 ? Duration : 10.0
                );

                Stages++;
            }
            else if (ArgSv == _T("name"))
            {
                Name = Value;
            }
            else if (ArgSv == _T("dur"))
            {
                unsigned int AccValue{};

                Duration = 0.0;

                for (std::size_t I{}; Value[I]; ++I)
                {
                    TCHAR C{ Value[I] };

                    if ((C >= _T('0')) && (C <= _T('9')))
                    {
                        AccValue = (AccValue * 10) + (C - _T('0'));
                    }
                    else if ((C == _T('s')) || (C == _T('S')))
                    {
                        Duration += AccValue;
                        AccValue = 0;
                    }
                    else if ((C == _T('m')) || (C == _T('M')))
                    {
                        Duration += AccValue * 60;
                        AccValue = 0;
                    }
                    else if ((C == _T('h')) || (C == _T('H')))
                    {
                        Duration += AccValue * 60 * 60;
                        AccValue = 0;
                    }
                }
            }
            else if (ArgSv == _T("color"))
            {
                for (std::size_t I{}; Value[I]; ++I)
                {
                    TCHAR C{ Value[I] };

                    if ((C >= _T('0')) && (C <= _T('9')))
                    {
                        Color = (Color * 16) + (C - _T('0'));
                    }
                    else if ((C >= _T('a')) && (C <= _T('f')))
                    {
                        Color = (Color * 16) + (C - _T('a')) + 10;
                    }
                    else if ((C >= _T('A')) && (C <= _T('F')))
                    {
                        Color = (Color * 16) + (C - _T('A')) + 10;
                    }
                }
            }
        }

        if (!Stages)
        {
            goto default_settings;
        }
    }

    int App::Run()
    {
        MSG Message{};

        m_Window.FreezeStages();

        while (::GetMessage(&Message, nullptr, 0, 0) > 0)
        {
            ::TranslateMessage(&Message);
            ::DispatchMessage(&Message);
        }
        
        return int(Message.wParam);
    }

}
