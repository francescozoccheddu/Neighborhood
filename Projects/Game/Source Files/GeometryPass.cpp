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
}

void GeometryPass::Destroy ()
{
	m_ConstantBuffer.Destroy ();
	m_VertexShader.Destroy ();
	m_PixelShader.Destroy ();
	m_RasterizerState = nullptr;
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

void GeometryPass::Render (ID3D11DeviceContext & _context, const Scene & _scene, const Target& _target) const
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

	{
		ConstantBuffer buffer;
		buffer.projView = *_scene.pProjection * *_scene.pView;
		m_ConstantBuffer.data = buffer;
		m_ConstantBuffer.Update (_context);
		m_ConstantBuffer.SetForVertexShader (_context, 0);
	}

	for (const Scene::Drawable& drawable : _scene.drawables)
	{
		const SceneMeshResource & mesh { *drawable.pMesh };
		mesh.SetBuffers (_context, false);

		_context.DrawIndexed (static_cast<UINT>(mesh.GetIndicesCount ()), 0, 0);
	}

	_context.VSSetShader (nullptr, nullptr, 0);
	_context.PSSetShader (nullptr, nullptr, 0);
}

