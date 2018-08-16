#pragma once

#include <Game/Windows.hpp>

#if GAME_PLATFORM == GAME_PLATFORM_UWP
#include <Xinput.h>
#pragma comment(lib,"Xinput")
#elif GAME_PLATFORM == GAME_PLATFORM_WIN32
#include <Xinput.h>
#pragma comment(lib,"Xinput")
#else
#error Unknown platform
#endif
