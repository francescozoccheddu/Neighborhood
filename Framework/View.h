#pragma once

#include <string>
#include <d3d11_4.h>

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

		virtual void Render (float deltaTime) = 0;

		virtual void Show () = 0;

		virtual void Hide () = 0;

		virtual void Size (Size size) = 0;

		virtual void Create () = 0;

		virtual void Destroy () = 0;

	};

	Listener& listener;

	View (Listener& listener);

	virtual Size GetScreenSize () const = 0;

	virtual Size GetSize () const = 0;

	virtual IDXGISwapChain4* CreateSwapChain (const IDXGIFactory5& factory, const DXGI_SWAP_CHAIN_DESC1& desc) const = 0;

	virtual void Close () = 0;

};

