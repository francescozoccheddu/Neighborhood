#pragma once

#include <pch.h>

#define _GAME_NATIVE_WINDOW_TYPE_COREWINDOW 7
#define _GAME_NATIVE_WINDOW_TYPE_HWND 8

#if defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_APP)
#define _GAME_NATIVE_WINDOW_TYPE _GAME_NATIVE_WINDOW_TYPE_COREWINDOW
#define GAME_NATIVE_WINDOW_T CoreWindow&
#elif !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
#define _GAME_NATIVE_WINDOW_TYPE _GAME_NATIVE_WINDOW_TYPE_HWND
#define GAME_NATIVE_WINDOW_T HWND
#else
#error Unknown windows API family
#endif

struct WindowSize
{
	int width;
	int height;

	inline bool operator == (const WindowSize& other) const
	{
		return width == other.width && height == other.height;
	}

	inline bool operator != (const WindowSize& other) const
	{
		return width != other.width || height != other.height;
	}

};

class ResourceHandler
{

public:

	class Listener
	{
	public:

		virtual ~Listener () = default;

	protected:

		friend class ResourceHandler;

		Listener () = default;

		virtual void OnDeviceDestroyed () = 0;

		virtual void OnDeviceCreated () = 0;

		virtual void OnRender (double deltaTime) = 0;

		virtual void OnSized (WindowSize size) = 0;

	};

	static void InitializeTimer ();

	ResourceHandler ();

	virtual ~ResourceHandler ();

	void Tick ();

	void Size (WindowSize size, bool bForce = false);

	void Destroy ();

	void SetWindow (GAME_NATIVE_WINDOW_T nativeWindow, WindowSize size);

	void Trim ();

	void InvalidateTimer ();

	ID3D11Device * GetDevice ();

	ID3D11DeviceContext * GetDeviceContext ();

	ID3D11RenderTargetView * GetRenderTargetView ();

	ID3D11DepthStencilView * GetDepthStencilView ();

	WindowSize GetSize () const;

	D3D_FEATURE_LEVEL GetSupportedFeatureLevel () const;

	Listener * pListener { nullptr };

private:

	static double s_TimerFreq;

	bool m_LastTimeValid { false };
	LARGE_INTEGER m_LastTime;
	void * m_NativeWindow { nullptr };
	WindowSize m_Size { -1, -1 };
	com_ptr<ID3D11Device> m_pDevice { nullptr };
	com_ptr<ID3D11DeviceContext> m_pDeviceContext { nullptr };
	com_ptr<IDXGISwapChain1> m_pSwapChain { nullptr };
	com_ptr<ID3D11RenderTargetView> m_pRenderTargetView { nullptr };
	com_ptr<ID3D11DepthStencilView> m_pDepthStencilView { nullptr };
	D3D_FEATURE_LEVEL m_SupportedFeatureLevel;

	void CreateDeviceAndDeviceContext ();

	void CreateSwapChain ();

	void CreateRenderTarget ();

	void CreateDepthStencilView ();

	void SetOutputMergerViews ();

	void SetOutputMergerViewport ();

	void HandleDeviceLost ();

	void ReleaseAll ();

	void ReleaseSizeDependentResources ();

};

