// Time Watch
// _pch.h
// Precompiled header file

#pragma once

#define _WIN32_WINDOWS _WIN32_WINNT_WIN7
#define _WIN32_WINNT _WIN32_WINDOWS
#define GDIPVER 0x0110
#define STRICT
#define NTDDI_VERSION NTDDI_WIN7
#define WINVER _WIN32_WINNT

#pragma warning(push, 2)

#include <windows.h>

#include <assert.h>
#include <commctrl.h>
#include <crtdbg.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <limits.h>
#include <objidl.h>
#include <shellapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <strsafe.h>
#include <tchar.h>
#include <wchar.h>
#include <windowsx.h>

#undef min
#undef max

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cwchar>
#include <iostream>
#include <exception>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#pragma warning(pop)
