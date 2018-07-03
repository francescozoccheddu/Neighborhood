#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windef.h>
#include <Framework/View.h>
#include <d3d11_4.h>

class Win32View : public View
{

public:
	
	Win32View (HWND hWnd);
	
	virtual Size GetScreenSize () const override;

	virtual Size GetSize () const override;

	virtual IDXGISwapChain4 * CreateSwapChain (const IUnknown & device, const IDXGIFactory5 & factory) const override;

	virtual void Close () override;

private:

	HWND hWnd;

};

