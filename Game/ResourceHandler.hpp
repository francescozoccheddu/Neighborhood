#pragma once

#include "WindowListener.hpp"
#include "Utils.hpp"
#include "Direct3D11.h"

#define GAME_PLATFORM_WIN32 4
#define GAME_PLATFORM_UWP 5

#if GAME_PLATFORM == GAME_PLATFORM_WIN32
typedef HWND NativeWindow;
#elif GAME_PLATFORM == GAME_PLATFORM_UWP
typedef IUnknown* NativeWindow;
#else
#error Unknown GAME_PLATFORM value.
#endif

class ResourceHandler : public WindowListener
{

public:

	ResourceHandler (NativeWindow nativeWindow);

	virtual ~ResourceHandler ();

	virtual void OnTick (double deltaTime) override final;

	virtual void OnSize (WindowSize size) override final;

	virtual void OnCreate () override final;

	virtual void OnDestroy () override final;

protected:

	virtual void OnDeviceDestroyed () = 0;

	virtual void OnDeviceCreated () = 0;

	virtual void OnRender (double deltaTime) = 0;

	virtual void OnSized (WindowSize size) = 0;

	ID3D11Device * GetDevice ();

	ID3D11DeviceContext * GetDeviceContext ();

	ID3D11RenderTargetView * GetRenderTargetView ();

	ID3D11DepthStencilView * GetDepthStencilView ();

	WindowSize GetSize () const;

private:

	NativeWindow m_NativeWindow;
	WindowSize m_Size;
	ID3D11Device * m_pDevice{ nullptr };
	ID3D11DeviceContext * m_pDeviceContext{ nullptr };
	IDXGISwapChain1 * m_pSwapChain{ nullptr };
	ID3D11RenderTargetView * m_pRenderTargetView{ nullptr };
	ID3D11DepthStencilView * m_pDepthStencilView{ nullptr };
	D3D_FEATURE_LEVEL m_supportedFeatureLevel;

	void CreateDeviceAndDeviceContext ();

	void CreateSwapChain (WindowSize bufferSize);

	void CreateRenderTarget (WindowSize viewSize);

	void CreateDepthStencilView (WindowSize viewSize);

	void SetOutputMergerViews ();

	void SetOutputMergerViewport (WindowSize viewportSize);

	void HandleDeviceLost ();

	void Release ();

};

