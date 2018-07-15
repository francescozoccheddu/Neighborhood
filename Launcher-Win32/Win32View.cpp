#include "Win32View.hpp"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "View.hpp"
#include "Direct3D11.h"
#include "Utils.hpp"

Win32View::Win32View (HWND _hWnd) : m_hWnd{ _hWnd }
{}

IDXGISwapChain1 * Win32View::CreateSwapChain (IUnknown & _device, IDXGIFactory2 & _factory, const DXGI_SWAP_CHAIN_DESC1 & desc) const
{
	IDXGISwapChain1 *swapChain;
	GAME_COMC (_factory.CreateSwapChainForHwnd (&_device, m_hWnd, &desc, nullptr, nullptr, &swapChain));
	return swapChain;
}

void Win32View::Close ()
{
	DestroyWindow (m_hWnd);
}
