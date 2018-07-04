#include "Renderer.hpp"
#include "View.hpp"
#include <stdexcept>
#include "Direct3D11.h"

Renderer::Renderer (View & _view) : m_View{ _view }
{}

Renderer::~Renderer ()
{}

void Renderer::OnTick (double _deltaTime)
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
		HR (hr);
	}
}

void Renderer::OnSize (View::Size _size)
{
	if (!m_pDevice || !m_pDeviceContext)
	{
		throw std::runtime_error ("Device not created");
	}
	if (m_pSwapChain)
	{
		const HRESULT hr{ m_pSwapChain->ResizeBuffers (2, _size.width, _size.height, DXGI_FORMAT_R8G8B8A8_UNORM, 0) };
		if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
		{
			HandleDeviceLost ();
		}
		else
		{
			HR (hr);
		}
	}
	else
	{
		CreateSwapChain (_size);
	}
	CreateRenderTarget (_size);
	CreateDepthStencilView (_size);
	SetOutputMergerViews ();
	SetOutputMergerViewport (_size);
	OnSized (_size);
}

void Renderer::OnCreate ()
{
	CreateDeviceAndDeviceContext ();
	OnDeviceCreated ();
}

void Renderer::OnDestroy ()
{
	OnDeviceDestroyed ();
	ReleaseCOM (m_pSwapChain);
	ReleaseCOM (m_pRenderTargetView);
	ReleaseCOM (m_pDepthStencilView);
	ReleaseCOM (m_pDeviceContext);
	ReleaseCOM (m_pDevice);
}

ID3D11Device * Renderer::GetDevice ()
{
	return m_pDevice;
}

ID3D11DeviceContext * Renderer::GetDeviceContext ()
{
	return m_pDeviceContext;
}

ID3D11RenderTargetView * Renderer::GetRenderTargetView ()
{
	return m_pRenderTargetView;
}

ID3D11DepthStencilView * Renderer::GetDepthStencilView ()
{
	return m_pDepthStencilView;
}

void Renderer::CreateDeviceAndDeviceContext ()
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
		HR (D3D11CreateDevice (nullptr, D3D_DRIVER_TYPE_WARP, nullptr, 0, featureLevels, ARRAYSIZE (featureLevels), D3D11_SDK_VERSION, &m_pDevice, &m_supportedFeatureLevel, &m_pDeviceContext));
	}
}

void Renderer::CreateSwapChain (View::Size _bufferSize)
{
	ReleaseCOM (m_pSwapChain);
	IDXGIFactory2* pFactory;
	{
		IDXGIDevice * pDevice;
		IDXGIAdapter * pAdapter;
		HR (m_pDevice->QueryInterface (__uuidof(IDXGIDevice), reinterpret_cast<void**>(&pDevice)));
		HR (pDevice->GetAdapter (&pAdapter));
		HR (pAdapter->GetParent (_uuidof (IDXGIFactory2), reinterpret_cast<void**>(&pFactory)));
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
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	desc.Stereo = FALSE;
	desc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	desc.Flags = 0;
	m_pSwapChain = m_View.CreateSwapChain (*m_pDevice, *pFactory, desc);
	ReleaseCOM (pFactory);
}

void Renderer::CreateRenderTarget (View::Size _viewSize)
{
	ReleaseCOM (m_pRenderTargetView);
	ID3D11Texture2D * pTexture;
	HR (m_pSwapChain->GetBuffer (0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pTexture)));
	HR (m_pDevice->CreateRenderTargetView (pTexture, nullptr, &m_pRenderTargetView));
	ReleaseCOM (pTexture);
}

void Renderer::CreateDepthStencilView (View::Size _viewSize)
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
	HR (m_pDevice->CreateTexture2D (&desc, nullptr, &pTexture));
	HR (m_pDevice->CreateDepthStencilView (pTexture, nullptr, &m_pDepthStencilView));
	ReleaseCOM (pTexture);
}

void Renderer::SetOutputMergerViews ()
{
	m_pDeviceContext->OMSetRenderTargets (1, &m_pRenderTargetView, m_pDepthStencilView);
}

void Renderer::SetOutputMergerViewport (View::Size _viewportSize)
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

void Renderer::HandleDeviceLost ()
{
	OnDeviceDestroyed ();
	CreateDeviceAndDeviceContext ();
	OnDeviceCreated ();
	const View::Size size{ m_View.GetSize () };
	CreateSwapChain (size);
	CreateRenderTarget (size);
	CreateDepthStencilView (size);
	SetOutputMergerViews ();
	SetOutputMergerViewport (size);
}
