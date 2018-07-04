#pragma once

#include <string>
#include "Direct3D11.h"

class View
{

public:

	struct Size
	{
		int width;
		int height;
	};

	class Listener
	{

	public:

		virtual void OnTick (double deltaTime) = 0;

		virtual void OnShow () = 0;

		virtual void OnHide () = 0;

		virtual void OnSize (Size size) = 0;

		virtual void OnCreate () = 0;

		virtual void OnDestroy () = 0;

	};

	virtual Size GetScreenSize () const = 0;

	virtual Size GetSize () const = 0;

	virtual IDXGISwapChain1* CreateSwapChain (IUnknown & device, IDXGIFactory2& factory, const DXGI_SWAP_CHAIN_DESC1 & desc) const = 0;

	virtual void Close () = 0;

};

