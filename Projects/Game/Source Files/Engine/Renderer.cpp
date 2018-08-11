#include <Game/Engine/Renderer.hpp>

#include <Game/Utils/Exceptions.hpp>
#include <Game/Utils/COMExceptions.hpp>

Renderer::Renderer (const DeviceHolder & _deviceHolder) : m_DeviceHolder { _deviceHolder }
//m_GeometryShaderPass ({ { "default_vertex", ShaderResource::ShaderType::VERTEX }, { "default_pixel", ShaderResource::ShaderType::PIXEL } })
{
	//m_GeometryShaderPass.Load ();
}

void Renderer::OnDeviceCreated ()
{
	ID3D11Device & device { *m_DeviceHolder.GetDevice () };
	//m_GeometryShaderPass.Create (device);
}

void Renderer::OnDeviceDestroyed ()
{
	//m_GeometryShaderPass.Destroy ();
	m_DepthStencilView = nullptr;
	for (int iView { 0 }; iView < _GAME_RENDERER_TARGET_VIEWS_COUNT; iView++)
	{
		m_RenderTargetViews[iView] = nullptr;
	}
}

void Renderer::OnSized (WindowSize _size, DXGI_MODE_ROTATION _rotation)
{
	ID3D11Device * pDevice { m_DeviceHolder.GetDevice () };
	{
		// Depth stencil view
		m_DepthStencilView = nullptr;
		D3D11_TEXTURE2D_DESC desc;
		desc.Width = _size.width;
		desc.Height = _size.height;
		desc.ArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_D16_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		com_ptr<ID3D11Texture2D> texture;
		GAME_COMC (pDevice->CreateTexture2D (&desc, nullptr, texture.put ()));
		GAME_COMC (pDevice->CreateDepthStencilView (texture.get (), nullptr, m_DepthStencilView.put ()));
	}
	{
		// Render target views
		for (int iView { 0 }; iView < _GAME_RENDERER_TARGET_VIEWS_COUNT; iView++)
		{
			m_RenderTargetViews[iView] = nullptr;
			D3D11_TEXTURE2D_DESC desc;
			desc.Width = _size.width;
			desc.Height = _size.height;
			desc.ArraySize = 1;
			desc.MipLevels = 1;
			desc.Format = DXGI_FORMAT_D16_UNORM;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_RENDER_TARGET;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;
			m_DepthStencilView = nullptr;
			com_ptr<ID3D11Texture2D> texture;
			GAME_COMC (pDevice->CreateTexture2D (&desc, nullptr, texture.put ()));
			GAME_COMC (pDevice->CreateRenderTargetView (texture.get (), nullptr, m_RenderTargetViews[iView].put ()));
		}
	}
	{
		// Viewport
		m_Viewport.Width = static_cast<FLOAT>(_size.width);
		m_Viewport.Height = static_cast<FLOAT>(_size.height);
		m_Viewport.MaxDepth = 1.0f;
		m_Viewport.MinDepth = 0.0f;
		m_Viewport.TopLeftX = 0.0f;
		m_Viewport.TopLeftY = 0.0f;
	}
}

void Renderer::Render (const Scene & _scene)
{
	float color[4] { 0.0f, 1.0f, 0.0f, 1.0f };
	m_DeviceHolder.GetDeviceContext ()->ClearRenderTargetView (m_DeviceHolder.GetRenderTargetView (), color);
}