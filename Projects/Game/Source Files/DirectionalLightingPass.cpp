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

	_inputs.screenShader->SetShaderAndInputLayout (_context);
	m_Shader.SetShader (_context);

	{
		m_ConstantBuffer.data.lights[0].direction = _lights[0].direction;
		m_ConstantBuffer.data.lights[0].color = { 1.0f, 0.0f, 0.5f };
		m_ConstantBuffer.Update (_context);
		m_ConstantBuffer.SetForPixelShader (_context, 0);
	}

	{
		ID3D11ShaderResourceView * views[2];
		views[0] = _inputs.normals;
		views[1] = _inputs.material;
		_context.PSSetShaderResources (0, 2, views);
	}

	_inputs.mesh->SetBuffer (_context);
	_context.Draw (ScreenMeshResource::GetVerticesCount (), 0);
}