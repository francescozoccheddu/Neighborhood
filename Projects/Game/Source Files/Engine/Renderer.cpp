#include <Game/Engine/Renderer.hpp>

#include <Game/Utils/Exceptions.hpp>
#include <Game/Utils/COMExceptions.hpp>
#include <Game/DirectXMath.hpp>

#define GEOMETRY_SHADER_PASS {GetShaderFileName ("default_vertex"), GetShaderFileName ("default_pixel")}

struct CbPerFrame
{
	DirectX::XMMATRIX projection;
	DirectX::XMMATRIX view;
};

Renderer::Renderer (const DeviceHolder & _deviceHolder) : m_DeviceHolder { _deviceHolder }, m_GeometryShaderPass GEOMETRY_SHADER_PASS
{
	m_GeometryShaderPass.Load ();
}

void Renderer::OnDeviceCreated ()
{
	ID3D11Device & device { *m_DeviceHolder.GetDevice () };
	m_GeometryShaderPass.Create (device);
	m_SceneResources.Create (device);
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
	{
		D3D11_BUFFER_DESC desc;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.ByteWidth = sizeof (CbPerFrame);
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		GAME_COMC (device.CreateBuffer (&desc, nullptr, m_ConstantBufferPerFrame.put ()));
	}
}

void Renderer::OnDeviceDestroyed ()
{
	m_SamplerState = nullptr;
	m_ConstantBufferPerFrame = nullptr;
	m_SceneResources.Destroy ();
	m_GeometryShaderPass.Destroy ();
	m_DepthStencilView = nullptr;
	for (int iView { 0 }; iView < _GAME_RENDERER_TARGET_VIEWS_COUNT; iView++)
	{
		m_RenderTargetViews[iView] = nullptr;
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

	ID3D11DeviceContext & context { *m_DeviceHolder.GetDeviceContext () };

	m_GeometryShaderPass.Set (context);

	ID3D11RenderTargetView * pRenderTarget { m_DeviceHolder.GetRenderTargetView () };

	context.OMSetRenderTargets (1, &pRenderTarget, m_DepthStencilView.get ());

	context.RSSetViewports (1, &m_Viewport);
	context.IASetPrimitiveTopology (D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	float color[4] { 0.2f, 0.2f, 0.2f, 1.0f };
	context.ClearRenderTargetView (m_DeviceHolder.GetRenderTargetView (), color);
	context.ClearDepthStencilView (m_DepthStencilView.get (), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	CbPerFrame cbPerFrame;
	cbPerFrame.projection = _scene.projection.Get ();
	cbPerFrame.view = _scene.pView->Get ();

	context.UpdateSubresource (m_ConstantBufferPerFrame.get (), 0, nullptr, &cbPerFrame, 0, 0);
	ID3D11Buffer *buffers[] { m_ConstantBufferPerFrame.get () };
	context.VSSetConstantBuffers (0, 1, buffers);

	ID3D11SamplerState *states[] { m_SamplerState.get () };
	context.PSSetSamplers (0, 1, states);

	for (const Scene::Drawable& drawable : _scene.drawables)
	{

		const MeshResource & mesh { m_SceneResources.GetMesh (GetMeshFileName (drawable.mesh)) };

		mesh.SetBuffers (context);

		const TextureResource & texture { m_SceneResources.GetTexture (GetTextureFileName (drawable.material.name)) };

		texture.SetShaderResourceView (context, 0);


		context.DrawIndexed (static_cast<UINT>(mesh.GetIndicesCount ()), 0, 0);

	}

}