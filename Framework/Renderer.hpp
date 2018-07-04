#pragma once

#include "View.hpp"
#include "Direct3D11.h"

class Renderer : public View::Listener
{

public:

	Renderer (View& view);

	virtual ~Renderer ();

	virtual void OnTick (double deltaTime) override final;

	virtual void OnSize (View::Size size) override final;

	virtual void OnCreate () override final;

	virtual void OnDestroy () override final;

protected:

	virtual void OnDeviceDestroyed () = 0;

	virtual void OnDeviceCreated () = 0;

	virtual void OnRender (double deltaTime) = 0;

	virtual void OnSized (View::Size size) = 0;

	ID3D11Device * GetDevice ();

	ID3D11DeviceContext * GetDeviceContext ();

	ID3D11RenderTargetView * GetRenderTargetView ();

	ID3D11DepthStencilView * GetDepthStencilView ();

private:

	View& m_View;
	ID3D11Device * m_pDevice{ nullptr };
	ID3D11DeviceContext * m_pDeviceContext{ nullptr };
	IDXGISwapChain1 * m_pSwapChain{ nullptr };
	ID3D11RenderTargetView * m_pRenderTargetView{ nullptr };
	ID3D11DepthStencilView * m_pDepthStencilView{ nullptr };
	D3D_FEATURE_LEVEL m_supportedFeatureLevel;

	void CreateDeviceAndDeviceContext ();

	void CreateSwapChain (View::Size bufferSize);

	void CreateRenderTarget (View::Size viewSize);

	void CreateDepthStencilView (View::Size viewSize);

	void SetOutputMergerViews ();

	void SetOutputMergerViewport (View::Size viewportSize);

	void HandleDeviceLost ();

};

