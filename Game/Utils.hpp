#pragma once

#include <comdef.h>
#include <string>
#include "Exceptions.hpp"

#ifdef _DEBUG
#ifndef GAME_COMC_ENABLE
#define GAME_COMC_ENABLE 1
#endif
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
#define GAME_LOGF(msg) { OutputDebugStringA( __FUNCTION__ " in file " __FILE__ " at line " _STRINGIZE(__LINE__) ": " msg ); }
#elif GAME_LOG_FILE_ENABLE
#define GAME_LOGF(msg) { OutputDebugStringA( __FILE__ " at line " _STRINGIZE(__LINE__) ": " msg ); }
#elif GAME_LOG_FUNCTION_ENABLE
#define GAME_LOGF(msg) { OutputDebugStringA( __FUNCTION__ ": " msg ); }
#else
#define GAME_LOGF(msg) { OutputDebugStringA( msg ); }
#endif

#if GAME_LOG_ENABLE
#define GAME_LOG(msg) GAME_LOGF(msg)
#else
#define GAME_LOG(msg)
#endif

#if GAME_COMC_ENABLE
#define GAME_COMC_MSG(res,msg) { ThrowIfCOMFailed(res,msg); }
#else
#define GAME_COMC_MSG(res,msg) { res; }
#endif

#define GAME_COMC(res) GAME_COMC_MSG(res,nullptr)

void ThrowIfCOMFailed (HRESULT result, const char * message);

char * LoadBlob (const std::string& filename, size_t& length);

template<typename T>
void ReleaseCOM (T& comPtr)
{
	if (comPtr)
	{
		comPtr->Release ();
		comPtr = nullptr;
	}
}