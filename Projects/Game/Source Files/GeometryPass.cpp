#include <Game/Rendering/GeometryPass.hpp>

#include <Game/Utils/COMExceptions.hpp>

void GeometryPass::Create (ID3D11Device & _device)
{
	m_ConstantBuffer.Create (_device);
	m_VertexShader.Create (_device);
	m_PixelShader.Create (_device);
	{
		D3D11_RASTERIZER_DESC desc {};
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_BACK;
		desc.DepthClipEnable = TRUE;
		GAME_COMC (_device.CreateRasterizerState (&desc, m_RasterizerState.put ()));
	}
	{
		D3D11_SAMPLER_DESC desc {};
		desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.BorderColor[0] = desc.BorderColor[1] = desc.BorderColor[2] = desc.BorderColor[3] = 0.0f;
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.MinLOD = 0;
		desc.MaxLOD = D3D11_FLOAT32_MAX;
		GAME_COMC (_device.CreateSamplerState (&desc, m_SamplerState.put ()));
	}
}

void GeometryPass::Destroy ()
{
	m_ConstantBuffer.Destroy ();
	m_VertexShader.Destroy ();
	m_PixelShader.Destroy ();
	m_RasterizerState = nullptr;
	m_SamplerState = nullptr;
}

bool GeometryPass::IsCreated () const
{
	return m_VertexShader.IsCreated ();
}

void GeometryPass::Load ()
{
	m_VertexShader.Load ();
	m_PixelShader.Load ();
}

void GeometryPass::Unload ()
{
	m_VertexShader.Unload ();
	m_PixelShader.Unload ();
}

bool GeometryPass::IsLoaded () const
{
	return m_VertexShader.IsLoaded ();
}

void GeometryPass::Render (ID3D11DeviceContext & _context, const SceneResources & _sceneResources, const Scene & _scene, const Target& _target) const
{
	{
		ID3D11RenderTargetView * views[3];
		{
			views[0] = _target.colors;
			const float color[4] { 0.0f, 0.0f, 0.0f, 0.0f };
			_context.ClearRenderTargetView (_target.colors, color);
		}
		{
			views[1] = _target.normals;
			const float color[4] { 0.0f, 0.0f, 0.0f, 0.0f };
			_context.ClearRenderTargetView (_target.normals, color);
		}
		{
			views[2] = _target.material;
			const float color[4] { 0.0f, 0.0f, 0.0f, 0.0f };
			_context.ClearRenderTargetView (_target.material, color);
		}
		_context.ClearDepthStencilView (_target.depth, D3D11_CLEAR_DEPTH, 1.0f, 0);
		_context.OMSetRenderTargets (3, views, _target.depth);
	}

	m_VertexShader.SetShaderAndInputLayout (_context);
	m_PixelShader.SetShader (_context);
	_context.RSSetState (m_RasterizerState.get ());
	ID3D11SamplerState * pSamplerStates[] { m_SamplerState.get () };
	_context.PSSetSamplers (0, 1, pSamplerStates);

	{
		ConstantBuffer buffer;
		buffer.projView = *_scene.pProjection * *_scene.pView;
		m_ConstantBuffer.data = buffer;
		m_ConstantBuffer.Update (_context);
		m_ConstantBuffer.SetForVertexShader (_context, 0);
	}

	for (const Scene::Drawable& drawable : _scene.drawables)
	{
		const SceneMeshResource & mesh { _sceneResources.GetMesh (drawable.mesh) };
		mesh.SetBuffers (_context, false);

		const TextureResource & texture { _sceneResources.GetTexture (drawable.texture) };
		texture.SetShaderResourceView (_context, 0);

		_context.DrawIndexed (static_cast<UINT>(mesh.GetIndicesCount ()), 0, 0);
	}

	_context.VSSetShader (nullptr, nullptr, 0);
	_context.PSSetShader (nullptr, nullptr, 0);
}

