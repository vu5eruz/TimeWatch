// Time Watch
// twApp.h
// Application entry point

#pragma once

#include "twNotify.h"
#include "twSystem.h"
#include "twWindow.h"

namespace timewatch
{

    class App
    {
    public:

        App();

        ~App();

        int Run();

    private:

        System m_System;
        Window m_Window;
        Notify m_Notify;
    };

}
