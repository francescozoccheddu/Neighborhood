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
