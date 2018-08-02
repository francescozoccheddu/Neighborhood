#pragma once

#include <Game/Utils/Direct3D11.hpp>

struct WindowSize
{
	int width;
	int height;
};

class ResourceHandler
{

public:

	enum class Platform
	{
		Win32, UWP
	};

	class Listener
	{
	public:

		virtual ~Listener () = default;

	protected:

		friend class ResourceHandler;

		Listener () = default;

		virtual void OnDeviceDestroyed () = 0;

		virtual void OnDeviceCreated () = 0;

		virtual void OnRender (float deltaTime) = 0;

		virtual void OnSized (WindowSize size) = 0;

	};

	ResourceHandler ();

	virtual ~ResourceHandler ();

	void Tick ();

	void Size (WindowSize size);

	void Destroy ();

	void SetWindow (void * nativeWindow, Platform ePlatform);

	void Trim ();

	ID3D11Device * GetDevice ();

	ID3D11DeviceContext * GetDeviceContext ();

	ID3D11RenderTargetView * GetRenderTargetView ();

	ID3D11DepthStencilView * GetDepthStencilView ();

	WindowSize GetSize () const;

	Listener * pListener { nullptr };

private:

	static double QueryTimerFrequency ();

	Platform m_ePlatform;
	LARGE_INTEGER m_LastTime;
	const double timerFreq;
	void * m_NativeWindow { nullptr };
	WindowSize m_Size { -1, -1 };
	ID3D11Device * m_pDevice { nullptr };
	ID3D11DeviceContext1 * m_pDeviceContext { nullptr };
	IDXGISwapChain1 * m_pSwapChain { nullptr };
	ID3D11RenderTargetView * m_pRenderTargetView { nullptr };
	ID3D11DepthStencilView * m_pDepthStencilView { nullptr };
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

