#pragma once

#include <Game/Direct3D.hpp>
#include <Game/Utils/WindowRect.hpp>

#define _GAME_NATIVE_WINDOW_TYPE_COREWINDOW 7
#define _GAME_NATIVE_WINDOW_TYPE_HWND 8

#if defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_APP)
#define _GAME_NATIVE_WINDOW_TYPE _GAME_NATIVE_WINDOW_TYPE_COREWINDOW
#define GAME_NATIVE_WINDOW_T IUnknown*
#elif !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
#define _GAME_NATIVE_WINDOW_TYPE _GAME_NATIVE_WINDOW_TYPE_HWND
#define GAME_NATIVE_WINDOW_T HWND
#else
#error Unknown windows API family
#endif

class DeviceHolder
{

public:

	class Listener
	{
	public:

		virtual ~Listener () = default;

	protected:

		friend class DeviceHolder;

		Listener () = default;

		virtual void OnDeviceDestroyed () = 0;

		virtual void OnDeviceCreated () = 0;

		virtual void OnSized (WindowSize size, WindowRotation rotation) = 0;

	};

	DeviceHolder ();

	virtual ~DeviceHolder ();

	void Present ();

	void Size (WindowSize size, WindowRotation rotation, bool bForce = false);

	void Destroy ();

	void SetWindow (GAME_NATIVE_WINDOW_T nativeWindow, WindowSize size, WindowRotation rotation);

	void Trim ();

	void ValidateDevice ();

	ID3D11Device * GetDevice () const;

	ID3D11DeviceContext * GetDeviceContext () const;

	ID3D11RenderTargetView * GetRenderTargetView () const;

	WindowSize GetSize () const;

	D3D_FEATURE_LEVEL GetSupportedFeatureLevel () const;

	WindowRotation GetRotation () const;

	Listener * pListener { nullptr };

private:

	static double s_TimerFreq;
	GAME_NATIVE_WINDOW_T m_NativeWindow { nullptr };
	WindowSize m_Size { -1, -1 };
	com_ptr<ID3D11Device> m_pDevice { nullptr };
	com_ptr<ID3D11DeviceContext> m_pDeviceContext { nullptr };
	com_ptr<IDXGISwapChain1> m_pSwapChain { nullptr };
	com_ptr<ID3D11RenderTargetView> m_pRenderTargetView { nullptr };
	D3D_FEATURE_LEVEL m_SupportedFeatureLevel;
	WindowRotation m_Rotation { WindowRotation::IDENTITY };

	void CreateDeviceAndDeviceContext ();

	void CreateSwapChain ();

	void CreateRenderTarget ();

	void HandleDeviceLost ();

	void ReleaseAll ();

};

