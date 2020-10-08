// Time Watch
// tw.cpp
// WinMain

#include "_pch.h"
#include "tw.h"
#include "twApp.h"

int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
#ifdef DEBUG
	::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	try
	{
		timewatch::App App{};
		
		return App.Run();
	}
	catch (const std::exception& Exception)
	{
		::FatalAppExitA(0, Exception.what());
	}

	return EXIT_FAILURE;
}
