#pragma once

#include <Game/Windows.hpp>

#if GAME_PLATFORM == GAME_PLATFORM_UWP
#include <d3d11_3.h>
#elif GAME_PLATFORM == GAME_PLATFORM_WIN32
#include <d3d11.h>
#else
#error Unknown platform
#endif

#pragma comment(lib,"d3d11")
