#include <Game/Rendering/LightingPass.hpp>

#include <Game/Utils/COMExceptions.hpp>

#define _DEPTH_SLOT (0)
#define _NORMAL_SLOT (1)
#define _MATERIAL_SLOT (2)
#define _MAP_STARTING_SLOT (3)
#define _LIGHTBUFFER_SLOT 0
#define _TRANSFORMBUFFER_SLOT 1

LightingPass::LightingPass (const GeometryPass & _geometryPass) : m_ShadowingSubPass { _geometryPass } {}

void LightingPass::Create (ID3D11Device & _device)
{
	m_DirectionalShader.Create (_device);
	m_ConeShader.Create (_device);
	m_PointShader.Create (_device);
	m_LightBuffer.Create (_device);
	m_TransformBuffer.Create (_device);
	m_ShadowingSubPass.Create (_device);
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
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.MinLOD = 0;
		desc.MaxLOD = D3D11_FLOAT32_MAX;
		GAME_COMC (_device.CreateSamplerState (&desc, m_SamplerState.put ()));
	}
	{
		D3D11_SAMPLER_DESC desc {};
		desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.BorderColor[0] = desc.BorderColor[1] = desc.BorderColor[2] = desc.BorderColor[3] = 1.0f;
		desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
		desc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
		desc.MinLOD = 0;
		desc.MaxLOD = D3D11_FLOAT32_MAX;
		GAME_COMC (_device.CreateSamplerState (&desc, m_SamplerComparisonState.put ()));
	}
}

void LightingPass::Destroy ()
{
	m_LightBuffer.Destroy ();
	m_TransformBuffer.Destroy ();
	m_DirectionalShader.Destroy ();
	m_ConeShader.Destroy ();
	m_PointShader.Destroy ();
	m_ShadowingSubPass.Destroy ();
	m_RasterizerState = nullptr;
	m_SamplerState = nullptr;
	m_SamplerComparisonState = nullptr;
}

bool LightingPass::IsCreated () const
{
	return m_DirectionalShader.IsCreated ();
}

void LightingPass::Load ()
{
	m_DirectionalShader.Load ();
	m_ConeShader.Load ();
	m_PointShader.Load ();
}

void LightingPass::Unload ()
{
	m_DirectionalShader.Unload ();
	m_ConeShader.Unload ();
	m_PointShader.Unload ();
}

bool LightingPass::IsLoaded () const
{
	return m_DirectionalShader.IsLoaded ();
}

void LightingPass::Render (const Scene & _scene, ID3D11DeviceContext & _context, const Inputs& _inputs, ID3D11RenderTargetView * _target)
{
	{
		_context.RSSetState (m_RasterizerState.get ());
		ID3D11SamplerState * pSamplerState[] { m_SamplerState.get (), m_SamplerComparisonState.get () };
		_context.PSSetSamplers (0, ARRAYSIZE (pSamplerState), pSamplerState);
	}

	DirectX::XMFLOAT4X4 invProjView;
	{
		DirectX::XMMATRIX proj { *_scene.pProjection };
		DirectX::XMMATRIX view { *_scene.pView };
		DirectX::XMStoreFloat4x4 (&invProjView, DirectX::XMMatrixInverse (nullptr, DirectX::XMMatrixMultiply (proj, view)));
	}

	std::list<const Light*> pLights;
	for (const Light & light : _scene.directionalLights) { pLights.push_back (&light); }
	for (const Light & light : _scene.pointLights) { pLights.push_back (&light); }
	for (const Light & light : _scene.coneLight) { pLights.push_back (&light); }

	ShadowingSubPass::ProcessLimits limits;
	limits.cone.cTotal = s_cConeTotal;
	limits.cone.cWithShadow = s_cConeWithShadow;
	limits.directional.cTotal = s_cDirectionalTotal;
	limits.directional.cWithShadow = s_cDirectionalWithShadow;
	limits.point.cTotal = s_cPointTotal;
	limits.point.cWithShadow = s_cPointWithShadow;

	while (!pLights.empty ())
	{
		ShadowingSubPass::ProcessOutput processedLights { m_ShadowingSubPass.ProcessLights (_context, _scene.drawables, pLights, limits) };

		_inputs.mesh->SetBuffer (_context);
		_context.OMSetRenderTargets (1, &_target, nullptr);
		_inputs.screenShader->SetShaderAndInputLayout (_context);
		m_TransformBuffer.SetForPixelShader (_context, _TRANSFORMBUFFER_SLOT);
		m_LightBuffer.SetForPixelShader (_context, _LIGHTBUFFER_SLOT);

		{
			ID3D11ShaderResourceView * views[3];
			views[_DEPTH_SLOT] = _inputs.depth;
			views[_NORMAL_SLOT] = _inputs.normals;
			views[_MATERIAL_SLOT] = _inputs.material;
			_context.PSSetShaderResources (0, 3, views);
		}

		ID3D11ShaderResourceView *pShadowMapResources[s_cMaxWithShadow] {};
		int iLight, iTransform;

		{
			iLight = iTransform = 0;
			if (!processedLights.directionalLights.empty ())
			{
				m_DirectionalBufferData.cLights = static_cast<UINT>(processedLights.directionalLights.size ());
				m_DirectionalBufferData.invProjView = invProjView;

				for (const ShadowingSubPass::ProcessedLight<DirectionalLight> & processedLight : processedLights.directionalLights)
				{
					const DirectionalLight & light { *processedLight.pLight };
					pShadowMapResources[iLight] = processedLight.pShadowMapShaderResource;

					if (light.bCastShadows)
					{
						m_TransformBuffer.data.transforms[iTransform++] = light;
					}

					DirectionalLightBufferData & packedLight { m_DirectionalBufferData.lights[iLight] };

					packedLight.shadowMapSize = processedLight.shadowMapSize;
					packedLight.color = light.color;
					packedLight.intensity = light.intensity;
					packedLight.direction = light.direction;

					iLight++;
				}

				m_DirectionalShader.SetShader (_context);

				_context.PSSetShaderResources (_MAP_STARTING_SLOT, s_cMaxWithShadow, pShadowMapResources);

				m_LightBuffer.Update (_context, &m_DirectionalBufferData, m_DirectionalBufferData.GetSize ());
				m_TransformBuffer.Update (_context, TransformBufferData::GetSize (iTransform));

				_context.Draw (ScreenMeshResource::GetVerticesCount (), 0);
			}

			if (!processedLights.coneLights.empty ())
			{
				for (int iMap { 0 }; iMap < iLight; iMap++)
				{
					pShadowMapResources[iMap] = nullptr;
				}
				iLight = iTransform = 0;

				m_ConeBufferData.cLights = static_cast<UINT>(processedLights.coneLights.size ());
				m_ConeBufferData.invProjView = invProjView;

				for (const ShadowingSubPass::ProcessedLight<ConeLight> & processedLight : processedLights.coneLights)
				{
					const ConeLight & light { *processedLight.pLight };
					pShadowMapResources[iLight] = processedLight.pShadowMapShaderResource;

					if (light.bCastShadows)
					{
						m_TransformBuffer.data.transforms[iTransform++] = light;
					}

					ConeLightBufferData & packedLight { m_ConeBufferData.lights[iLight] };

					packedLight.shadowMapSize = processedLight.shadowMapSize;
					packedLight.color = light.color;
					packedLight.intensity = light.intensity;
					packedLight.direction = light.direction;
					packedLight.innerCutoff = std::cosf (light.innerAngle);
					packedLight.outerCutoff = std::cosf (light.outerAngle);
					packedLight.position = light.position;
					packedLight.realEndLength = light.realEndLength;
					packedLight.endLenght = light.endLength;
					packedLight.startLength = light.startLength;

					iLight++;
				}

				m_ConeShader.SetShader (_context);

				_context.PSSetShaderResources (_MAP_STARTING_SLOT, s_cMaxWithShadow, pShadowMapResources);

				m_LightBuffer.Update (_context, &m_ConeBufferData, m_ConeBufferData.GetSize ());
				m_TransformBuffer.Update (_context, TransformBufferData::GetSize (iTransform));

				_context.Draw (ScreenMeshResource::GetVerticesCount (), 0);

			}

			if (!processedLights.pointLights.empty ())
			{
				for (int iMap { 0 }; iMap < iLight; iMap++)
				{
					pShadowMapResources[iMap] = nullptr;
				}
				iLight = iTransform = 0;

				m_PointBufferData.cLights = static_cast<UINT>(processedLights.pointLights.size ());
				m_PointBufferData.invProjView = invProjView;

				for (const ShadowingSubPass::ProcessedLight<PointLight> & processedLight : processedLights.pointLights)
				{
					const PointLight & light { *processedLight.pLight };
					pShadowMapResources[iLight] = processedLight.pShadowMapShaderResource;


					if (light.bCastShadows)
					{
						m_TransformBuffer.data.transforms[iTransform++] = light;
					}

					PointLightBufferData & packedLight { m_PointBufferData.lights[iLight] };

					packedLight.shadowMapSize = processedLight.shadowMapSize;
					packedLight.color = light.color;
					packedLight.intensity = light.intensity;
					packedLight.position = light.position;
					packedLight.radius = light.radius;

					iLight++;
				}

				m_PointShader.SetShader (_context);

				_context.PSSetShaderResources (_MAP_STARTING_SLOT, s_cMaxWithShadow, pShadowMapResources);

				m_LightBuffer.Update (_context, &m_PointBufferData, m_PointBufferData.GetSize ());
				m_TransformBuffer.Update (_context, TransformBufferData::GetSize (iTransform));

				_context.Draw (ScreenMeshResource::GetVerticesCount (), 0);

			}

			for (int iMap { 0 }; iMap < iLight; iMap++)
			{
				pShadowMapResources[iMap] = nullptr;
			}
			_context.PSSetShaderResources (_MAP_STARTING_SLOT, s_cMaxWithShadow, pShadowMapResources);
		}
	}


	{
		ID3D11ShaderResourceView* views[3] {};
		_context.PSSetShaderResources (0, 3, views);
	}
}
