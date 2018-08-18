#pragma once

#include <Game/Windows.hpp>

#if GAME_PLATFORM == GAME_PLATFORM_UWP
#include <Xinput.h>
#pragma comment(lib,"Xinput9_1_0")
#elif GAME_PLATFORM == GAME_PLATFORM_WIN32
#include <Xinput.h>
#pragma comment(lib,"Xinput9_1_0")
#else
#error Unknown platform
#endif
