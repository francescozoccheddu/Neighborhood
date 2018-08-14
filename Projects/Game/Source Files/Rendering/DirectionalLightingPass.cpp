#include <Game/Rendering/DirectionalLightingPass.hpp>

void DirectionalLightingPass::Create (ID3D11Device & _device)
{
	m_Shader.Create (_device);
	m_ConstantBuffer.Create (_device);
}

void DirectionalLightingPass::Destroy ()
{
	m_ConstantBuffer.Destroy ();
	m_Shader.Destroy ();
}

bool DirectionalLightingPass::IsCreated () const
{
	return m_Shader.IsCreated ();
}

void DirectionalLightingPass::Load ()
{
	m_Shader.Load ();
}

void DirectionalLightingPass::Unload ()
{
	m_Shader.Unload ();
}

bool DirectionalLightingPass::IsLoaded () const
{
	return m_Shader.IsLoaded ();
}

void DirectionalLightingPass::Render (const std::vector<Scene::DirectionalLight> & _lights, ID3D11DeviceContext & _context, const Inputs& _inputs, ID3D11RenderTargetView * _target)
{
	{
		_context.OMSetRenderTargets (1, &_target, nullptr);
	}

	{
		//m_ConstantBuffer.data.light = _lights[0];
		m_ConstantBuffer.Update (_context);
		m_ConstantBuffer.SetForPixelShader (_context, 0);
	}

	_inputs.screenShader->SetShaderAndInputLayout (_context);
	m_Shader.SetShader (_context);

	{
		ID3D11ShaderResourceView * views[2];
		views[0] = _inputs.normals;
		views[1] = _inputs.material;
		_context.PSSetShaderResources (0, 2, views);
	}

	_inputs.mesh->SetBuffer (_context);
	_context.Draw (ScreenMeshResource::GetVerticesCount (), 0);
}