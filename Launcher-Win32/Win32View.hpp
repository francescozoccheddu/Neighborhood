#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windef.h>
#include <Framework/View.hpp>
#include <Framework/Direct3D11.h>

class Win32View : public View
{

public:
	
	Win32View (HWND hWnd);
	
	virtual IDXGISwapChain1 * CreateSwapChain (IUnknown & device, IDXGIFactory2 & factory, const DXGI_SWAP_CHAIN_DESC1 & desc) const override;

	virtual void Close () override;

private:

	HWND m_hWnd;

};

