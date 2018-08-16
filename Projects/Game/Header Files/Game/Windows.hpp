#pragma once

#include <WinSDKVer.h>
#include <SDKDDKVer.h>

#define NOMINMAX
#define NODRAWTEXT
#define NOGDI
#define NOBITMAP
#define NOMCX
#define NOSERVICE
#define NOHELP
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <winrt/base.h>

using namespace winrt;

#pragma comment(lib,"windowsapp")

#define GAME_PLATFORM_UWP 5
#define GAME_PLATFORM_WIN32 6

#if defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_APP)
#define GAME_PLATFORM GAME_PLATFORM_UWP
#define GAME_PLATFORM_IS_UWP 1
#elif !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
#define GAME_PLATFORM GAME_PLATFORM_WIN32
#define GAME_PLATFORM_IS_WIN32 1
#else
#error Unknown windows API family
#endif