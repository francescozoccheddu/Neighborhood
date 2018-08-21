#include <Game/Rendering/LightingPass.hpp>

LightingPass::LightingPass (const GeometryPass & _geometryPass) : m_ShadowingSubPass { _geometryPass } {}

void LightingPass::Create (ID3D11Device & _device)
{
	m_DirectionalShader.Create (_device);
	m_DirectionalBuffer.Create (_device);
	//m_ShadowingSubPass.Create (_device);
}

void LightingPass::Destroy ()
{
	m_DirectionalBuffer.Destroy ();
	m_DirectionalShader.Destroy ();
	//m_ShadowingSubPass.Destroy ();
}

bool LightingPass::IsCreated () const
{
	return m_DirectionalShader.IsCreated ();
}

void LightingPass::Load ()
{
	m_DirectionalShader.Load ();
}

void LightingPass::Unload ()
{
	m_DirectionalShader.Unload ();
}

bool LightingPass::IsLoaded () const
{
	return m_DirectionalShader.IsLoaded ();
}

void LightingPass::Render (const Scene & _scene, ID3D11DeviceContext & _context, const Inputs& _inputs, ID3D11RenderTargetView * _target)
{
	std::list<const Light*> pLights;
	for (const Light & light : _scene.directionalLights) { pLights.push_back (&light); }
	for (const Light & light : _scene.pointLights) { pLights.push_back (&light); }
	for (const Light & light : _scene.coneLight) { pLights.push_back (&light); }
	//m_ShadowingSubPass.ProcessLights (_context, _scene.drawables, pLights);


	{
		_context.OMSetRenderTargets (1, &_target, nullptr);
	}

	_inputs.screenShader->SetShaderAndInputLayout (_context);
	m_DirectionalShader.SetShader (_context);

	{
		for (int iLight { 0 }; iLight < _scene.directionalLights.size (); iLight++)
		{
			m_DirectionalBuffer.data.lights[iLight].direction = _scene.directionalLights[iLight].direction;
			m_DirectionalBuffer.data.lights[iLight].color = _scene.directionalLights[iLight].color;
		}
		m_DirectionalBuffer.data.count = static_cast<UINT>(_scene.directionalLights.size ());

		m_DirectionalBuffer.Update (_context, m_DirectionalBuffer.data.GetSize ());
		m_DirectionalBuffer.SetForPixelShader (_context, 0);
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