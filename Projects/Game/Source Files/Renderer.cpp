#include <Game/Rendering/Renderer.hpp>

#include <Game/Utils/Exceptions.hpp>
#include <Game/Utils/COMExceptions.hpp>
#include <Game/DirectXMath.hpp>


Renderer::Renderer (const DeviceHolder & _deviceHolder) : m_DeviceHolder { _deviceHolder }
{
	m_ScreenShader.Load ();
	m_GeometryPass.Load ();
	m_DirectionalLightingPass.Load ();
}

Renderer::~Renderer ()
{
	m_ScreenShader.Unload ();
	m_GeometryPass.Unload ();
	m_DirectionalLightingPass.Unload ();
}

void Renderer::OnDeviceCreated ()
{
	ID3D11Device & device { *m_DeviceHolder.GetDevice () };

	m_GeometryPass.Create (device);
	m_DirectionalLightingPass.Create (device);
	m_ScreenMesh.Create (device);
	{
		D3D11_SAMPLER_DESC desc {};
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.MinLOD = 0;
		desc.MaxLOD = D3D11_FLOAT32_MAX;
		GAME_COMC (device.CreateSamplerState (&desc, m_SamplerState.put ()));
	}
	m_ScreenShader.Create (device);
}

void Renderer::OnDeviceDestroyed ()
{
	m_SamplerState = nullptr;
	m_DepthStencilView = nullptr;
	m_ScreenMesh.Destroy ();
	for (int iView { 0 }; iView < s_cRenderTargets; iView++)
	{
		m_RenderTargetViews[iView] = nullptr;
		m_ShaderResourceViews[iView] = nullptr;
	}
	m_ScreenShader.Destroy ();
	m_GeometryPass.Destroy ();
	m_DirectionalLightingPass.Destroy ();
}

void Renderer::OnSized (WindowSize _size, WindowRotation _rotation)
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
		for (int iView { 0 }; iView < s_cRenderTargets; iView++)
		{
			m_RenderTargetViews[iView] = nullptr;
			m_ShaderResourceViews[iView] = nullptr;
			D3D11_TEXTURE2D_DESC desc;
			desc.Width = _size.width;
			desc.Height = _size.height;
			desc.ArraySize = 1;
			desc.MipLevels = 1;
			desc.Format = s_renderTargetFormats[iView];
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;
			com_ptr<ID3D11Texture2D> texture;
			GAME_COMC (pDevice->CreateTexture2D (&desc, nullptr, texture.put ()));
			GAME_COMC (pDevice->CreateRenderTargetView (texture.get (), nullptr, m_RenderTargetViews[iView].put ()));
			GAME_COMC (pDevice->CreateShaderResourceView (texture.get (), nullptr, m_ShaderResourceViews[iView].put ()));
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

	ID3D11DeviceContext & context { *m_DeviceHolder.GetDeviceContext () };
	ID3D11RenderTargetView * const pRenderTargetView { m_DeviceHolder.GetRenderTargetView () };

	{
		// Set viewport, primitive topology and sampler state
		context.RSSetViewports (1, &m_Viewport);
		context.IASetPrimitiveTopology (D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		ID3D11SamplerState *states[] { m_SamplerState.get () };
		context.PSSetSamplers (0, 1, states);
	}

	{
		// Clear and set output render target
		float color[4] { 0.2f, 0.2f, 0.2f, 1.0f };
		context.ClearRenderTargetView (pRenderTargetView, color);
	}

	{
		GeometryPass::Target target;
		target.colors = m_RenderTargetViews[s_iColorTexture].get ();
		target.normals = m_RenderTargetViews[s_iNormalTexture].get ();
		target.material = m_RenderTargetViews[s_iMaterialTexture].get ();
		target.depth = m_DepthStencilView.get ();
		m_GeometryPass.Render (_scene, context, target);
	}

	{
		DirectionalLightingPass::Inputs inputs;
		inputs.material = m_ShaderResourceViews[s_iMaterialTexture].get ();
		inputs.normals = m_ShaderResourceViews[s_iNormalTexture].get ();
		inputs.mesh = &m_ScreenMesh;
		inputs.screenShader = &m_ScreenShader;
		m_DirectionalLightingPass.Render (_scene.directionalLights, context, inputs, pRenderTargetView);
	}

}