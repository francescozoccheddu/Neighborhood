#include <Game/Engine/Renderer.hpp>

#include <Game/Utils/Exceptions.hpp>
#include <Game/Utils/COMExceptions.hpp>
#include <Game/DirectXMath.hpp>

#define GEOMETRY_SHADER_PASS {RES_SHADER_FILENAME ("geometry_vertex"), RES_SHADER_FILENAME ("geometry_pixel"), SceneMeshResource::s_aInputElementDesc, ARRAYSIZE(SceneMeshResource::s_aInputElementDesc)}
#define LIGHTING_SHADER_PASS {RES_SHADER_FILENAME ("lighting_vertex"), RES_SHADER_FILENAME ("lighting_pixel"), ScreenMeshResource::s_aInputElementDesc, ARRAYSIZE(ScreenMeshResource::s_aInputElementDesc)}

Renderer::Renderer (const DeviceHolder & _deviceHolder) : m_DeviceHolder { _deviceHolder }, m_GeometryShaderPass GEOMETRY_SHADER_PASS, m_LightingShaderPass LIGHTING_SHADER_PASS
{
	m_GeometryShaderPass.Load ();
m_LightingShaderPass.Load ();
}

void Renderer::OnDeviceCreated ()
{
	ID3D11Device & device { *m_DeviceHolder.GetDevice () };
	m_GeometryShaderPass.Create (device);
	m_LightingShaderPass.Create (device);
	m_SceneResources.Create (device);
	m_ScreenMesh.Create (device);
	m_constantBufferGeometry.Create (device);
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
}

void Renderer::OnDeviceDestroyed ()
{
	m_SamplerState = nullptr;
	m_constantBufferGeometry.Destroy ();
	m_SceneResources.Destroy ();
	m_GeometryShaderPass.Destroy ();
	m_LightingShaderPass.Destroy ();
	m_DepthStencilView = nullptr;
	m_ScreenMesh.Destroy ();
	for (int iView { 0 }; iView < s_cRenderTargets; iView++)
	{
		m_RenderTargetViews[iView] = nullptr;
		m_ShaderResourceViews[iView] = nullptr;
	}
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

	{
		// Set viewport, primitive topology and sampler state
		context.RSSetViewports (1, &m_Viewport);
		context.IASetPrimitiveTopology (D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		ID3D11SamplerState *states[] { m_SamplerState.get () };
		context.PSSetSamplers (0, 1, states);
	}

	{
		// Set shader pass
		m_GeometryShaderPass.Set (context);
	}

	{
		// Clear and set deferred render targets
		context.ClearDepthStencilView (m_DepthStencilView.get (), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		const float color[4] { 0.0f, 0.0f, 0.0f, 0.0f };
		ID3D11RenderTargetView * views[s_cRenderTargets];
		for (int iView { 0 }; iView < s_cRenderTargets; iView++)
		{
			views[iView] = m_RenderTargetViews[iView].get ();
			context.ClearRenderTargetView (m_RenderTargetViews[iView].get (), color);
		}
		context.OMSetRenderTargets (s_cRenderTargets, views, m_DepthStencilView.get ());
	}

	{
		// Set per-frame constant buffer
		m_constantBufferGeometry.data.projection = _scene.projection.Get ();
		m_constantBufferGeometry.data.view = _scene.pView->Get ();
		m_constantBufferGeometry.Update (context);
		m_constantBufferGeometry.SetForVertexShader (context, 0);
	}


	for (const Scene::Drawable& drawable : _scene.drawables)
	{

		const SceneMeshResource & mesh { m_SceneResources.GetMesh (GetMeshFileName (drawable.mesh)) };

		mesh.SetBuffers (context);

		const TextureResource & texture { m_SceneResources.GetTexture (GetTextureFileName (drawable.material.name)) };

		texture.SetShaderResourceView (context, 0);

		context.DrawIndexed (static_cast<UINT>(mesh.GetIndicesCount ()), 0, 0);
	}

	{
		// Set shader pass
		m_LightingShaderPass.Set (context);
	}

	{
		// Clear and set output render target
		float color[4] { 0.2f, 0.2f, 0.2f, 1.0f };
		ID3D11RenderTargetView * const pRenderTargetView { m_DeviceHolder.GetRenderTargetView () };
		context.ClearRenderTargetView (pRenderTargetView, color);
		context.OMSetRenderTargets (1, &pRenderTargetView, nullptr);
	}

	{
		ID3D11ShaderResourceView * views[s_cRenderTargets];
		for (int iView { 0 }; iView < s_cRenderTargets; iView++)
		{
			views[iView] = m_ShaderResourceViews[iView].get ();
		}
		context.PSSetShaderResources (0, s_cRenderTargets, views);
	}

	m_ScreenMesh.SetBuffer (context);
	context.Draw (m_ScreenMesh.GetVerticesCount (), 0);

}