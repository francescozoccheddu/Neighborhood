#pragma once

#include "View.h"
#include <d3d11_4.h>

class Renderer : public View::Listener
{

public:

	virtual ~Renderer ();

	virtual void Render (float deltaTime) override;

	virtual void Show () override;

	virtual void Hide () override;

	virtual void Size (View::Size size) override;

	virtual void Create () override;

	virtual void Destroy () override;

private:

	ID3D11Device5 * device;
	ID3D11DeviceContext4 * context;
	IDXGISwapChain4 * swapChain;
	
};

