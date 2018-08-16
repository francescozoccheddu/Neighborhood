#pragma once

#include <Game/Windows.hpp>

#if GAME_PLATFORM == GAME_PLATFORM_UWP
#include <d3d11_3.h>
#define GAME_D3D11_HEADER_VERS 3
#elif GAME_PLATFORM == GAME_PLATFORM_WIN32
#include <d3d11.h>
#define GAME_D3D11_HEADER_VERS 0
#else
#error Unknown platform
#endif

#pragma comment(lib,"d3d11")

