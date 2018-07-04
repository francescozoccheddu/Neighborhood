#pragma once

#pragma comment(lib, "d3d11.lib")

#include <d3d11_4.h>
#include <comdef.h>
#include <DirectXMath.h>
#include <DirectXColors.h>

#define STRINGIZE_DETAIL(x) #x
#define STRINGIZE(x) STRINGIZE_DETAIL(x)
#define HR_THROW(hr) { HRThrow(hr, "HRESULT failure\n" "File: "  __FILE__ "\n" "Line: " STRINGIZE (__LINE__) "\n" "Function: " __FUNCTION__); }
#define HR(hr) {if (!SUCCEEDED(hr)) {HR_THROW(hr);}}

inline void HRThrow (HRESULT _hr, const char * _desc)
{
	const TCHAR * hrMsg{ _com_error (_hr).ErrorMessage () };
	std::string msg{ _desc };
	msg += "\nMessage: ";
#ifndef UNICODE
	msg += std::string{ hrMsg };
#else
	std::wstring hrWMsg{ hrMsg };
	msg += std::string{ hrWMsg.begin (), hrWMsg.end () };
#endif
	throw std::runtime_error (msg);
}

template<typename T>
void ReleaseCOM (T& comPtr)
{
	if (comPtr)
	{
		comPtr->Release ();
		comPtr = nullptr;
	}
}