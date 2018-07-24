#pragma once

#include "Utils.hpp"
#include "Direct3D11.hpp"

#define GAME_PLATFORM_WIN32 4
#define GAME_PLATFORM_UWP 5

#if GAME_PLATFORM == GAME_PLATFORM_WIN32
typedef HWND NativeWindow;
#elif GAME_PLATFORM == GAME_PLATFORM_UWP
typedef IUnknown* NativeWindow;
#else
#error Unknown GAME_PLATFORM value.
#endif

struct WindowSize
{
	int width;
	int height;
};


class ResourceHandler
{

public:

	ResourceHandler ();

	virtual ~ResourceHandler ();

	void OnTick ();

	void OnSize (WindowSize size);

	void OnDestroy ();

	void OnWindowChanged (NativeWindow nativeWindow);

	void OnSuspended ();

	virtual void OnShow () = 0;

	virtual void OnHide () = 0;

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

	static double QueryTimerFrequency ();

	LARGE_INTEGER m_LastTime;
	const double timerFreq;
	NativeWindow m_NativeWindow{};
	WindowSize m_Size{-1,-1};
	ID3D11Device * m_pDevice{ nullptr };
	ID3D11DeviceContext1 * m_pDeviceContext{ nullptr };
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

