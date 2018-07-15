#include "ResourceHandler.hpp"
#include "View.hpp"
#include "Exceptions.hpp"
#include "Utils.hpp"
#include "Direct3D11.h"

#define SWAP_CHAIN_FORMAT DXGI_FORMAT_R8G8B8A8_UNORM

ResourceHandler::ResourceHandler (View & _view) : m_View{ _view }
{}

ResourceHandler::~ResourceHandler ()
{}

void ResourceHandler::OnTick (double _deltaTime)
{
	if (!m_pSwapChain)
	{
		throw std::runtime_error ("Swap chain not created");
	}
	OnRender (_deltaTime);
	const HRESULT hr{ m_pSwapChain->Present (1,0) };
	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	{
		HandleDeviceLost ();
	}
	else
	{
		GAME_COMC (hr);
	}
}

void ResourceHandler::OnSize (View::Size _size)
{
	GAME_LOG ("OnSize");
	m_Size = _size;
	if (!m_pDevice || !m_pDeviceContext)
	{
		GAME_THROW ("Device not created");
	}
	bool recreated{ false };
	if (m_pSwapChain)
	{
		GAME_LOG ("Resizing buffers");
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
		GAME_LOG ("Creating swap chain");
		CreateSwapChain (_size);
	}
	if (!recreated)
	{
		CreateRenderTarget (_size);
		CreateDepthStencilView (_size);
		SetOutputMergerViews ();
		SetOutputMergerViewport (_size);
	}
	OnSized (_size);
}

void ResourceHandler::OnCreate ()
{
	CreateDeviceAndDeviceContext ();
	OnDeviceCreated ();
}

void ResourceHandler::OnDestroy ()
{
	OnDeviceDestroyed ();
	Release ();
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

View::Size ResourceHandler::GetSize () const
{
	return m_Size;
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
	HRESULT hr{ D3D11CreateDevice (nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, featureLevels, ARRAYSIZE (featureLevels), D3D11_SDK_VERSION, &m_pDevice, &m_supportedFeatureLevel, &m_pDeviceContext) };
	if (FAILED (hr))
	{
		GAME_COMC (D3D11CreateDevice (nullptr, D3D_DRIVER_TYPE_WARP, nullptr, 0, featureLevels, ARRAYSIZE (featureLevels), D3D11_SDK_VERSION, &m_pDevice, &m_supportedFeatureLevel, &m_pDeviceContext));
	}
}

void ResourceHandler::CreateSwapChain (View::Size _bufferSize)
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
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	desc.Stereo = FALSE;
	desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	desc.Flags = 0;
	m_pSwapChain = m_View.CreateSwapChain (*m_pDevice, *pFactory, desc);
	ReleaseCOM (pFactory);
}

void ResourceHandler::CreateRenderTarget (View::Size _viewSize)
{
	ReleaseCOM (m_pRenderTargetView);
	ID3D11Texture2D * pTexture;
	GAME_COMC (m_pSwapChain->GetBuffer (0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pTexture)));
	GAME_COMC (m_pDevice->CreateRenderTargetView (pTexture, nullptr, &m_pRenderTargetView));
	ReleaseCOM (pTexture);
}

void ResourceHandler::CreateDepthStencilView (View::Size _viewSize)
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
	m_pDeviceContext->OMSetRenderTargets (1, &m_pRenderTargetView, nullptr);
}

void ResourceHandler::SetOutputMergerViewport (View::Size _viewportSize)
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
	GAME_LOG ("Device lost");
	OnDeviceDestroyed ();
	Release ();
	CreateDeviceAndDeviceContext ();
	OnDeviceCreated ();
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
