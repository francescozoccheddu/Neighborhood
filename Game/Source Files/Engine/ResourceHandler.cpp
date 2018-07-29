#include <Game/Engine/ResourceHandler.hpp>
#include <Game/Utils/Exceptions.hpp>
#include <Game/Utils/COMExceptions.hpp>
#include <Game/Utils/Direct3D11.hpp>
#include <WinBase.h>

#define SWAP_CHAIN_FORMAT DXGI_FORMAT_R8G8B8A8_UNORM
#define MAX_DELTA_TIME 1.0
#define FIRE_EVENT(x) { if (pListener) pListener -> x ; }

ResourceHandler::ResourceHandler () : timerFreq{ QueryTimerFrequency () }
{
	m_LastTime.QuadPart = 0;
}

ResourceHandler::~ResourceHandler ()
{
	Release ();
}

void ResourceHandler::Tick ()
{
	if (!m_pSwapChain)
	{
		throw std::runtime_error ("Swap chain not created");
	}
	double deltaTime = 0.0;
	{
		LARGE_INTEGER newTime;
		if (!QueryPerformanceCounter (&newTime))
		{
			GAME_THROW_MSG ("Timer query failed");
		}
		if (m_LastTime.QuadPart == 0.0)
		{
			deltaTime = 0.0;
		}
		else
		{
			deltaTime = static_cast<double>(newTime.QuadPart - m_LastTime.QuadPart) / timerFreq;
			if (deltaTime < 0.0)
			{
				deltaTime = 0.0;
			}
			else if (deltaTime > MAX_DELTA_TIME)
			{
				deltaTime = MAX_DELTA_TIME;
			}
		}
		m_LastTime = newTime;
	}
	FIRE_EVENT (OnRender (static_cast<float>(deltaTime)));
	DXGI_PRESENT_PARAMETERS pars;
	pars.DirtyRectsCount = 0;
	pars.pDirtyRects = nullptr;
	pars.pScrollOffset = nullptr;
	pars.pScrollRect = nullptr;
	const HRESULT hr{ m_pSwapChain->Present1 (1,0, &pars) };
	m_pDeviceContext->DiscardView1 (m_pRenderTargetView, nullptr, 0);
	m_pDeviceContext->DiscardView1 (m_pDepthStencilView, nullptr, 0);
	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	{
		HandleDeviceLost ();
	}
	else
	{
		GAME_COMC (hr);
	}
}

void ResourceHandler::Size (WindowSize _size)
{
	m_Size = _size;
	if (!m_pDevice || !m_pDeviceContext)
	{
		GAME_THROW ("Device not created");
	}
	bool recreated{ false };
	if (m_pSwapChain)
	{
		ReleaseCOM (m_pRenderTargetView);
		ReleaseCOM (m_pDepthStencilView);
		const HRESULT hr{ m_pSwapChain->ResizeBuffers (2, _size.width, _size.height, SWAP_CHAIN_FORMAT, 0) };
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
		CreateSwapChain (_size);
	}
	if (!recreated)
	{
		CreateRenderTarget (_size);
		CreateDepthStencilView (_size);
		SetOutputMergerViews ();
		SetOutputMergerViewport (_size);
	}
	FIRE_EVENT (OnSized (_size));
}

void ResourceHandler::Destroy ()
{
	FIRE_EVENT (OnDeviceDestroyed ());
	Release ();
}

void ResourceHandler::SetWindow (NativeWindow _nativeWindow)
{
	if (!m_pDevice)
	{
		CreateDeviceAndDeviceContext ();
		FIRE_EVENT (OnDeviceCreated ());
	}
	m_NativeWindow = _nativeWindow;
}

void ResourceHandler::Suspend ()
{
	if (m_pDevice)
	{
		IDXGIDevice3 * pDevice;
		GAME_COMC (m_pDevice->QueryInterface (__uuidof(IDXGIDevice3), reinterpret_cast<void**>(&pDevice)));
		pDevice->Trim ();
		pDevice->Release ();
	}
}

ID3D11Device * ResourceHandler::GetDevice ()
{
	return m_pDevice;
}

ID3D11DeviceContext * ResourceHandler::GetDeviceContext ()
{
	return m_pDeviceContext;
}

ID3D11RenderTargetView * ResourceHandler::GetRenderTargetView ()
{
	return m_pRenderTargetView;
}

ID3D11DepthStencilView * ResourceHandler::GetDepthStencilView ()
{
	return m_pDepthStencilView;
}

WindowSize ResourceHandler::GetSize () const
{
	return m_Size;
}

double ResourceHandler::QueryTimerFrequency ()
{
	LARGE_INTEGER frequency;
	if (!QueryPerformanceFrequency (&frequency))
	{
		GAME_THROW_MSG ("Timer frequency query failed");
	}
	return static_cast<double>(frequency.QuadPart);
}

void ResourceHandler::CreateDeviceAndDeviceContext ()
{
	ReleaseCOM (m_pDeviceContext);
	ReleaseCOM (m_pDevice);
	const D3D_FEATURE_LEVEL featureLevels[]{ D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0 };
	UINT flags{ D3D11_CREATE_DEVICE_SINGLETHREADED };
#ifdef _DEBUG
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	HRESULT hr{ D3D11CreateDevice (nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, featureLevels, ARRAYSIZE (featureLevels), D3D11_SDK_VERSION, &m_pDevice, &m_supportedFeatureLevel, reinterpret_cast<ID3D11DeviceContext**>(&m_pDeviceContext)) };
	if (FAILED (hr))
	{
		GAME_COMC (D3D11CreateDevice (nullptr, D3D_DRIVER_TYPE_WARP, nullptr, 0, featureLevels, ARRAYSIZE (featureLevels), D3D11_SDK_VERSION, &m_pDevice, &m_supportedFeatureLevel, reinterpret_cast<ID3D11DeviceContext**>(&m_pDeviceContext)));
	}
}

void ResourceHandler::CreateSwapChain (WindowSize _bufferSize)
{
	ReleaseCOM (m_pSwapChain);
	IDXGIFactory2* pFactory;
	{
		IDXGIDevice * pDevice;
		IDXGIAdapter * pAdapter;
		GAME_COMC (m_pDevice->QueryInterface (__uuidof(IDXGIDevice), reinterpret_cast<void**>(&pDevice)));
		GAME_COMC (pDevice->GetAdapter (&pAdapter));
		GAME_COMC (pAdapter->GetParent (_uuidof (IDXGIFactory2), reinterpret_cast<void**>(&pFactory)));
		ReleaseCOM (pAdapter);
		ReleaseCOM (pDevice);
	}
	DXGI_SWAP_CHAIN_DESC1 desc;
	desc.BufferCount = 2;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Width = _bufferSize.width;
	desc.Height = _bufferSize.height;
	desc.Scaling = DXGI_SCALING_STRETCH;
	desc.Format = SWAP_CHAIN_FORMAT;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.Stereo = FALSE;
	desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	desc.Flags = 0;
#if GAME_PLATFORM == GAME_PLATFORM_WIN32
	desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	GAME_COMC (pFactory->CreateSwapChainForHwnd (m_pDevice, m_NativeWindow, &desc, nullptr, nullptr, &m_pSwapChain));
#elif GAME_PLATFORM == GAME_PLATFORM_UWP
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	GAME_COMC (pFactory->CreateSwapChainForCoreWindow (m_pDevice, m_NativeWindow, &desc, nullptr, &m_pSwapChain));
#endif
	ReleaseCOM (pFactory);
}

void ResourceHandler::CreateRenderTarget (WindowSize _viewSize)
{
	ReleaseCOM (m_pRenderTargetView);
	ID3D11Texture2D * pTexture;
	GAME_COMC (m_pSwapChain->GetBuffer (0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pTexture)));
	GAME_COMC (m_pDevice->CreateRenderTargetView (pTexture, nullptr, &m_pRenderTargetView));
	ReleaseCOM (pTexture);
}

void ResourceHandler::CreateDepthStencilView (WindowSize _viewSize)
{
	ReleaseCOM (m_pDepthStencilView);
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = _viewSize.width;
	desc.Height = _viewSize.height;
	desc.ArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_D16_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	ID3D11Texture2D * pTexture;
	GAME_COMC (m_pDevice->CreateTexture2D (&desc, nullptr, &pTexture));
	GAME_COMC (m_pDevice->CreateDepthStencilView (pTexture, nullptr, &m_pDepthStencilView));
	ReleaseCOM (pTexture);
}

void ResourceHandler::SetOutputMergerViews ()
{
	m_pDeviceContext->OMSetRenderTargets (1, &m_pRenderTargetView, m_pDepthStencilView);
}

void ResourceHandler::SetOutputMergerViewport (WindowSize _viewportSize)
{
	D3D11_VIEWPORT viewport;
	viewport.Width = static_cast<FLOAT>(_viewportSize.width);
	viewport.Height = static_cast<FLOAT>(_viewportSize.height);
	viewport.MaxDepth = 1.0f;
	viewport.MinDepth = 0.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	m_pDeviceContext->RSSetViewports (1, &viewport);
}

void ResourceHandler::HandleDeviceLost ()
{
	FIRE_EVENT (OnDeviceDestroyed ());
	Release ();
	CreateDeviceAndDeviceContext ();
	FIRE_EVENT (OnDeviceCreated ());
	CreateSwapChain (m_Size);
	CreateRenderTarget (m_Size);
	CreateDepthStencilView (m_Size);
	SetOutputMergerViews ();
	SetOutputMergerViewport (m_Size);
}

void ResourceHandler::Release ()
{
	ReleaseCOM (m_pSwapChain);
	ReleaseCOM (m_pRenderTargetView);
	ReleaseCOM (m_pDepthStencilView);
	ReleaseCOM (m_pDevice);
	if (m_pDeviceContext)
	{
		m_pDeviceContext->ClearState ();
		m_pDeviceContext->Flush ();
		m_pDeviceContext->Release ();
	}
}
