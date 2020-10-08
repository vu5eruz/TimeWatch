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
		int Argc{};
		LPWSTR* Argv{ ::CommandLineToArgvW(::GetCommandLine(), &Argc) };
		std::vector<LPWSTR> Args(Argv + 1, Argv + Argc);

		if (!Args.empty() && !_tcscmp(Args[0], _T("/?")))
		{
			::MessageBox(
				nullptr,
				_T(
					"/?\tShow Help\r\n"
					"/name:...\tSet stage name\r\n"
					"/dur:...\tSet stage duration\r\n"
					"/color:...\tSet stage color\r\n"
					"/stage\tAdd stage"
				),
				_T("Time Watch Help"),
				MB_OK
			);

			return EXIT_SUCCESS;
		}

		timewatch::App App{};
		App.ParseCommandLine(Args);
		return App.Run();
	}
	catch (const std::exception& Exception)
	{
		::FatalAppExitA(0, Exception.what());
	}

	return EXIT_FAILURE;
}
