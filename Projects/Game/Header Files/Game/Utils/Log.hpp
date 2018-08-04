#pragma once

#define NOMINMAX
#include <comdef.h>

#ifdef _DEBUG
#ifndef GAME_LOG_ENABLE
#define GAME_LOG_ENABLE 1
#endif
#ifndef GAME_LOG_FILE_ENABLE
#define GAME_LOG_FILE_ENABLE 1
#endif
#ifndef GAME_LOG_FUNCTION_ENABLE
#define GAME_LOG_FUNCTION_ENABLE 1
#endif
#else
#ifndef GAME_COMC_ENABLE
#define GAME_COMC_ENABLE 0
#endif
#ifndef GAME_LOG_ENABLE
#define GAME_LOG_ENABLE 0
#endif
#ifndef GAME_LOG_FILE_ENABLE
#define GAME_LOG_FILE_ENABLE 0
#endif
#ifndef GAME_LOG_FUNCTION_ENABLE
#define GAME_LOG_FUNCTION_ENABLE 1
#endif
#endif

#if GAME_LOG_FILE_ENABLE && GAME_LOG_FUNCTION_ENABLE
#define GAME_LOGF(msg) { OutputDebugStringA( __FUNCTION__ " in file " __FILE__ " at line " _STRINGIZE(__LINE__) ": " msg "\n" ); }
#elif GAME_LOG_FILE_ENABLE
#define GAME_LOGF(msg) { OutputDebugStringA( __FILE__ " at line " _STRINGIZE(__LINE__) ": " msg "\n" ); }
#elif GAME_LOG_FUNCTION_ENABLE
#define GAME_LOGF(msg) { OutputDebugStringA( __FUNCTION__ ": " msg "\n" ); }
#else
#define GAME_LOGF(msg) { OutputDebugStringA( msg "\n" ); }
#endif

#if GAME_LOG_ENABLE
#define GAME_LOG(msg) GAME_LOGF(msg)
#else
#define GAME_LOG(msg)
#endif

