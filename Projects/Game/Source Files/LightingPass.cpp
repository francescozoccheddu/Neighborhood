#include <Game/Rendering/LightingPass.hpp>

#include <Game/Utils/Exceptions.hpp>
#include <Game/Utils/COMExceptions.hpp>

void LightingPass::Create (ID3D11Device & _device)
{
	m_ShadowingSubPass.Create (_device);
	m_TransformBuffer.Create (_device);
	m_LightBuffer.Create (_device);
	m_PixelShader.Create (_device);
	{
		D3D11_RASTERIZER_DESC desc {};
		desc.DepthBias = 100000;
		desc.DepthBiasClamp = 0.0f;
		desc.SlopeScaledDepthBias = 1.0f;
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
		desc.BorderColor[0] = desc.BorderColor[1] = desc.BorderColor[2] = desc.BorderColor[3] = 1.0f;
		desc.MinLOD = 0;
		desc.MaxLOD = D3D11_FLOAT32_MAX;

		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		GAME_COMC (_device.CreateSamplerState (&desc, &m_apSamplerStates[0]));

		desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
		desc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
		GAME_COMC (_device.CreateSamplerState (&desc, &m_apSamplerStates[1]));
	}
}

void LightingPass::Destroy ()
{
	m_ShadowingSubPass.Destroy ();
	m_TransformBuffer.Destroy ();
	m_LightBuffer.Destroy ();
	m_PixelShader.Destroy ();
	m_RasterizerState = nullptr;
	for (auto pSamplerState : m_apSamplerStates)
	{
		pSamplerState->Release ();
	}
}

bool LightingPass::IsCreated () const
{
	return m_ShadowingSubPass.IsCreated ();
}

void LightingPass::Load ()
{
	m_ShadowingSubPass.Load ();
	m_PixelShader.Load ();
}

void LightingPass::Unload ()
{
	m_ShadowingSubPass.Unload ();
	m_PixelShader.Unload ();
}

bool LightingPass::IsLoaded () const
{
	return m_ShadowingSubPass.IsLoaded ();
}

void LightingPass::Render (ID3D11DeviceContext & _context, const SceneResources & _sceneResources, const Scene & _scene, const Inputs& _inputs, ID3D11RenderTargetView * _target)
{
	return;
	std::list<const Light*> lights { GatherLights (_scene) };
	while (!lights.empty ())
	{
		const std::vector<ShadowingSubPass::ProcessedLight> processedLights { m_ShadowingSubPass.ProcessLights (_context, _sceneResources, _scene.drawables, lights, s_cMaxLights) };
		//TODO Draw lights
	}
}

std::list<const Light*> LightingPass::GatherLights (const Scene & _scene)
{
	std::list<const Light*> lights;
	for (auto & light : _scene.coneLight)
	{
		lights.push_back (&light);
	}
	for (auto & light : _scene.directionalLights)
	{
		lights.push_back (&light);
	}
	for (auto & light : _scene.pointLights)
	{
		lights.push_back (&light);
	}
	return lights;
}
