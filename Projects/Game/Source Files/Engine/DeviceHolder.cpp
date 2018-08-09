#include <Game/Engine/DeviceHolder.hpp>

#include <Game/Utils/Exceptions.hpp>
#include <Game/Utils/COMExceptions.hpp>

#define SWAP_CHAIN_FORMAT DXGI_FORMAT_R8G8B8A8_UNORM
#define FIRE_EVENT(x) { if (pListener) pListener -> x ; }

DeviceHolder::DeviceHolder () {}

DeviceHolder::~DeviceHolder ()
{
	ReleaseAll ();
}

void DeviceHolder::Present ()
{
	DXGI_PRESENT_PARAMETERS pars;
	pars.DirtyRectsCount = 0;
	pars.pDirtyRects = nullptr;
	pars.pScrollOffset = nullptr;
	pars.pScrollRect = nullptr;
	const HRESULT hResPresent { m_pSwapChain->Present1 (1, 0, &pars) };

	com_ptr<ID3D11DeviceContext1> pDeviceContext1;
	m_pDeviceContext.try_as (pDeviceContext1);
	if (pDeviceContext1)
	{
		pDeviceContext1->DiscardView1 (m_pRenderTargetView.get (), nullptr, 0);
	}

	if (hResPresent == DXGI_ERROR_DEVICE_REMOVED || hResPresent == DXGI_ERROR_DEVICE_RESET)
	{
		HandleDeviceLost ();
	}
	else
	{
		GAME_COMC (hResPresent);
	}
}

void DeviceHolder::Size (WindowSize _size, DXGI_MODE_ROTATION _rotation, bool _bForce)
{
	if (_bForce || _size != m_Size || _rotation != m_Rotation)
	{
		m_pRenderTargetView = nullptr;
		m_Size = _size;
		m_Rotation = _rotation;
		bool recreated { false };
		if (m_pSwapChain)
		{
			const HRESULT hr { m_pSwapChain->ResizeBuffers (2, _size.width, _size.height, SWAP_CHAIN_FORMAT, 0) };
			if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
			{
				HandleDeviceLost ();
				recreated = true;
			}
			else
			{
				GAME_COMC (hr);
			}
		}
		else
		{
			CreateSwapChain ();
		}
		GAME_COMC (m_pSwapChain->SetRotation (m_Rotation));
		if (!recreated)
		{
			CreateRenderTarget ();
		}
		FIRE_EVENT (OnSized (_size, _rotation));
	}
}

void DeviceHolder::Destroy ()
{
	ReleaseAll ();
	FIRE_EVENT (OnDeviceDestroyed ());
}

void DeviceHolder::SetWindow (GAME_NATIVE_WINDOW_T _window, WindowSize _size, DXGI_MODE_ROTATION _rotation)
{
	if (!m_pDevice)
	{
		CreateDeviceAndDeviceContext ();
		FIRE_EVENT (OnDeviceCreated ());
	}
	m_NativeWindow = _window;
	m_pSwapChain = nullptr;
	m_pRenderTargetView = nullptr;
	m_pDeviceContext->ClearState ();
	m_pDeviceContext->Flush ();
	Size (_size, _rotation, true);
}

void DeviceHolder::Trim ()
{
	if (m_pDevice)
	{
		m_pDeviceContext->ClearState ();
		com_ptr<IDXGIDevice3> pDevice;
		if (m_pDevice.try_as (pDevice))
		{
			pDevice->Trim ();
		}
	}
}

void DeviceHolder::ValidateDevice ()
{
	com_ptr<IDXGIFactory2> pFactory;
	{
		com_ptr<IDXGIDevice> pDevice;
		com_ptr<IDXGIAdapter> pAdapter;
		m_pDevice.as (pDevice);
		pDevice->GetAdapter (pAdapter.put ());
		pAdapter->GetParent (IID_PPV_ARGS (pFactory.put ()));
	}
	DXGI_ADAPTER_DESC previousDesc;
	{
		com_ptr<IDXGIAdapter1> previousDefaultAdapter;
		GAME_COMC (pFactory->EnumAdapters1 (0, previousDefaultAdapter.put ()));
		GAME_COMC (previousDefaultAdapter->GetDesc (&previousDesc));
	}

	DXGI_ADAPTER_DESC currentDesc;
	{
		com_ptr<IDXGIFactory2> currentFactory;
		GAME_COMC (CreateDXGIFactory2 (0, IID_PPV_ARGS (currentFactory.put ())));

		com_ptr<IDXGIAdapter1> currentDefaultAdapter;
		GAME_COMC (currentFactory->EnumAdapters1 (0, currentDefaultAdapter.put ()));

		GAME_COMC (currentDefaultAdapter->GetDesc (&currentDesc));
	}

	if (previousDesc.AdapterLuid.LowPart != currentDesc.AdapterLuid.LowPart
		|| previousDesc.AdapterLuid.HighPart != currentDesc.AdapterLuid.HighPart
		|| FAILED (m_pDevice->GetDeviceRemovedReason ()))
	{
		HandleDeviceLost ();
	}
}

ID3D11Device * DeviceHolder::GetDevice () const
{
	return m_pDevice.get ();
}

ID3D11DeviceContext * DeviceHolder::GetDeviceContext () const
{
	return m_pDeviceContext.get ();
}

ID3D11RenderTargetView * DeviceHolder::GetRenderTargetView () const
{
	return m_pRenderTargetView.get ();
}

WindowSize DeviceHolder::GetSize () const
{
	return m_Size;
}

D3D_FEATURE_LEVEL DeviceHolder::GetSupportedFeatureLevel () const
{
	return m_SupportedFeatureLevel;
}

DXGI_MODE_ROTATION DeviceHolder::GetRotation () const
{
	return m_Rotation;
}

void DeviceHolder::CreateDeviceAndDeviceContext ()
{
	const D3D_FEATURE_LEVEL featureLevels[] { D3D_FEATURE_LEVEL_11_1 };
	UINT flags { D3D11_CREATE_DEVICE_SINGLETHREADED };
#ifdef _DEBUG
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	HRESULT hr { D3D11CreateDevice (nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, featureLevels, ARRAYSIZE (featureLevels), D3D11_SDK_VERSION, m_pDevice.put (), &m_SupportedFeatureLevel, m_pDeviceContext.put ()) };
	if (FAILED (hr))
	{
		GAME_COMC (D3D11CreateDevice (nullptr, D3D_DRIVER_TYPE_WARP, nullptr, 0, featureLevels, ARRAYSIZE (featureLevels), D3D11_SDK_VERSION, m_pDevice.put (), &m_SupportedFeatureLevel, m_pDeviceContext.put ()));
	}
}

void DeviceHolder::CreateSwapChain ()
{
	m_pSwapChain = nullptr;
	com_ptr<IDXGIFactory2> pFactory;
	{
		com_ptr<IDXGIDevice> pDevice;
		com_ptr<IDXGIAdapter> pAdapter;
		m_pDevice.as (pDevice);
		pDevice->GetAdapter (pAdapter.put ());
		pAdapter->GetParent (IID_PPV_ARGS (pFactory.put ()));
	}

	DXGI_SWAP_CHAIN_DESC1 desc;
	desc.BufferCount = 2;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Width = m_Size.width;
	desc.Height = m_Size.height;
	desc.Scaling = DXGI_SCALING_STRETCH;
	desc.Format = SWAP_CHAIN_FORMAT;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.Stereo = FALSE;
	desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	desc.Flags = 0;
#if _GAME_NATIVE_WINDOW_TYPE == _GAME_NATIVE_WINDOW_TYPE_HWND
	desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	GAME_COMC (pFactory->CreateSwapChainForHwnd (m_pDevice.get (), m_NativeWindow, &desc, nullptr, nullptr, m_pSwapChain.put ()));
#elif _GAME_NATIVE_WINDOW_TYPE == _GAME_NATIVE_WINDOW_TYPE_COREWINDOW
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	GAME_COMC (pFactory->CreateSwapChainForCoreWindow (m_pDevice.get (), m_NativeWindow, &desc, nullptr, m_pSwapChain.put ()));
#else
#error Unknown native window type
#endif
}

void DeviceHolder::CreateRenderTarget ()
{
	m_pRenderTargetView = nullptr;
	com_ptr<ID3D11Texture2D> pTexture;
	GAME_COMC (m_pSwapChain->GetBuffer (0, __uuidof(ID3D11Texture2D), pTexture.put_void ()));
	GAME_COMC (m_pDevice->CreateRenderTargetView (pTexture.get (), nullptr, m_pRenderTargetView.put ()));
}

void DeviceHolder::HandleDeviceLost ()
{
	ReleaseAll ();
	FIRE_EVENT (OnDeviceDestroyed ());
	CreateDeviceAndDeviceContext ();
	FIRE_EVENT (OnDeviceCreated ());
	CreateSwapChain ();
	CreateRenderTarget ();
}

void DeviceHolder::ReleaseAll ()
{
	m_pRenderTargetView = nullptr;
	m_pSwapChain = nullptr;
	m_pDevice = nullptr;
	if (m_pDeviceContext)
	{
		m_pDeviceContext->ClearState ();
		m_pDeviceContext->Flush ();
		m_pDeviceContext = nullptr;
	}
}
