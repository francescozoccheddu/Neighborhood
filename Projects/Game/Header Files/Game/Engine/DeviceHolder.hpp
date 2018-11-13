#pragma once

#include <Game/Direct3D.hpp>
#include <Game/Utils/WindowRect.hpp>
#include <Game/Engine/EngineListener.hpp>
#include <vector>

#if GAME_PLATFORM == GAME_PLATFORM_UWP
#define GAME_NATIVE_WINDOW_T IUnknown*
#if GAME_D3D11_HEADER_VERS < 1
#error UWP requires D3D11.1 header
#endif
#elif GAME_PLATFORM == GAME_PLATFORM_WIN32
#define GAME_NATIVE_WINDOW_T HWND
#else
#error Unknown platform
#endif

class DeviceHolder
{

public:

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

	std::vector<EngineListener*> Listeners;

private:

	static double s_TimerFreq;
	GAME_NATIVE_WINDOW_T m_NativeWindow { nullptr };
	WindowSize m_Size { -1, -1 };
	com_ptr<ID3D11Device> m_pDevice { nullptr };
#if GAME_D3D11_HEADER_VERS >= 1
	com_ptr<IDXGISwapChain1> m_pSwapChain { nullptr };
#else
	com_ptr<IDXGISwapChain> m_pSwapChain { nullptr };
#endif
	com_ptr<ID3D11DeviceContext> m_pDeviceContext { nullptr };
	com_ptr<ID3D11RenderTargetView> m_pRenderTargetView { nullptr };
	D3D_FEATURE_LEVEL m_SupportedFeatureLevel;
	WindowRotation m_Rotation { WindowRotation::IDENTITY };

	void CreateDeviceAndDeviceContext ();

	void CreateSwapChain ();

	void CreateRenderTarget ();

	void HandleDeviceLost ();

	void ReleaseAll ();

};

