#include <Game/Rendering/GeometryPass.hpp>
#include "..\Header Files\Game\Rendering\GeometryPass.hpp"

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

void GeometryPass::Render (const Scene & _scene, ID3D11DeviceContext & _context, const Target& _target) const
{
	ConstantBuffer buffer;
	buffer.projView = *_scene.pProjection * *_scene.pView;
	Render (_scene.drawables, _context, buffer, _target);
}

void GeometryPass::Render (const std::vector<Scene::Drawable>& _drawables, ID3D11DeviceContext & _context, const ConstantBuffer & _buffer, const Target & _target) const
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
		m_ConstantBuffer.data = _buffer;
		m_ConstantBuffer.Update (_context);
		m_ConstantBuffer.SetForVertexShader (_context, 0);
	}

	Draw (_drawables, _context);
}

void GeometryPass::Render (const std::vector<Scene::Drawable>& _drawables, ID3D11DeviceContext & _context, const ConstantBuffer & _buffer, ID3D11DepthStencilView * _pDepthTarget) const
{

	{
		_context.ClearDepthStencilView (_pDepthTarget, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		_context.OMSetRenderTargets (0, nullptr, _pDepthTarget);
	}

	m_Shader.SetVertexOnly (_context);
	_context.PSSetShader (nullptr, nullptr, 0);

	{
		m_ConstantBuffer.data = _buffer;
		m_ConstantBuffer.Update (_context);
		m_ConstantBuffer.SetForVertexShader (_context, 0);
	}

	Draw (_drawables, _context);
}

void GeometryPass::Draw (const std::vector<Scene::Drawable>& _drawables, ID3D11DeviceContext & _context) const
{
	for (const Scene::Drawable& drawable : _drawables)
	{
		const SceneMeshResource & mesh { m_SceneResources.GetMesh (drawable.mesh) };
		mesh.SetBuffers (_context);

		const TextureResource & texture { m_SceneResources.GetTexture (drawable.texture) };
		texture.SetShaderResourceView (_context, 0);

		_context.DrawIndexed (static_cast<UINT>(mesh.GetIndicesCount ()), 0, 0);
	}
}
