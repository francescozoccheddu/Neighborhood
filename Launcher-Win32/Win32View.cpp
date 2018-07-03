#include "Win32View.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Framework/View.h>
#include <d3d11_4.h>

Win32View::Win32View (HWND _hWnd) : hWnd{_hWnd}
{}

View::Size Win32View::GetScreenSize () const
{
	return Size ();
}

View::Size Win32View::GetSize () const
{
	RECT rect;
	GetClientRect (hWnd, &rect);
	return View::Size{ rect.right - rect.left, rect.bottom - rect.top };
}

IDXGISwapChain4 * Win32View::CreateSwapChain (const IUnknown & _device, const IDXGIFactory5 & _factory) const
{
	return nullptr;
}

void Win32View::Close ()
{
	DestroyWindow (hWnd);
}
 