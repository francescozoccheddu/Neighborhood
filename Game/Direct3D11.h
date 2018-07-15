#pragma once

#include <d3d11_4.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <string>

#define STRINGIZE_DETAIL(x) #x
#define STRINGIZE(x) STRINGIZE_DETAIL(x)
#ifdef _DEBUG
#define HR(hr) {if (!SUCCEEDED(hr)) {HR_THROW(hr);}}
#define HR_THROW(hr) { HRThrow(hr, "HRESULT failure\n" "File: "  __FILE__ "\n" "Line: " STRINGIZE (__LINE__) "\n" "Function: " __FUNCTION__); }
#define LOGM(x) OutputDebugStringA(__FUNCTION__ ": " x "\n");
#else
#define HR(hr) { hr; }
#define HR_THROW(hr) { hr; }
#define LOGM(x)
#endif

void HRThrow (HRESULT hr, const char * desc);

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