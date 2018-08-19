#include <Game/Rendering/GeometryPass.hpp>

void GeometryPass::Create (ID3D11Device & _device)
{
	m_ConstantBuffer.Create (_device);
	m_Shader.Create (_device);
	m_SceneResources.Create (_device);
}

void GeometryPass::Destroy ()
{
	m_ConstantBuffer.Destroy ();
	m_Shader.Destroy ();
	m_SceneResources.Destroy ();
}

bool GeometryPass::IsCreated () const
{
	return m_Shader.IsCreated ();
}

void GeometryPass::Load ()
{
	m_Shader.Load ();
}

void GeometryPass::Unload ()
{
	m_Shader.Unload ();
}

bool GeometryPass::IsLoaded () const
{
	return m_Shader.IsLoaded ();
}

void GeometryPass::Render (const Scene & _scene, ID3D11DeviceContext & _context, const Target& _target)
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
		_context.ClearDepthStencilView (_target.depth, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		_context.OMSetRenderTargets (3, views, _target.depth);
	}

	m_Shader.Set (_context);

	{
		m_ConstantBuffer.data.projection = _scene.projection.Get ();
		m_ConstantBuffer.data.view = _scene.pView->Get ();
		m_ConstantBuffer.Update (_context);
		m_ConstantBuffer.SetForVertexShader (_context, 0);
	}


	for (const Scene::Drawable& drawable : _scene.drawables)
	{
		const SceneMeshResource & mesh { m_SceneResources.GetMesh (GetMeshFileName (drawable.mesh)) };
		mesh.SetBuffers (_context);

		const TextureResource & texture { m_SceneResources.GetTexture (GetTextureFileName (drawable.texture)) };
		texture.SetShaderResourceView (_context, 0);

		_context.DrawIndexed (static_cast<UINT>(mesh.GetIndicesCount ()), 0, 0);
	}
}