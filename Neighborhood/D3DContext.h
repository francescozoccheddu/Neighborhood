#pragma once

#include <d3d11.h>

class D3DContext
{

private:

	ID3D11Device * device{ nullptr };
	ID3D11DeviceContext * context{ nullptr };
	IDXGISwapChain * swapChain{ nullptr };
	ID3D11RenderTargetView * targetWiew{ nullptr };
	D3D_FEATURE_LEVEL featureLevel;
	D3D11_VIEWPORT viewport;

public:

	D3DContext ();

	void Create (HWND hwnd);

	void Destroy ();

	bool IsCreated ();

	virtual ~D3DContext ();

};

